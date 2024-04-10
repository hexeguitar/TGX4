#include <EEPROM.h>
#include "audioComponents.h"
#include "presetSystem.h"
#include "presetDefault.h"
#include "reverbs.h"
#include "stats.h"

/**
 * Default presets:
 * 1. Basic clean sound
 * 		- L in (mono)
 * 		- Compressor OFF, Booster OFF
 * 		- Gate = -65dB
 * 		- Amp: Clean2, all controls max
 * 		- Cabinet IR: G1
 * 		- Delay OFF
 * 		- Reverb: Spring, Mix 0.2, Time 0.65, BassCut 0.46, TrebleCut 0.2
 * 2. Clean + Chorus + Reverb
 * 		- L in, Compressor OFF, Booster OFF
 * 		- Gate -65dB
 * 		- Amp: Clean3, Doubler ON
 * 		- Cabinet IR: G6
 * 		- Delay Time 0.05, Inertia 0, Repeats 0, Mix 0.5, Hi/Lo Cut 0, Treble/Bass 1.0, ModRate 0.25, ModDepth 0.25
 * 		- Reverb: Plate, Size 0.85, Diff 1.0, Mix 0.28, BassCut 0.17, TrebleCut 0.67, Treble/Bass 1.0, no pitch/shimm
 * 3. Blues overdrive
 * 		- L in, Compressor OFF
 * 		- Booster: Drive 0.24, Bottom 1.0, Presence 0.35, Mix 0, Level 1.0
 * 		- Doubler OFF, Amp: Clean 4, Cab: G7
 * 		- Delay OFF
 * 		- Reverb SC, Mix 0.28, Time 0.72, LowpassF 0.41
 * 4. Crunch Solo
 * 		- L in, Compressor OFF
 * 		- Boost: Gain 0.27, Bottom 1.0, Pres. 1.0, Mix 0, Level 1.0
 * 		- Amp Crunch2, Gain 0.5 Cab: G7, rest 1.0
 * 		- Delay: Time 0.54, Repeats 0.15, Mix 0.2, ModRate 0.25, ModDepth 0.25
 * 		- Reverb: Spring, Mix 0.20, Time 0.64, BassCut 0.46, TrebleCut 0.2
 * 5. Hi Gain, Rhythm, stereo
 * 		- L in, Compressor OFF
 * 		- Boost: Drive 0.14, Bottom 0.65, Presence 0.76, Mix 0, Level 1.0
 * 		- Amp: Lead2, Cab: G7, Doubler ON
 * 		- Delay OFF
 * 		- Reverb: Spring, Mix 0.11, Time 0.64, BassCut 0.46, TrebleCut 0.2
 * 6. Hi Gain, Solo
 * 		- L in, Compressor OFF
 * 		- Boost: Drive 0.06, Bottom 0.65, Presence 0.75, Mix 0, Level 1
 * 		- Amp: Lead1, CabIR: G7
 * 		- Delay Time 0.33, Repeats 0.15, Mix 0.24, HiCut 0.34, Lowcut 0.6, Treble 0.53, Bass 1, ModRate 0.06, ModDepth 1
 * 		- Reverb: Spring, Mix 0.11, Time 0.64, BassCut 0.45, TrebleCut 0.2
 * 7. Bass Basic, Clean1, compressor, B2 IR
 * 8. Bass Drive Stereo, Compressor OFF, 
 * 		- Boost: Drive 1, Bottom 0, Presence 0.5, Mix 0.4, Level 1
 * 		- Amp Clean1, Mid 0.5, Cab: B1
 * 		- 
 * 
 */

const uint32_t save_timeout = 2000u;
preset_t currentPreset;
static uint16_t currentPresetIdx = 0;
static uint32_t t_now, t_saveParam, t_saveLastPreset;
static bool saveParamsFlag = false;
static bool saveLastPresetFlag = false;
const uint16_t marker_eepAddr = 0;
const uint8_t eep_marker = 0x5A;
const uint16_t midiChannel_eepAddr = marker_eepAddr + sizeof(eep_marker);
uint8_t midiChannel = 1;
const uint16_t presetLast_eepAddr = midiChannel_eepAddr + sizeof(midiChannel);
const uint16_t preset_eepAddr_base = presetLast_eepAddr + sizeof(presetLast_eepAddr);
const uint16_t preset_eepSizeBytes = sizeof(currentPreset.flags) + sizeof(currentPreset.fxParams)/sizeof(float32_t);

char msgBf[20]; 

void preset_writeToEeprom(uint8_t presetIdx, preset_t *presetPtr);

/**
 * @brief check for the marker at addr 0 and if not found
 * 	populates the eeprom preset area with default values
 * 
 */
FLASHMEM void preset_init()
{
	uint8_t marker;
	marker = EEPROM.read(marker_eepAddr);
	if (marker != eep_marker) // First run, write default presets
	{
		preset_writeDefaults();
	}
	EEPROM.get(midiChannel_eepAddr, midiChannel);
	EEPROM.get(presetLast_eepAddr, currentPresetIdx);
	if (currentPresetIdx > PRESET_MAX_NO) currentPresetIdx = 0;
	delay(100);
	masterLowCut.setLowShelf(0, 20.0f, -15.0f, 1.0f);
	preset_load(currentPresetIdx);

}
// --------------------------------------------------------------
uint8_t preset_getCurrentIdx()
{
	return currentPresetIdx;
}
// --------------------------------------------------------------
FLASHMEM void preset_updateFlag(preset_paramFlag_t flag, uint8_t value)
{
	switch(flag)
	{
		case PRESET_FLAG_INSELECT:
			currentPreset.flags.bits.inSelect = value & 0x03;
			break;
		case PRESET_FLAG_COMP_EN:
			currentPreset.flags.bits.compressorEn = value & 0x01;
			break;
		case PRESET_FLAG_BOOST_EN:
			currentPreset.flags.bits.boostEn = value & 0x01;
			break;
		case PRESET_FLAG_OCTAVE_EN:
			currentPreset.flags.bits.octaveEn = value & 0x01;
			break;
		case PRESET_FLAG_AMPMODEL_NO:
			currentPreset.flags.bits.ampModelNo = value & 0x0F;
			break;
		case PRESET_FLAG_DOUBLER_EN:
			currentPreset.flags.bits.doublerEn = value & 0x01;
			break;
		case PRESET_FLAG_CABIR_NO:
			currentPreset.flags.bits.cabIrNo = value & 0x0F;
			break;
		case PRESET_FLAG_DELAY_EN:
			currentPreset.flags.bits.delayEn = value & 0x01;
			break;
		case PRESET_FLAG_REVBTYPE:
			currentPreset.flags.bits.revebType = value & 0x03;
			break;
		case PRESET_FLAG_REVB_EN:
			currentPreset.flags.bits.reverbEn = value & 0x01;
			break;
		case PRESET_FLAG_REVB_FREEZE_EN:
			currentPreset.flags.bits.reverbFreezeEn = value & 0x01;
			break; 
		case PRESET_FLAG_DELAY_FREEZE_EN:
			currentPreset.flags.bits.delayFreezeEn = value & 0x01;
		default: break;	
	}
}
// --------------------------------------------------------------
FLASHMEM uint8_t preset_getFlag(preset_paramFlag_t flag)
{
	uint8_t result = 0;
	switch(flag)
	{
		case PRESET_FLAG_INSELECT:
			result = currentPreset.flags.bits.inSelect;
			break;
		case PRESET_FLAG_COMP_EN:
			result = currentPreset.flags.bits.compressorEn;
			break;
		case PRESET_FLAG_BOOST_EN:
			result = currentPreset.flags.bits.boostEn;
			break;
		case PRESET_FLAG_OCTAVE_EN:
			result = currentPreset.flags.bits.octaveEn;
			break;
		case PRESET_FLAG_AMPMODEL_NO:
			result = currentPreset.flags.bits.ampModelNo;
			break;
		case PRESET_FLAG_DOUBLER_EN:
			result = currentPreset.flags.bits.doublerEn;
			break;
		case PRESET_FLAG_CABIR_NO:
			result = currentPreset.flags.bits.cabIrNo;
			break;
		case PRESET_FLAG_DELAY_EN:
			result = currentPreset.flags.bits.delayEn;
			break;
		case PRESET_FLAG_REVBTYPE:
			result = currentPreset.flags.bits.revebType;
			break;
		case PRESET_FLAG_REVB_EN:
			result = currentPreset.flags.bits.reverbEn;
			break;
			// freeze is not restored on preset load
			// delay/reverb buffers are cleares, would make no sense
		default: break;	
	}
	return result;
}

uint32_t preset_getFlag() { return currentPreset.flags.raw; } // return all flags as 32bit word

#define DLY_SETUP 100
// --------------------------------------------------------------
FLASHMEM void preset_load(uint32_t presetNo)
{
	// maybe mute the codec output?
	// if there is a pending preset save, write the previous one before  changing to a new one
	if (saveParamsFlag) 
	{
		preset_writeToEeprom(currentPresetIdx, &currentPreset);
		saveParamsFlag = false;
	}
	if (presetNo != currentPresetIdx) 
	{
		saveLastPresetFlag = true;
		t_saveLastPreset = millis();
	}
	currentPresetIdx = presetNo;
	uint16_t eepAddr = preset_eepAddr_base + preset_eepSizeBytes * currentPresetIdx;
	// read the data, scale params and populate the current preset
	EEPROM.get(eepAddr, currentPreset.flags);
	uint8_t p = 0;
	eepAddr += sizeof(currentPreset.flags.raw);
	while (p < PRESET_PARAM_LAST)
	{
		currentPreset.fxParams[p] = (float32_t)EEPROM.read(eepAddr++) / 255.0f;
		p++;
	}
	AudioNoInterrupts();
	inputSwitch.setMode((AudioSwitchSelectorStereo::selector_mode_t) currentPreset.flags.bits.inSelect);
	compressor.bypass_set(!currentPreset.flags.bits.compressorEn);
	booster.bypass_set(!currentPreset.flags.bits.boostEn);
	booster.octave_set(currentPreset.flags.bits.octaveEn);
	amp.changeModel(currentPreset.flags.bits.ampModelNo);
	cabsim.ir_load(currentPreset.flags.bits.cabIrNo);
	cabsim.doubler_set(currentPreset.flags.bits.doublerEn);
	echo.bypass_set(!currentPreset.flags.bits.delayEn);
	echo.bypass_setMode(AudioEffectDelayStereo_F32::BYPASS_MODE_OFF);
	reverbs_set((active_reverb_e) currentPreset.flags.bits.revebType, !currentPreset.flags.bits.reverbEn);
	// --- Compressor ---
	compressor.setPreGain_normalized(preset_getFXparam(PRESET_PARAM_COMP_PREGAIN));
	compressor.setThresh_normalized(preset_getFXparam(PRESET_PARAM_COMP_THRES));
	compressor.setAttack_normalized(preset_getFXparam(PRESET_PARAM_COMP_ATTACK));
	compressor.setRelease_normalized(preset_getFXparam(PRESET_PARAM_COMP_RELEASE));
	compressor.setPostGain_normalized(preset_getFXparam(PRESET_PARAM_COMP_POSTGAIN));
	// --- Booster ---
	booster.drive_normalized(preset_getFXparam(PRESET_PARAM_BOOST_DRIVE));
	booster.bottom(preset_getFXparam(PRESET_PARAM_BOOST_BOTTOM));
	booster.tone(preset_getFXparam(PRESET_PARAM_BOOST_PRESENCE));
	booster.mix(preset_getFXparam(PRESET_PARAM_BOOST_MIX));
	booster.volume(preset_getFXparam(PRESET_PARAM_BOOST_LEVEL));
	// --- Amp ---
	gate.setThreshold_normalized(preset_getFXparam(PRESET_PARAM_AMP_GATE));
	amp.gain(preset_getFXparam(PRESET_PARAM_AMP_GAIN));
	toneStack.bass(preset_getFXparam(PRESET_PARAM_AMP_BASS));
	toneStack.mid(preset_getFXparam(PRESET_PARAM_AMP_MID));
	toneStack.treble(preset_getFXparam(PRESET_PARAM_AMP_TREBLE));
	xfader_main.mix(preset_getFXparam(PRESET_PARAM_AMP_FXMIX));	
	// --- Delay ---
	echo.time(preset_getFXparam(PRESET_PARAM_DELAY_TIME), true); // force immediate update
	echo.feedback(preset_getFXparam(PRESET_PARAM_DELAY_REPEATS));
	echo.mix(preset_getFXparam(PRESET_PARAM_DELAY_MIX));
	echo.treble_cut(preset_getFXparam(PRESET_PARAM_DELAY_HICUT));
	echo.bass_cut(preset_getFXparam(PRESET_PARAM_DELAY_LOWCUT));
	echo.treble(preset_getFXparam(PRESET_PARAM_DELAY_TREBLE));
	echo.bass(preset_getFXparam(PRESET_PARAM_DELAY_BASS));
	echo.mod_rate(preset_getFXparam(PRESET_PARAM_DELAY_MODRATE));
	echo.mod_depth(preset_getFXparam(PRESET_PARAM_DELAY_MODDEPTH));
	echo.freeze(false);
	// --- Reverb
	reverbPL.size(preset_getFXparam(PRESET_PARAM_REVERBPL_SIZE));
	reverbPL.diffusion(preset_getFXparam(PRESET_PARAM_REVERBPL_DIFF));
	reverbPL.mix(preset_getFXparam(PRESET_PARAM_REVERBPL_MIX));
	reverbPL.lodamp(preset_getFXparam(PRESET_PARAM_REVERBPL_BASSCUT));
	reverbPL.hidamp(preset_getFXparam(PRESET_PARAM_REVERBPL_TREBLECUT));
	reverbPL.lowpass(1.0f - preset_getFXparam(PRESET_PARAM_REVERBPL_TREBLE));
	reverbPL.hipass(1.0f - preset_getFXparam(PRESET_PARAM_REVERBPL_BASS));
	reverbPL.freezeBleedIn(preset_getFXparam(PRESET_PARAM_REVERBPL_BLEEDIN));
	reverbPL.pitchSemitones((int8_t)preset_getFXparam(PRESET_PARAM_REVERBPL_PITCH));
	reverbPL.pitchMix(preset_getFXparam(PRESET_PARAM_REVERBPL_PITCHMIX));
	reverbPL.shimmer(preset_getFXparam(PRESET_PARAM_REVERBPL_SHIMMER));
	reverbPL.shimmerPitchSemitones((int8_t)preset_getFXparam(PRESET_PARAM_REVERBPL_PITCHSHIMM));
	reverbPL.freeze(false);
	reverbSC.feedback(preset_getFXparam(PRESET_PARAM_REVERBSC_TIME));
	reverbSC.mix(preset_getFXparam(PRESET_PARAM_REVERBSC_MIX));
	reverbSC.lowpass(1.0f-preset_getFXparam(PRESET_PARAM_REVERBSC_LOWPASSF));
	reverbSC.freeze(false);
	reverbSP.time(preset_getFXparam(PRESET_PARAM_REVERBSP_TIME));
	reverbSP.mix(preset_getFXparam(PRESET_PARAM_REVERBSP_MIX));
	reverbSP.bass_cut(preset_getFXparam(PRESET_PARAM_REVERBSP_BASSCUT));
	reverbSP.treble_cut(preset_getFXparam(PRESET_PARAM_REVERBSP_TREBLECUT));
	xfader_FX.mix((preset_getFXparam(PRESET_PARAM_DELAY_REVERB_MIX)));
	masterLowCut.makeupGain(1.0f + preset_getFXparam(PRESET_PARAM_MASTER_LOWCUT)*0.5f);
	masterLowCut.setLowShelf(0, MASTER_LOWCUT_FMIN +\
		 preset_getFXparam(PRESET_PARAM_MASTER_LOWCUT)*MASTER_LOWCUT_FMAX, -15.0f, 1.0f);
	masterVol.setGain(preset_getFXparam(PRESET_PARAM_MASTER_VOLUME));
	masterVol.setPan(preset_getFXparam(PRESET_PARAM_MASTER_PAN));

	AudioInterrupts();
}
// --------------------------------------------------------------
void preset_save()
{
	saveParamsFlag = true; // trigger delayed eeprom save
	t_saveParam = millis();
}
// --------------------------------------------------------------
void preset_writeToEeprom(uint8_t presetIdx, preset_t *presetPtr)
{
	if (presetIdx >= PRESET_MAX_NO) return;
	uint32_t eepAddr = preset_eepAddr_base + preset_eepSizeBytes*presetIdx;
	uint32_t addrEnd = eepAddr + sizeof(currentPreset.flags);
	uint8_t *dataPtr = (uint8_t *)&presetPtr->flags.raw;
	// write flags
	while (eepAddr < addrEnd)
	{
		EEPROM.update(eepAddr, *dataPtr);
		eepAddr++;
		dataPtr++;
	}
	// eepAddr is at start of params  write them as uint8_t
	addrEnd = eepAddr + sizeof(currentPreset.fxParams)/sizeof(float32_t);
	uint8_t param_u8;
	while (eepAddr < addrEnd)
	{
		param_u8 = *(float32_t *)dataPtr * 255.0f;
		EEPROM.update(eepAddr, param_u8);
		eepAddr++;
		dataPtr += 4;
	}
	stats_displayMsg("Preset saved.", STATS_COLOR_YELLOW);
}
// --------------------------------------------------------------
void presetSystem_process()
{
	if (saveParamsFlag)
	{
		t_now = millis();
		if (t_now - t_saveParam > save_timeout)
		{
			preset_writeToEeprom(currentPresetIdx, &currentPreset);
			saveParamsFlag = false;
		}
	}
	if (saveLastPresetFlag)
	{
		t_now = millis();
		if (t_now - t_saveLastPreset > save_timeout)
		{
			EEPROM.put(presetLast_eepAddr, currentPresetIdx);
			saveLastPresetFlag = false;
		}
	}
}
// --------------------------------------------------------------
uint8_t midiChannel_get()
{
	return midiChannel;
}
// --------------------------------------------------------------
void midiChannel_set(uint8_t val)
{
	if (val > 0 && val < 17) 
	{
		midiChannel = val;
		EEPROM.update(midiChannel_eepAddr, midiChannel);
	}
}
// --------------------------------------------------------------
void preset_writeDefaults(void)
{
	uint8_t i = 0;
	DBG_SERIAL.println("Populating EEPROM with default presets...");
	EEPROM.put(marker_eepAddr, eep_marker); 				// write the marker 
	EEPROM.put(midiChannel_eepAddr, MIDI_CHANNEL_DEFAULT);
	EEPROM.put(presetLast_eepAddr, PRESET_NO_DEFAULT); 		// and the default last preset	
	for (i=0; i<PRESET_MAX_NO; i++)
	{
		preset_writeToEeprom(i, (preset_t *)&presets_default[i]);
	}
	preset_load(currentPresetIdx);							// reload current preset
}
// --------------------------------------------------------------