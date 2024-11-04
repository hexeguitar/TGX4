#include <EEPROM.h>
#include <SD.h>
#include "debug_log.h"
#include "audioComponents.h"
#include "presetSystem.h"
#include "presetDefault.h"
#include "reverbs.h"
#include "console.h"

/**
 * Default presets:
 * 1. Basic clean sound
 * 2. Clean + Chorus + Reverb
 * 3. Blues overdrive
 * 4. Crunch Solo
 * 5. Hi Gain, Rhythm, stereo
 * 6. Hi Gain, Solo
 * 7. Bass Basic, Clean1, compressor, B2 IR
 * 8. Bass Drive Stereo, Compressor OFF, 
 */


PresetSystem presetSystem;
extern uint8_t external_psram_size;

/**
 * @brief 	1. detect SD card
 * 		  	2. check for marker in EEPROM (default presets written)
 * 			3. Load midi channe and last used preset number
 * 			4. load the preset
 */
FLASHMEM void PresetSystem::init()
{
	if (!SD.begin(BUILTIN_SDCARD))
	{
		DBG_println("Failed to initialize the SD card!\r\nUsing EEPROM only!"); 
	}
	else 
	{
		DBG_println("SD card initialized."); 
		SDcard_ready = true;
	}
	uint8_t marker;
	marker = EEPROM.read(marker_eepAddr);
	if (marker != eep_marker) // First run, write default presets
	{
		writeDefaults();
	}
	EEPROM.get(midiChannel_eepAddr, midiChannel);
	EEPROM.get(presetLast_eepAddr, currentPresetIdx);
	load(currentPresetIdx);
}


// --------------------------------------------------------------
/**
 * @brief loads new preset. If the SD card is not present, EEPROM is used 
 * 		for the 1st 8 presets. If the requested preset number is > 8, 
 * 		preset 1 is loaded. Otherwise - loads the preset from the SD card.
 */
FLASHMEM void PresetSystem::load(int16_t presetNo)
{
	// if there is a pending preset save, write the previous one before changing to a new one
	if (saveParamsFlag) 
	{
		write(currentPresetIdx, &currentPreset);
		saveParamsFlag = false;
	}
	if (presetNo != currentPresetIdx) 
	{
		saveLastPresetFlag = true;
		t_saveLastPreset = millis();
	}
	if (!SDcard_ready)
	{
		if (presetNo <= (int16_t)PRESET_MAX_NO_EEP)	currentPresetIdx = load_EEP(presetNo);
		else 
		{
			const char msg[] = "NO SD card detected, loading Preset 1";
			DBG_println(msg); 
			console_displayMsg(msg, CONSOLE_COLOR_YELLOW);
			currentPresetIdx = load_EEP(0);
			loadCurrent();
			return;
		}
	}
	else 
	{
		currentPresetIdx = load_SD(presetNo);
	}
	// populate hardware info flags for the external controller
	// these flag is neither used or saved, using the free space in the flags bitfield
	// to pass the HW info to the external controller
	uint8_t hw_info = 0;
#if ARDUINO_TEENSY40
	hw_info = 0;
#elif ARDUINO_TEENSY41
	hw_info |= 0x01;
#endif	
	if (external_psram_size)	hw_info |= 1<<2;
	if (SDcard_ready)			hw_info |= 1<<3;
	currentPreset.flags.hw_info = hw_info & 0x1F;
	loadCurrent();
}
/**
 * @brief Preset up + load, wraps around to 0
 * 
 * @return FLASHMEM 
 */
FLASHMEM void PresetSystem::load_next()
{
	load(currentPresetIdx + 1);
}
/**
 * @brief Preset down + load, wraps around to 127
 * 
 * @return FLASHMEM 
 */
FLASHMEM void PresetSystem::load_previous()
{
	load(currentPresetIdx - 1);
}

// --------------------------------------------------------------
/**
 * @brief load a new preset (range 0-7) from the EEPROM, used if no SD
 * 			card is present, fallback mechanism and backup copy of
 * 			the 1st eight presets.
 * 
 * @param presetNo preset numer to load
 * @return uint8_t - loaded preset number 
 */
FLASHMEM uint8_t PresetSystem::load_EEP(int16_t presetNo)
{
	float32_t tmpf;
	// wrap around the preset number
	if (presetNo >= (int16_t)PRESET_MAX_NO_EEP) presetNo = 0;
	else if (presetNo < 0) 				presetNo = PRESET_MAX_NO_EEP - 1;

	uint16_t eepAddr = preset_eepAddr_base + preset_eepSizeBytes * presetNo;
	// read the data, scale params and populate the current preset
	EEPROM.get(eepAddr, currentPreset.name);
	eepAddr += sizeof(currentPreset.name);
	EEPROM.get(eepAddr, currentPreset.flags);
	uint8_t p = 0;
	eepAddr += sizeof(currentPreset.flags);
	while (p < PRESET_PARAM_LAST)
	{
		tmpf = (float32_t)EEPROM.read(eepAddr++) / 255.0f;
		currentPreset.fxParams[p] = tmpf;
		p++;
	}
	currentPreset.flags.idx = presetNo;
	return presetNo;
}
// --------------------------------------------------------------
/**
 * @brief 	Load a new preset from the SD card. Full 128 presets
 * 			are available. The bin file is a direct data copy 
 * 			of the preset_t structure.
 */
FLASHMEM uint8_t PresetSystem::load_SD(int16_t presetNo)
{
	if (presetNo >= (int16_t)PRESET_MAX_NO) 	presetNo = 0;
	else if (presetNo < 0)			presetNo = PRESET_MAX_NO - 1;
	char bf[16];
	snprintf(bf, 15, "PRESET%03d.bin", presetNo);
	File presetFile = SD.open(bf, FILE_READ);
	if (presetFile)
	{
		presetFile.readBytes((char *)&currentPreset, sizeof(preset_t));
		presetFile.close();
		currentPreset.flags.idx = presetNo;
	}
	else
	{
		const char msg[] = "Can't open preset file";
		DBG_printf("%s: %s", msg, bf);
		console_displayMsg(msg, CONSOLE_COLOR_RED);
		return currentPresetIdx;
	}	
	return presetNo;
}
// --------------------------------------------------------------
/**
 * @brief 	Writes a new preset data either to EEPROM (No SC card),
 * 			SD card and EEPROM (presetsNo 0-7) or SD card only 
 * 			for presetNo above 7
 * 
 * @param presetNo 		preset number (0-127)
 * @param presetPtr 	pointer to the preset struct
 */
FLASHMEM void PresetSystem::write(uint8_t presetNo, preset_t *presetPtr)
{
	if (presetNo < PRESET_MAX_NO_EEP)
	{
		writeToEeprom(presetNo, presetPtr);
	}
	if (presetNo >= PRESET_MAX_NO) return;
	char bf[16];
	snprintf(bf, 15, "PRESET%03d.bin", presetNo);
	
	uint8_t *dataPtr = (uint8_t *)presetPtr;
	uint16_t size = sizeof(preset_t);

	DBG_printf("writing to file %s, size=%d bytes\r\n", bf, size);
	File presetFile = SD.open(bf, FILE_WRITE_BEGIN);
	if (presetFile)
	{
		presetFile.write(dataPtr, size);
		presetFile.close();	
	}
	else
	{
		DBG_printf("Can't open %s file for write", bf);
		return;
	}
	console_displayMsg("Preset saved.", CONSOLE_COLOR_YELLOW);	
}
// --------------------------------------------------------------
/**
 * @brief 	Writes a n new preset data to EEPROM. Valid for 1st 8 
 * 			presets. Also used as a backup copy. To save the space
 * 			float params are scaled to 8bit values.
 * 
 * @param presetNo 		preset number (0-7)
 * @param presetPtr 	pointer to the preset struct
 */
void PresetSystem::writeToEeprom(uint8_t presetNo, preset_t *presetPtr)
{
	if (presetNo >= PRESET_MAX_NO_EEP) return;
	uint32_t eepAddr = preset_eepAddr_base + preset_eepSizeBytes*presetNo;
	uint32_t addrEnd = eepAddr + sizeof(currentPreset.name) + sizeof(preset_flags_t);
	uint8_t *dataPtr = (uint8_t *)presetPtr->name;
	while (eepAddr < addrEnd)
	{
		EEPROM.update(eepAddr, *dataPtr);
		eepAddr++;
		dataPtr++;
	}
	addrEnd = eepAddr + sizeof(currentPreset.fxParams)/sizeof(float32_t);
	uint8_t param_u8;
	while (eepAddr < addrEnd)
	{
		param_u8 = *(float32_t *)dataPtr * 255.0f;
		EEPROM.update(eepAddr, param_u8);
		eepAddr++;
		dataPtr += 4;
	}
	console_displayMsg("Preset saved.", CONSOLE_COLOR_YELLOW);
}
// --------------------------------------------------------------
/**
 * @brief 	Updates a flag in the current preset, used only for the preset related flags
 * 			(no hw info or newPresetNo)
 */
FLASHMEM void PresetSystem::updateFlag(preset_paramFlag_t flag, uint8_t value, bool signal)
{
	if (flag >= PRESET_FLAG_LAST) return;
	if (signal) flagChangeMask |= 1<<((uint8_t)flag);
	switch(flag)
	{
		case PRESET_FLAG_BYPASS:
			currentPreset.flags.bypass = value & 0x01; 			break;
		case PRESET_FLAG_INSELECT:
			currentPreset.flags.inSelect = value & 0x03; 		break;
		case PRESET_FLAG_COMP_EN:
			currentPreset.flags.compressorEn = value & 0x01; 	break;
		case PRESET_FLAG_WAH_EN:
			currentPreset.flags.wahEn = value & 0x01; 			break;
		case PRESET_FLAG_WAH_MODEL:
			currentPreset.flags.wahModel = value & 0x07; 		break;			
		case PRESET_FLAG_BOOST_EN:
			currentPreset.flags.boostEn = value & 0x01;			break;
		case PRESET_FLAG_OCTAVE_EN:
			currentPreset.flags.octaveEn = value & 0x01;		break;
		case PRESET_FLAG_AMPMODEL_NO:
			currentPreset.flags.ampModelNo = value & 0x0F;		break;
		case PRESET_FLAG_DOUBLER_EN:
			currentPreset.flags.doublerEn = value & 0x01;		break;
		case PRESET_FLAG_CABIR_NO:
			currentPreset.flags.cabIrNo = value & 0x0F;			break;
		case PRESET_FLAG_DELAY_EN:
			currentPreset.flags.delayEn = value & 0x01;			break;
		case PRESET_FLAG_REVBTYPE:
			currentPreset.flags.revebType = value & 0x03;		break;
		case PRESET_FLAG_REVB_EN:
			currentPreset.flags.reverbEn = value & 0x01;		break;
		case PRESET_FLAG_REVB_FREEZE_EN:
			currentPreset.flags.reverbFreezeEn = value & 0x01;	break; 
		case PRESET_FLAG_DELAY_FREEZE_EN:
			currentPreset.flags.delayFreezeEn = value & 0x01;	break;
		case PRESET_FLAG_POT1_ASSIGN:
			currentPreset.flags.pot1Assign = value & 0x3F;		break;
		case PRESET_FLAG_POT2_ASSIGN:
			currentPreset.flags.pot2Assign = value & 0x3F;		break;
		case PRESET_FLAG_POT3_ASSIGN:
			currentPreset.flags.pot3Assign = value & 0x3F;		break;
		case PRESET_FLAG_EXP_ASSIGN:
			currentPreset.flags.expAssign = value & 0x3F;		break;
		case PRESET_FLAG_FT1_ASSIGN:
			currentPreset.flags.ft1Assign = value & 0x0F;		break;	
		case PRESET_FLAG_FT2_ASSIGN:
			currentPreset.flags.ft2Assign = value & 0x0F;		break;						
		case PRESET_FLAG_DRYSIG:
			currentPreset.flags.drySigEn = value & 0x01;		break;
		case PRESET_FLAG_IDX:
			currentPreset.flags.idx = value & 0x7F;				break;
		case PRESET_FLAG_LAST:
		default:
			break;	
	}
	if(flag_cb[flag]) (*flag_cb[flag])(getFlag(flag));

}
// --------------------------------------------------------------
/**
 * @brief 	returns the value of the flag 
 */
FLASHMEM uint8_t PresetSystem::getFlag(preset_paramFlag_t flag, preset_flags_t* flagsPrt)
{
	if (flagsPrt == NULL) flagsPrt = &currentPreset.flags;
	uint8_t result = 0;
	switch(flag)
	{
		case PRESET_FLAG_BYPASS:
			result = flagsPrt->bypass; 				break;		
		case PRESET_FLAG_INSELECT:
			result = flagsPrt->inSelect; 			break;
		case PRESET_FLAG_COMP_EN:
			result = flagsPrt->compressorEn;		break;
		case PRESET_FLAG_WAH_EN:
			result = flagsPrt->wahEn;				break;
		case PRESET_FLAG_WAH_MODEL:
			result = flagsPrt->wahModel;			break;				
		case PRESET_FLAG_BOOST_EN:
			result = flagsPrt->boostEn;				break;
		case PRESET_FLAG_OCTAVE_EN:
			result = flagsPrt->octaveEn;			break;
		case PRESET_FLAG_AMPMODEL_NO:
			result = flagsPrt->ampModelNo;			break;
		case PRESET_FLAG_DOUBLER_EN:
			result = flagsPrt->doublerEn;			break;
		case PRESET_FLAG_CABIR_NO:
			result = flagsPrt->cabIrNo;				break;
		case PRESET_FLAG_DELAY_EN:
			result = flagsPrt->delayEn;				break;
		case PRESET_FLAG_REVBTYPE:
			result = flagsPrt->revebType;			break;
		case PRESET_FLAG_REVB_EN:
			result = flagsPrt->reverbEn;			break;
		case PRESET_FLAG_POT1_ASSIGN:
			result = flagsPrt->pot1Assign;			break;
		case PRESET_FLAG_POT2_ASSIGN:
			result = flagsPrt->pot2Assign;			break;
		case PRESET_FLAG_POT3_ASSIGN:
			result = flagsPrt->pot3Assign;			break;
		case PRESET_FLAG_EXP_ASSIGN:
			result = flagsPrt->expAssign;			break;
		case PRESET_FLAG_FT1_ASSIGN:
			result = flagsPrt->ft1Assign;			break;
		case PRESET_FLAG_FT2_ASSIGN:
			result = flagsPrt->ft2Assign;			break;
		case PRESET_FLAG_DRYSIG:
			result = flagsPrt->drySigEn;			break;
		case PRESET_FLAG_IDX:
			result = flagsPrt->idx;					break;			
		case PRESET_FLAG_REVB_FREEZE_EN:
			result = flagsPrt->reverbFreezeEn;		break;
		case PRESET_FLAG_DELAY_FREEZE_EN:
			result = flagsPrt->delayFreezeEn;		break;
		default: break;	
	}
	return result;
}
// --------------------------------------------------------------
/**
 * @brief populate all the parameters with the preset values
 * 			using the callback arrays
 */
FLASHMEM void PresetSystem::loadCurrent()
{
	uint16_t i;
	// store the current preset index in the flags area (used by the GUI|ESP32)
	currentPreset.flags.idx = currentPresetIdx;
	currentPreset.flags.newPresetNo = currentPresetIdx;
	AudioNoInterrupts();
	for (i=0; i<PRESET_FLAG_LAST; i++)
	{
		if(flag_cb[i]) (*flag_cb[i])(getFlag((preset_paramFlag_t)i));
	}
	for (i=0; i<PRESET_PARAM_LAST; i++)
	{
		if(param_cb[i]) (*param_cb[i])(getFXparam((preset_param_t)i)); // trigger the callback if defined		
	}
	AudioInterrupts();
}

// --------------------------------------------------------------
/**
 * @brief main preset system processing function.
 * 		- saving the last used preset in eeprom
 * 		- saving the preset data to SD and/or EEPROM
 */
void PresetSystem::process()
{
	if (saveParamsFlag)
	{
		t_now = millis();
		if (t_now - t_saveParam > save_timeout)
		{
			write(currentPresetIdx, &currentPreset);
			saveParamsFlag = false;
		}
	}
	if (saveLastPresetFlag) // update the last used preset number
	{
		t_now = millis();
		if (t_now - t_saveLastPreset > save_timeout)
		{
			EEPROM.put(presetLast_eepAddr, currentPresetIdx);
			saveLastPresetFlag = false;
		}
	}
	// Timeout for browsing presets
	if (currentPreset.flags.newPresetNo != currentPresetIdx)
	{
		t_now = millis();
		if (t_now - presetPreloadTimestamp > presetPreloadTimeout)
		{
			currentPreset.flags.newPresetNo = currentPresetIdx;
		}	
	}
}
// --------------------------------------------------------------
/**
 * @brief Sets the MIDI channel pedal responds to
 * 
 * @param val range 1-16, 16 = omni
 */
void PresetSystem::setMidiChannel(uint8_t val)
{
	if (val > 0 && val < 17) 
	{
		midiChannel = val;
		EEPROM.update(midiChannel_eepAddr, midiChannel);
	}
}
// --------------------------------------------------------------
/**
 * @brief Populate the preset and settings with default ones. Happens on a 1st boot
 * 		or when preset-rst command is executed. The 1st 8 presets have 
 * 		a backup copy in EEPROM, remaining ones are stored on the SD card.
 * 		All 8 default presets are repeated for the remaining ones.
 * 		The marker byte in EEPROM is used to detect the 1st boot.
 */
void PresetSystem::writeDefaults(void)
{
	uint8_t i = 0;
	DBG_println("Populating EEPROM with default presets...");
	EEPROM.put(marker_eepAddr, eep_marker); 				// write the marker 
	EEPROM.put(midiChannel_eepAddr, MIDI_CHANNEL_DEFAULT);
	EEPROM.put(presetLast_eepAddr, PRESET_NO_DEFAULT); 		// and the default last preset	

	for (i=0; i<PRESET_MAX_NO_EEP; i++)
	{
		writeToEeprom(i, (preset_t *)&presets_default[i]);
	}
	if (SDcard_ready)
	{
		for (i=0; i<PRESET_MAX_NO; i++)
		{
			write(i, (preset_t *)&presets_default[i&0x07]);
		}	
	}
	load(currentPresetIdx);									// reload current preset
}


void PresetSystem::printFlags()
{
	DBG_SERIAL.printf("Bypass: %02x\r\n", currentPreset.flags.bypass);
	DBG_SERIAL.printf("inSelect: %02x\r\n", currentPreset.flags.inSelect);
	DBG_SERIAL.printf("comp: %02x\r\n", currentPreset.flags.compressorEn);
	DBG_SERIAL.printf("wah: %02x\r\n", currentPreset.flags.wahEn);
	DBG_SERIAL.printf("wah model: %02x\r\n", currentPreset.flags.wahModel);
	DBG_SERIAL.printf("boost: %02x\r\n", currentPreset.flags.boostEn);
	DBG_SERIAL.printf("oct: %02x\r\n", currentPreset.flags.octaveEn);
	DBG_SERIAL.printf("amp#: %02x\r\n", currentPreset.flags.ampModelNo);
	DBG_SERIAL.printf("doubler: %02x\r\n", currentPreset.flags.doublerEn);
	DBG_SERIAL.printf("cab#: %02x\r\n", currentPreset.flags.cabIrNo);
	DBG_SERIAL.printf("delay: %02x\r\n", currentPreset.flags.delayEn);
	DBG_SERIAL.printf("delay Freeze: %02x\r\n", currentPreset.flags.delayFreezeEn);
	DBG_SERIAL.printf("reverb: %02x\r\n", currentPreset.flags.reverbEn);
	DBG_SERIAL.printf("rvb type: %02x\r\n", currentPreset.flags.revebType);
	DBG_SERIAL.printf("rvb Freeze: %02x\r\n", currentPreset.flags.reverbFreezeEn);
	DBG_SERIAL.printf("pot1: %02x\r\n", currentPreset.flags.pot1Assign);
	DBG_SERIAL.printf("pot2: %02x\r\n", currentPreset.flags.pot2Assign);
	DBG_SERIAL.printf("pot3: %02x\r\n", currentPreset.flags.pot3Assign);
	DBG_SERIAL.printf("exp : %02x\r\n", currentPreset.flags.expAssign);
	DBG_SERIAL.printf("dry: %02x\r\n", currentPreset.flags.drySigEn);
	DBG_SERIAL.printf("idx: %02x\r\n", currentPreset.flags.idx);
}


void PresetSystem::preLoad_next()
{
	uint8_t presetNo = (currentPreset.flags.newPresetNo + 1) & (SDcard_ready ? (PRESET_MAX_NO-1) : (PRESET_MAX_NO_EEP-1));
	currentPreset.flags.newPresetNo = presetNo;
	DBG_printf("preload %d\r\n", presetNo);
	flagChangeMask |= 1 << (PRESET_FLAG_LAST + 1);
	presetPreloadTimestamp = millis();
}

void PresetSystem::preLoad_previous()
{
	uint8_t presetNo = (currentPreset.flags.newPresetNo - 1) & (SDcard_ready ? (PRESET_MAX_NO-1) : (PRESET_MAX_NO_EEP-1));
	currentPreset.flags.newPresetNo = presetNo;
	DBG_printf("preload %d\r\n", presetNo);
	flagChangeMask |= 1 << (PRESET_FLAG_LAST + 1);
	presetPreloadTimestamp = millis();
}

char* PresetSystem::getName(uint8_t presetNo)
{
	presetNo &= PRESET_MAX_NO - 1;
	if (!SDcard_ready)
	{
		if (presetNo < (int16_t)PRESET_MAX_NO_EEP)
		{
			uint16_t eepAddr = preset_eepAddr_base + preset_eepSizeBytes * presetNo;
			EEPROM.get(eepAddr, tmpBf);
		}
		else 
		{
			memset(tmpBf, 0, sizeof(tmpBf));
		}
	}
	else 
	{
		snprintf((char*)tmpBf, 15, "PRESET%03d.bin", presetNo);
		File presetFile = SD.open((const char*)tmpBf, FILE_READ);
		if (presetFile)
		{
			presetFile.readBytes((char *)tmpBf, member_size(preset_t, name));
			presetFile.close();
		}
		else
		{
			memset(tmpBf, 0, sizeof(tmpBf));
		}	
	}
	return (char* )tmpBf;
}