#include <Arduino.h>
#include "stats.h"
#include "audioComponents.h"
#include "reverbs.h"
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"
#include "presetSystem.h"

#define INFOLINE_TIMEOUT 3

const char PROGMEM *termCls = "\x1b[2J";
const char PROGMEM *termCursorHide = "\x1b[?25l";
const char PROGMEM *termCursorShow = "\x1b[?25h";
const char PROGMEM *on = "\x1b[32mon \x1b[0m";
const char PROGMEM *off = "\x1b[31moff\x1b[0m";
const char PROGMEM *clrLineCursor2End = "\x1b[0K";
const char PROGMEM *clrLineBeg2Cursor = "\x1b[1K";
const char PROGMEM *clrLineAll = "\x1b[2K";
const char PROGMEM *termPosHome = "\x1b[;H";

const char PROGMEM *midiChannel_err_msg = "Please provide a value in range 1 - 16";

static uint32_t timeNow, timeLast;
static uint8_t infoLineTimeout = INFOLINE_TIMEOUT;

void writeChar(EmbeddedCli *embeddedCli, char c);
EmbeddedCliConfig *config = embeddedCliDefaultConfig();

EmbeddedCli *cli = embeddedCliNew(config);

// --- CLI callbacks ---
void cb_presetRST(EmbeddedCli *cli, char *args, void *context);
void cb_memInfo(EmbeddedCli *cli, char *args, void *context);
void cb_componentCPU(EmbeddedCli *cli, char *args, void *context);
void cb_setMidiChannel(EmbeddedCli *cli, char *args, void *context);


stats_mode_t statsMode = STATS_MODE_CONSOLE;
const char* infoLine = NULL;
stats_color_t infoLineColor = STATS_COLOR_DEFAULT;
void printStatus();
FLASHMEM void getFreeITCM();
inline void stats_reset_color();
inline void stats_set_color(uint8_t color);

char bf[20] = "";

// ---------------------------------------------------------------------
FLASHMEM void stats_init()
{
	statsMode = STATS_MODE_INFO;
	cli->writeChar = writeChar;
	embeddedCliAddBinding(cli, {
			"preset-rst",          			// command name (spaces are not allowed)
			"Restore default presets",   	// Optional help for a command (NULL for no help)
			false,              			// flag whether to tokenize arguments (see below)
			nullptr,            			// optional pointer to any application context
			cb_presetRST               		// binding function 
	});
	embeddedCliAddBinding(cli, {
			"meminfo",
			"Print memory info",
			false,
			nullptr,
			cb_memInfo
	});
	embeddedCliAddBinding(cli, {
			"load",
			"List cpu loads",
			false,
			nullptr,
			cb_componentCPU
	});
	embeddedCliAddBinding(cli, {
			"midi-ch",
			"Set MIDI channel",
			false,
			nullptr,
			cb_setMidiChannel
	});	
}
// ---------------------------------------------------------------------
FLASHMEM void stats_setMode(stats_mode_t mode)
{
	statsMode = mode;
	DBG_SERIAL.print(termCls);
	DBG_SERIAL.print(termPosHome);
}
// ---------------------------------------------------------------------
FLASHMEM void stats_tglMode()
{
	switch(statsMode)
	{
		case STATS_MODE_INFO:
			statsMode = STATS_MODE_CONSOLE;
			DBG_SERIAL.print(termCursorShow);
			break;
		case STATS_MODE_CONSOLE:
			statsMode = STATS_MODE_INFO;
			DBG_SERIAL.print(termCursorHide);
			break;
	}
	DBG_SERIAL.print(termCls);
	DBG_SERIAL.print(termPosHome);
}
// ---------------------------------------------------------------------
FLASHMEM void stats_process()
{
	switch(statsMode)
	{
		case STATS_MODE_INFO:
			timeNow = millis();
			if (timeNow - timeLast > 500)
			{
				DBG_SERIAL.print(termPosHome);
				printStatus();		
				timeLast = timeNow;
			}		
			break;
		case STATS_MODE_CONSOLE:
			while(DBG_SERIAL.available())	embeddedCliReceiveChar(cli, DBG_SERIAL.read());
			embeddedCliProcess(cli);
			break;
		default: break;
	}
}
// ---------------------------------------------------------------------
void writeChar(EmbeddedCli *embeddedCli, char c)
{
	DBG_SERIAL.write(c);
}
// ---------------------------------------------------------------------
void cb_presetRST(EmbeddedCli *cli, char *args, void *context)
{
	preset_writeDefaults();
	DBG_SERIAL.println("Done!");
}
// ---------------------------------------------------------------------
void stats_displayMsg(const char *msg, stats_color_t color)
{
	infoLineColor = color;
	infoLine = msg;
	infoLineTimeout = INFOLINE_TIMEOUT;
}
// ---------------------------------------------------------------------
inline void stats_set_color(uint8_t color) 
{
    DBG_SERIAL.print(F("\x1b["));
    DBG_SERIAL.print(30 + color);
    DBG_SERIAL.print("m");
}
// ---------------------------------------------------------------------
inline void stats_reset_color() 
{
    DBG_SERIAL.print(F("\x1b[0m"));
}
// ---------------------------------------------------------------------
FLASHMEM void printStatus()
{
	uint8_t reverbIdx = preset_getFlag(PRESET_FLAG_REVBTYPE);
	uint8_t IRno =  preset_getFlag(PRESET_FLAG_CABIR_NO);
	int8_t reverbPL_pitch = preset_getFXparam(PRESET_PARAM_REVERBPL_PITCH);
	int8_t  reverbPL_shimmPitch = preset_getFXparam(PRESET_PARAM_REVERBPL_PITCHSHIMM);
	const char* name = reverbs_getName();
	float32_t load = AudioProcessorUsageMax();

	uint16_t memUsage = AudioMemoryUsageMax_F32();

	switch(inputSwitch.getMode())
	{
		case AudioSwitchSelectorStereo::SIGNAL_SELECT_LR:
			snprintf(bf, 20, "Stereo ");
			break;
		case AudioSwitchSelectorStereo::SIGNAL_SELECT_L:
			snprintf(bf, 20, "Mono L ");
			break;
		case AudioSwitchSelectorStereo::SIGNAL_SELECT_R:
			snprintf(bf, 20, "Mono R ");
			break;
		default: break;
	}
	DBG_SERIAL.print("Input: ");
	stats_set_color(STATS_COLOR_YELLOW);
	DBG_SERIAL.print(bf);
	stats_reset_color();
	DBG_SERIAL.printf("    Comp: %s    Boost: %s    Octave: %s  \r\n", 
							preset_getFlag(PRESET_FLAG_COMP_EN) ? on: off,
							preset_getFlag(PRESET_FLAG_BOOST_EN) ? on : off,
							preset_getFlag(PRESET_FLAG_OCTAVE_EN) ? on : off);
	uint8_t model = preset_getFlag(PRESET_FLAG_AMPMODEL_NO);
	switch(model)
	{
		case 0:
			snprintf(bf, 20, "OFF"); 
			break;
		case 1 ... 4:
			snprintf(bf, 20, "Clean %d", model);
			break;
		case 5 ... 6:
			snprintf(bf, 20, "Crunch %d", model-4);
			break;
		case 7 ... 8:
			snprintf(bf, 20, "Lead %d", model-6);
			break;
		default: break;
	}							
	DBG_SERIAL.print("Amp model: ");
	stats_set_color(STATS_COLOR_YELLOW);
	DBG_SERIAL.print(bf);
	stats_reset_color();
	switch(IRno)
	{
		case IR_GUITAR1 ... IR_GUITAR7:
			snprintf(bf, 20, "Guitar %d", IRno+1);
			break;
		case 7 ... 9:
			snprintf(bf, 20, "Bass %d", IRno-6);
			break;
		case 10:
			snprintf(bf, 20, "OFF");
			break;
		default: break;
	}
	DBG_SERIAL.print("    IR: ");
	stats_set_color(STATS_COLOR_YELLOW);
	DBG_SERIAL.print(bf);
	stats_reset_color();
	DBG_SERIAL.printf("    Doubler: %s%s\r\n", preset_getFlag(PRESET_FLAG_DOUBLER_EN) ? on : off, clrLineCursor2End);

	DBG_SERIAL.printf("Reverb: %s Type: %s Freeze: %s", 
						preset_getFlag(PRESET_FLAG_REVB_EN) ? on : off, 
						name, 
						reverb_getFreeze() ? on : off);
	if (reverbIdx == REVERB_PLATE)
	{
		DBG_SERIAL.printf(" Pitch: %d ShimPitch: %d", reverbPL_pitch, reverbPL_shimmPitch);
		DBG_SERIAL.print(clrLineCursor2End);
		DBG_SERIAL.println();
	}
	else
	{
		DBG_SERIAL.print(clrLineCursor2End);
		DBG_SERIAL.println();
	}
	DBG_SERIAL.printf("Delay: %s Freeze: %s\r\n", preset_getFlag(PRESET_FLAG_DELAY_EN) ? on : off, 
							echo.freeze_get()  ? on : off);	
	DBG_SERIAL.printf("Preset: %d ", preset_getCurrentIdx()+1);	
	DBG_SERIAL.printf("CPU load: %2.2f%%  Mem usage: %d    MIDI channel: %d  \r\n", load, memUsage, midiChannel_get()); 
	// status line, displayed only once
	if (infoLine)
	{
		if (infoLineColor != STATS_COLOR_DEFAULT) stats_set_color(infoLineColor);
		DBG_SERIAL.print(infoLine);
		stats_reset_color();
		if (!infoLineTimeout--)	infoLine = NULL;
	}
	else 
	{
		DBG_SERIAL.print(clrLineAll);
	}
	AudioProcessorUsageMaxReset();
}
// ---------------------------------------------------------------------
void cb_memInfo(EmbeddedCli *cli, char *args, void *context)
{
	constexpr auto RAM_BASE = 0x2020'0000;
	constexpr auto RAM_SIZE = 512 << 10;
	constexpr auto FLASH_BASE = 0x6000'0000;
#if ARDUINO_TEENSY40
	constexpr auto FLASH_SIZE = 2 << 20;
#elif ARDUINO_TEENSY41
	constexpr auto FLASH_SIZE = 8 << 20;
#endif

	// note: these values are defined by the linker, they are not valid memory
	// locations in all cases - by defining them as arrays, the C++ compiler
	// will use the address of these definitions - it's a big hack, but there's
	// really no clean way to get at linker-defined symbols from the .ld file

	extern char _stext[], _etext[], _sbss[], _ebss[], _sdata[], _edata[],
		_estack[], _heap_start[], _heap_end[], _itcm_block_count[], *__brkval;

	auto sp = (char *)__builtin_frame_address(0);

	DBG_SERIAL.printf("_stext        %08x\r\n", _stext);
	DBG_SERIAL.printf("_etext        %08x +%db\r\n", _etext, _etext - _stext);
	DBG_SERIAL.printf("_sdata        %08x\r\n", _sdata);
	DBG_SERIAL.printf("_edata        %08x +%db\r\n", _edata, _edata - _sdata);
	DBG_SERIAL.printf("_sbss         %08x\r\n", _sbss);
	DBG_SERIAL.printf("_ebss         %08x +%db\r\n", _ebss, _ebss - _sbss);
	DBG_SERIAL.printf("curr stack    %08x +%db\r\n", sp, sp - _ebss);
	DBG_SERIAL.printf("_estack       %08x +%db\r\n", _estack, _estack - sp);
	DBG_SERIAL.printf("_heap_start   %08x\r\n", _heap_start);
	DBG_SERIAL.printf("__brkval      %08x +%db\r\n", __brkval, __brkval - _heap_start);
	DBG_SERIAL.printf("_heap_end     %08x +%db\r\n", _heap_end, _heap_end - __brkval);
#if ARDUINO_TEENSY41
	extern char _extram_start[], _extram_end[], *__brkval;
	DBG_SERIAL.printf("_extram_start %08x\r\n", _extram_start);
	DBG_SERIAL.printf("_extram_end   %08x +%db\r\n", _extram_end,
		   _extram_end - _extram_start);
#endif
	DBG_SERIAL.printf("\r\n");

	DBG_SERIAL.printf("<ITCM>  %08x .. %08x\r\n",
		   _stext, _stext + ((int)_itcm_block_count << 15) - 1);
	DBG_SERIAL.printf("<DTCM>  %08x .. %08x\r\n",
		   _sdata, _estack - 1);
	DBG_SERIAL.printf("<RAM>   %08x .. %08x\r\n",
		   RAM_BASE, RAM_BASE + RAM_SIZE - 1);
	DBG_SERIAL.printf("<FLASH> %08x .. %08x\r\n",
		   FLASH_BASE, FLASH_BASE + FLASH_SIZE - 1);
#if ARDUINO_TEENSY41
	extern uint8_t external_psram_size;
	if (external_psram_size > 0)
		DBG_SERIAL.printf("<PSRAM> %08x .. %08x\r\n",
			   _extram_start, _extram_start + (external_psram_size << 20) - 1);
#endif
	DBG_SERIAL.printf("\r\n");

	auto stack = sp - _ebss;
	DBG_SERIAL.printf("avail STACK %8d b %5d kb\r\n", stack, stack >> 10);

	auto heap = _heap_end - __brkval;
	DBG_SERIAL.printf("avail HEAP  %8d b %5d kb\r\n", heap, heap >> 10);

#if ARDUINO_TEENSY41
	auto psram = _extram_start + (external_psram_size << 20) - _extram_end;
	DBG_SERIAL.printf("avail PSRAM %8d b %5d kb\r\n", psram, psram >> 10);
#endif
}

uint32_t *ptrFreeITCM;	 // Set to Usable ITCM free RAM
uint32_t sizeofFreeITCM; // sizeof free RAM in uint32_t units.
uint32_t SizeLeft_etext;

extern char _stext[], _etext[];
FLASHMEM void getFreeITCM()
{ // end of CODE ITCM, skip full 32 bits
	DBG_SERIAL.println("\r\n\r\n++++++++++++++++++++++");
	SizeLeft_etext = (32 * 1024) - (((uint32_t)&_etext - (uint32_t)&_stext) % (32 * 1024));
	sizeofFreeITCM = SizeLeft_etext - 4;
	sizeofFreeITCM /= sizeof(ptrFreeITCM[0]);
	ptrFreeITCM = (uint32_t *)((uint32_t)&_stext + (uint32_t)&_etext + 4);
	DBG_SERIAL.printf("Size of Free ITCM in Bytes = %u\r\n", sizeofFreeITCM * sizeof(ptrFreeITCM[0]));
	DBG_SERIAL.printf("Start of Free ITCM = %u [%X] \r\n", ptrFreeITCM, ptrFreeITCM);
	DBG_SERIAL.printf("End of Free ITCM = %u [%X] \r\n", ptrFreeITCM + sizeofFreeITCM, ptrFreeITCM + sizeofFreeITCM);
	for (uint32_t ii = 0; ii < sizeofFreeITCM; ii++)
		ptrFreeITCM[ii] = 1;
	uint32_t jj = 0;
	for (uint32_t ii = 0; ii < sizeofFreeITCM; ii++)
		jj += ptrFreeITCM[ii];
	DBG_SERIAL.printf("ITCM DWORD cnt = %u [#bytes=%u] \r\n", jj, jj * 4);
}

void cb_componentCPU(EmbeddedCli *cli, char *args, void *context)
{
	stats_printComponents();
}

// Print max CPU load for all components
void stats_printComponents()
{
	DBG_SERIAL.println("Component:\t\tCPU load %:\r\n");
	DBG_SERIAL.printf("Input Switch:\t\t%.2f\r\n", inputSwitch.processorUsageMax());
	DBG_SERIAL.printf("Compressor:\t\t%.2f\r\n", compressor.processorUsageMax());
	DBG_SERIAL.printf("Booster:\t\t%.2f\r\n", booster.processorUsageMax());
	DBG_SERIAL.printf("Amp:\t\t\t%.2f\r\n", amp.processorUsageMax());
	DBG_SERIAL.printf("ToneStack:\t\t%.2f\r\n", toneStack.processorUsageMax());
	DBG_SERIAL.printf("Gate:\t\t\t%.2f\r\n", gate.processorUsageMax());
	DBG_SERIAL.printf("Delay:\t\t\t%.2f\r\n", echo.processorUsageMax());	
	DBG_SERIAL.printf("SpringReverb:\t\t%.2f\r\n", reverbSP.processorUsageMax());
	DBG_SERIAL.printf("PlateReverb:\t\t%.2f\r\n", reverbPL.processorUsageMax());
	DBG_SERIAL.printf("ReverbSC:\t\t%.2f\r\n", reverbSC.processorUsageMax());
	DBG_SERIAL.printf("Dry/FX Xfader:\t\t%.2f\r\n", xfader_main.processorUsageMax());
	DBG_SERIAL.printf("Delay/Reverb Xfader:\t%.2f\r\n", xfader_FX.processorUsageMax());
	DBG_SERIAL.printf("IR Cabsim:\t\t%.2f\r\n", cabsim.processorUsageMax());
	DBG_SERIAL.printf("LowCut:\t%.2f\r\n", masterLowCut.processorUsageMax());
	DBG_SERIAL.printf("Volume:\t\t\t%.2f\r\n", masterVol.processorUsageMax());
}

void cb_setMidiChannel(EmbeddedCli *cli, char *args, void *context)
{
    if (embeddedCliGetTokenCount(args) == 0)
	{
		stats_set_color(STATS_COLOR_RED);
		DBG_SERIAL.println(midiChannel_err_msg);
		stats_reset_color();		
	}
    else
	{
		const char* arg = embeddedCliGetToken(args, 1);
		uint32_t chn = strtoul(arg, NULL, 10);
		if (chn > 0 && chn < 17)
		{
			stats_set_color(STATS_COLOR_GREEN);
			DBG_SERIAL.printf("Setting MIDI channel to: %d\r\n", chn);
			stats_reset_color();
			midiChannel_set(chn);
		}
		else
		{
			stats_set_color(STATS_COLOR_RED);
			DBG_SERIAL.println(midiChannel_err_msg);
			stats_reset_color();			
		}
	}
}