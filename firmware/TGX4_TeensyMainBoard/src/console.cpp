/**
 * @file console.cpp
 * @author Piotr Zapart (www.hexefx.com)
 * @brief Serial command line interface, status report print
 * @version 0.1
 * @date 2024-07-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <Arduino.h>
#include <SD.h>
#include "console.h"
#include "debug_log.h"
#include "audioComponents.h"
#include "reverbs.h"
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"
#include "presetSystem.h"
#include "hardware_defs/hardware_defs.h"

#define INFOLINE_TIMEOUT 3

const char PROGMEM *termCls = "\x1b[2J";
const char PROGMEM *termCursorHide = "\x1b[?25l";
const char PROGMEM *termCursorShow = "\x1b[?25h";
const char PROGMEM *msg_on = "\x1b[32mon \x1b[0m";
const char PROGMEM *msg_off = "\x1b[31moff\x1b[0m";
const char PROGMEM *msg_ok = "\x1b[32mok \x1b[0m";
const char PROGMEM *msg_fail = "\x1b[31mfail\x1b[0m";
const char PROGMEM *clrLineCursor2End = "\x1b[0K";
const char PROGMEM *clrLineBeg2Cursor = "\x1b[1K";
const char PROGMEM *clrLineAll = "\x1b[2K";
const char PROGMEM *termPosHome = "\x1b[;H";

const char PROGMEM *midiChannel_err_msg = "Please provide a value in range 1 - 16";
const char PROGMEM *presetNo_err_msg = "Please provide a valid preset number 1-128 (SD) or 1-8 (EEPROM)";
#if ARDUINO_TEENSY40
	const char hw_type_msg[]="T4.0";
#elif ARDUINO_TEENSY41
	const char hw_type_msg[]="T4.1";
#else
	const char hw_type_msg[]="???";
#endif


static uint32_t timeNow, timeLast;
static uint8_t infoLineTimeout = INFOLINE_TIMEOUT;

void writeChar(EmbeddedCli *embeddedCli, char c);
EmbeddedCliConfig *config = embeddedCliDefaultConfig();
EmbeddedCli *cli;// = embeddedCliNew(config);

// --- CLI callbacks ---
void cb_presetRST(EmbeddedCli *cli, char *args, void *context);
void cb_memInfo(EmbeddedCli *cli, char *args, void *context);
void cb_componentCPU(EmbeddedCli *cli, char *args, void *context);
void cb_setMidiChannel(EmbeddedCli *cli, char *args, void *context);
void cb_loadPreset(EmbeddedCli *cli, char *args, void *context);
void cb_backupPresets(EmbeddedCli *cli, char *args, void *context);
void cb_presetName(EmbeddedCli *cli, char *args, void *context);
void cb_i2cScan(EmbeddedCli *cli, char *args, void *context);
void cb_SDlist(EmbeddedCli *cli, char *args, void *context);
void i2c_scan(TwoWire *i2cBus);

console_mode_t consoleMode = CONSOLE_MODE_CONSOLE;
const char* infoLine = NULL;
console_color_t infoLineColor = CONSOLE_COLOR_DEFAULT;
void printStatus();
FLASHMEM void getFreeITCM();
inline void console_reset_color();
inline void console_set_color(uint8_t color);
bool console_copyFile(File src, File dst);
void printDirectory(const char* path, int levels);

char bf[32] = "";

// ---------------------------------------------------------------------
FLASHMEM void console_init()
{
	//consoleMode = CONSOLE_MODE_INFO;
	
	config->maxBindingCount = 24;
	cli = embeddedCliNew(config);
	cli->writeChar = writeChar;

	embeddedCliAddBinding(cli, {
			"preset-rst",          			// command name (spaces are not allowed)
			"Restore default presets",   	// Optional help for a command (NULL for no help)
			false,              			// flag whether to tokenize arguments (see below)
			nullptr,            			// optional pointer to any application context
			cb_presetRST               		// binding function 
	});
	embeddedCliAddBinding(cli, {
			"preset-load",
			"load preset 1-128",
			false,
			nullptr,
			cb_loadPreset
	});	
	embeddedCliAddBinding(cli, {
			"preset-backup",
			"load preset 1-128",
			false,
			nullptr,
			cb_backupPresets
	});	
	embeddedCliAddBinding(cli, {
			"preset-name",
			"set preset name",
			false,
			nullptr,
			cb_presetName
	});		
	embeddedCliAddBinding(cli, {
			"meminfo",
			"Print memory info",
			false,
			nullptr,
			cb_memInfo
	});
	embeddedCliAddBinding(cli, {
			"cpu",
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
	embeddedCliAddBinding(cli, {
			"i2c",
			"Scan I2C bus (0, 1, 2)",
			false,
			nullptr,
			cb_i2cScan
	});
	embeddedCliAddBinding(cli, {
			"list",
			"SD - list files",
			false,
			nullptr,
			cb_SDlist
	});
	
}
// ---------------------------------------------------------------------
FLASHMEM void console_setMode(console_mode_t mode)
{
	consoleMode = mode;	
	switch(consoleMode)
	{
		case CONSOLE_MODE_INFO:
			DBG_SERIAL.print(termCursorShow);
			break;
		case CONSOLE_MODE_CONSOLE:
			DBG_SERIAL.print(termCursorHide);
			break;
	}		
	DBG_SERIAL.print(termCls);
	DBG_SERIAL.print(termPosHome);
}
// ---------------------------------------------------------------------
FLASHMEM void console_tglMode()
{
	console_setMode(consoleMode==CONSOLE_MODE_INFO ? CONSOLE_MODE_CONSOLE: CONSOLE_MODE_INFO);
}
// ---------------------------------------------------------------------
FLASHMEM void console_process()
{
	switch(consoleMode)
	{
		case CONSOLE_MODE_INFO:
			timeNow = millis();
			if (timeNow - timeLast > 500)
			{
				DBG_SERIAL.print(termPosHome);
				printStatus();		
				timeLast = timeNow;
			}		
			break;
		case CONSOLE_MODE_CONSOLE:
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
FLASHMEM void console_displayMsg(const char *msg, console_color_t color)
{
	if (consoleMode != CONSOLE_MODE_INFO) return; // show only in info mode
	infoLineColor = color;
	infoLine = msg;
	infoLineTimeout = INFOLINE_TIMEOUT;
}
// ---------------------------------------------------------------------
inline void console_set_color(uint8_t color) 
{
    DBG_SERIAL.print(F("\x1b["));
    DBG_SERIAL.print(30 + color);
    DBG_SERIAL.print("m");
}
// ---------------------------------------------------------------------
inline void console_reset_color() 
{
    DBG_SERIAL.print(F("\x1b[0m"));
}
// ---------------------------------------------------------------------
FLASHMEM void printStatus()
{
	uint8_t reverbIdx = presetSystem.getFlag(PRESET_FLAG_REVBTYPE);
	uint8_t IRno =  presetSystem.getFlag(PRESET_FLAG_CABIR_NO);

	int8_t reverbPL_pitch = reverbPL.pitch_get();
	int8_t  reverbPL_shimmPitch = reverbPL.shimmerPitch_get();
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
	console_set_color(CONSOLE_COLOR_YELLOW);
	DBG_SERIAL.print(bf);
	console_reset_color();
	DBG_SERIAL.printf("    Comp: %s   Wah: %s   Boost: %s   Octave: %s   \r\n", 
							presetSystem.getFlag(PRESET_FLAG_COMP_EN) ? msg_on: msg_off,
							presetSystem.getFlag(PRESET_FLAG_WAH_EN) ? msg_on : msg_off,
							presetSystem.getFlag(PRESET_FLAG_BOOST_EN) ? msg_on : msg_off,
							presetSystem.getFlag(PRESET_FLAG_OCTAVE_EN) ? msg_on : msg_off);
	uint8_t model = presetSystem.getFlag(PRESET_FLAG_AMPMODEL_NO);
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
	console_set_color(CONSOLE_COLOR_YELLOW);
	DBG_SERIAL.print(bf);
	console_reset_color();
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
	console_set_color(CONSOLE_COLOR_YELLOW);
	DBG_SERIAL.print(bf);
	console_reset_color();
	DBG_SERIAL.printf("    Doubler: %s%s\r\n", presetSystem.getFlag(PRESET_FLAG_DOUBLER_EN) ? msg_on : msg_off, clrLineCursor2End);

	DBG_SERIAL.printf("Reverb: %s Type: %s Freeze: %s", 
						presetSystem.getFlag(PRESET_FLAG_REVB_EN) ? msg_on : msg_off, 
						name, 
						reverb_getFreeze() ? msg_on : msg_off);
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
	DBG_SERIAL.printf("Delay: %s Freeze: %s\r\n", presetSystem.getFlag(PRESET_FLAG_DELAY_EN) ? msg_on : msg_off, 
							echo.freeze_get()  ? msg_on : msg_off);	
	DBG_SERIAL.printf("Preset: %d %s%s\r\n", presetSystem.getCurrentIdx()+1, presetSystem.getName(),clrLineCursor2End);	

	DBG_SERIAL.printf("CPU load: %2.2f%%  Mem usage: %d    MIDI channel: %d HW: %s   SD: %s\r\n", 
						load, memUsage, presetSystem.getMidiChannel(), hw_type_msg, presetSystem.getSDstatus() ? msg_ok : msg_fail); 
	// status line, displayed only once
	if (infoLine)
	{
		if (infoLineColor != CONSOLE_COLOR_DEFAULT) console_set_color(infoLineColor);
		DBG_SERIAL.print(infoLine);
		console_reset_color();
		if (!infoLineTimeout--)	infoLine = NULL;
	}
	else 
	{
		DBG_SERIAL.print(clrLineAll);
	}
	AudioProcessorUsageMaxReset();
}
// ---------------------------------------------------------------------
FLASHMEM void cb_memInfo(EmbeddedCli *cli, char *args, void *context)
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
// ---------------------------------------------------------------------
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
// ---------------------------------------------------------------------
FLASHMEM void cb_componentCPU(EmbeddedCli *cli, char *args, void *context)
{
	console_printComponents();
}
// ---------------------------------------------------------------------
// Print max CPU load for all components
FLASHMEM void console_printComponents()
{
	DBG_SERIAL.println("Component:\t\tCPU load %:\r\n");
	DBG_SERIAL.printf("Input Switch:\t\t%.2f\r\n", inputSwitch.processorUsageMax());
	DBG_SERIAL.printf("Compressor:\t\t%.2f\r\n", compressor.processorUsageMax());
	DBG_SERIAL.printf("Wah:\t\t\t%.2f\r\n", wah.processorUsageMax());
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
// ---------------------------------------------------------------------
FLASHMEM void cb_setMidiChannel(EmbeddedCli *cli, char *args, void *context)
{
    if (embeddedCliGetTokenCount(args) == 0)
	{
		console_set_color(CONSOLE_COLOR_RED);
		DBG_SERIAL.println(midiChannel_err_msg);
		console_reset_color();		
	}
    else
	{
		const char* arg = embeddedCliGetToken(args, 1);
		uint32_t chn = strtoul(arg, NULL, 10);
		if (chn > 0 && chn < 17)
		{
			console_set_color(CONSOLE_COLOR_GREEN);
			DBG_SERIAL.printf("Setting MIDI channel to: %d\r\n", chn);
			console_reset_color();
			presetSystem.setMidiChannel(chn);
		}
		else
		{
			console_set_color(CONSOLE_COLOR_RED);
			DBG_SERIAL.println(midiChannel_err_msg);
			console_reset_color();			
		}
	}
}
// ---------------------------------------------------------------------
FLASHMEM void cb_presetRST(EmbeddedCli *cli, char *args, void *context)
{
	presetSystem.writeDefaults();
	presetSystem.init();
	DBG_SERIAL.println("Done!");
}
// ---------------------------------------------------------------------
FLASHMEM void cb_i2cScan(EmbeddedCli *cli, char *args, void *context)
{
	TwoWire *i2cBus;
    if (embeddedCliGetTokenCount(args) == 0)
	{
		console_set_color(CONSOLE_COLOR_GREEN);
		i2cBus = &Wire;
		DBG_SERIAL.println("Scanning bus: Wire");
		console_reset_color();		
	}
    else
	{
		const char* arg = embeddedCliGetToken(args, 1);
		uint32_t wireNo = strtoul(arg, NULL, 10);
		if (wireNo >= 0 && wireNo < 3)
		{
			console_set_color(CONSOLE_COLOR_GREEN);
			switch(wireNo)
			{
				case 0: i2cBus = &Wire; break;
				case 1: i2cBus = &Wire1; break;
				case 2: i2cBus = &Wire2; break;
				default: i2cBus = &Wire; break;
			}
			DBG_SERIAL.printf("Scanning bus: Wire%d\r\n", wireNo);
			console_reset_color();
		}
		else
		{
			console_set_color(CONSOLE_COLOR_RED);
			DBG_SERIAL.println("Error! Wrong I2C bus number!");
			console_reset_color();
			return;			
		}
	}
	i2c_scan(i2cBus);
}

// ---------------------------------------------------------------------
FLASHMEM void i2c_scan(TwoWire *i2cBus)
{
	uint8_t i, err;
	TwoWire *_i2c = i2cBus;
	if (!i2cBus)
	{ 
		console_set_color(CONSOLE_COLOR_RED);
		DBG_SERIAL.println("Error! Please provide valid I2C bus!"); 
		console_reset_color();
		return;	
	}
	_i2c->begin();
	DBG_SERIAL.print("   ");
	for (i = 0; i < 16; i++) { DBG_SERIAL.printf("%3x", i); }
	for (i = 0; i <= 127; i++)
	{
		if (i % 16 == 0)
		{
			DBG_SERIAL.printf("\n%02x:", i & 0xF0);
		}
		_i2c->beginTransmission(i);
		err = _i2c->endTransmission();
		
		if (err==0) 	        
		{ 
			console_set_color(CONSOLE_COLOR_CYAN);
			DBG_SERIAL.printf(" %02x", i);
			console_reset_color(); 
		}
		else 					
		{ 
			DBG_SERIAL.printf(" XX"); 
		}
		delayMicroseconds(200);		   
	}
	DBG_SERIAL.println("");
}
// ---------------------------------------------------------------------
FLASHMEM void cb_SDlist(EmbeddedCli *cli, char *args, void *context)
{
	DBG_SERIAL.printf("Bytes Used: %llu, Bytes Total:%llu\n", SD.usedSize(), SD.totalSize());
	printDirectory("/", 2);
}
void printDirectory(const char* path, int levels)
{
	Serial.printf("Listing directory: %s\n", path);

	File root = SD.open(path);
	if (!root)
	{
		Serial.println("Failed to open directory");
		return;
	}
	if (!root.isDirectory())
	{
		Serial.println("Not a directory");
		return;
	}

	File file = root.openNextFile();
	while (file)
	{
		if (file.isDirectory())
		{
			Serial.print("  DIR : ");
			Serial.println(file.name());
			if (levels)
			{
				printDirectory(file.name(), levels - 1);
			}
		}
		else
		{
			Serial.print("  FILE: ");
			Serial.print(file.name());
			Serial.print("  SIZE: ");
			Serial.println(file.size());
		}
		file = root.openNextFile();
	}
	
}

// ---------------------------------------------------------------------
FLASHMEM void cb_loadPreset(EmbeddedCli *cli, char *args, void *context)
{
	if (embeddedCliGetTokenCount(args) == 0)
	{
		console_set_color(CONSOLE_COLOR_RED);
		DBG_SERIAL.println(presetNo_err_msg);
		console_reset_color();
		return;
	}	
	const char* arg = embeddedCliGetToken(args, 1);
	uint32_t presetNo = strtoul(arg, NULL, 10);
	if (presetNo > 0 && presetNo <= 128)
	{
		presetSystem.load(presetNo-1);
		presetSystem.triggerFullGuiUpdate();

	}
	else DBG_SERIAL.println(presetNo_err_msg);
}
// ---------------------------------------------------------------------
FLASHMEM void cb_backupPresets(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t presetNo = 0;
	const char backupPath[]= "/backup/";
	if (!presetSystem.getSDstatus())
	{
		DBG_SERIAL.println("No SD card present - aborting ...");
		return;
	}
	// create backup dir if it doesn't exist
	File root = SD.open(backupPath);
	if (!root)
	{
		DBG_SERIAL.printf("Creating dir %s\r\n", backupPath);
		SD.mkdir(backupPath);
	}
	
	for (presetNo = 0; presetNo<128; presetNo++)
	{
		snprintf(bf, 15, "PRESET%03d.bin", presetNo);
		File fileSrc = SD.open(bf, FILE_READ);
		snprintf(bf, 15+sizeof(backupPath), "%sPRESET%03d.bin", backupPath, presetNo);
		File fileDst = SD.open(bf, FILE_WRITE_BEGIN);
		if (console_copyFile(fileSrc, fileDst))
		{
			fileSrc.close();
			fileDst.close();	
		}
		else
		{
			DBG_SERIAL.println("Can't copy file");
			if (fileSrc) fileSrc.close();
			if (fileDst) fileDst.close();
			return;
		}
	}
}
// ---------------------------------------------------------------------
void cb_presetName(EmbeddedCli *cli, char *args, void *context)
{
	if (embeddedCliGetTokenCount(args) == 0)
	{
		console_set_color(CONSOLE_COLOR_RED);
		DBG_SERIAL.println(presetNo_err_msg);
		console_reset_color();
		return;
	}
	const char* arg = embeddedCliGetToken(args, 1);
	uint8_t len = strlen(arg);
	if (len > PRESET_NAME_MAXCHARS) 
	{
		len = PRESET_NAME_MAXCHARS;
		console_set_color(CONSOLE_COLOR_YELLOW);
		DBG_SERIAL.println("Warning! Name too long, it will be truncated.");
		console_reset_color();
	}
	DBG_SERIAL.printf("Renaming preset %d from %s to %s\r\n",
					presetSystem.getCurrentIdx()+1,
					presetSystem.getName(),
					arg);
	presetSystem.setName((char *)arg, true);
}

// ---------------------------------------------------------------------
void console_printBin(uint64_t v, uint8_t num_places)
{
    uint64_t mask=0, n;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1ull) | 0x0001ull;
    }
    v = v & mask;  // truncate v to specified number of places

    while(num_places)
    {

        if (v & (0x0001ull << (num_places-1))) DBG_SERIAL.write('1');
        else                                DBG_SERIAL.write('0');
        --num_places;
        if(((num_places%4) == 0) && (num_places != 0))
        {
             DBG_SERIAL.write('_');
        }
    }
    DBG_SERIAL.print("\r\n");
}
// ---------------------------------------------------------------------
bool console_copyFile(File src, File dst)
{
	size_t n;  
	uint8_t buf[64];
	if (!src || !dst) return false;
	while ((n = src.read(buf, sizeof(buf))) > 0) 
	{
		dst.write(buf, n);
	}
	return true;
}