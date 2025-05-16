/**
 * @file main.cpp
 * @author Piotr Zapart
 * @brief Teensy4 guitar amp emulation project
 * 
 * 		required libraries: 
 * 			OpenAudio_ArduinoLibrary: https://github.com/chipaudette/OpenAudio_ArduinoLibrary
 * 			HexeFX_audiolib_F32: https://github.com/hexeguitar/hexefx_audiolib_F32
 * 			
 * 		Neural A Amp modeler based on Seed pedal design by
 * 			Keith Bloemer (GuitarML) https://github.com/GuitarML/Seed
 * 		Wah implementation based on model developed by Transmogrifox
 * 			https://cackleberrypines.net/transmogrifox/src/bela/inductor_wah_C_src/
 * 		
 *  
 * @version 2.0
 * @date 2024-07-26
 * 
 * @copyright Copyright www.hexefx.com (c) 2024
 */


#include <Arduino.h>
#include "debug_log.h"
#include "system.h"
#include "audioComponents.h"
#include "presetSystem.h"
#include "hardware_defs/hardware_defs.h"
#include "console.h"
#include "reverbs.h"
#include "midi.h"	

#if defined(USE_I2S1)
AudioInputI2S_ext_F32			i2s_in;
#elif defined(USE_I2S2)
AudioInputI2S2_F32				i2s_in;
#endif

AudioFilterDCblockerStereo_F32	DCblocker;
AudioSwitchSelectorStereo		inputSwitch;
AudioBasicTempBuffer_F32		InBufferL;
AudioBasicTempBuffer_F32		InBufferR;
AudioEffectCompressorStereo_F32 compressor;
AudioEffectWahMono_F32			wah;
AudioEffectGuitarBooster_F32	booster;
AudioEffectRTNeural_F32			amp;
AudioFilterEqualizer3bandStereo_F32	toneStack;
AudioEffectNoiseGateStereo_F32	gate(InBufferL.dataPtr, InBufferR.dataPtr);
// for Teensy4.0 delay meminit will fail and default to 200ms in DRAM
AudioEffectDelayStereo_F32		echo = AudioEffectDelayStereo_F32(1500, true); // 1 sec delay, buffer in PSRAM;
AudioEffectSpringReverb_F32		reverbSP;
AudioEffectPlateReverb_F32		reverbPL;
// ReverbSC set to use PSRAM will fail to init on Teensy4 and stick in bypass-PASS mode 
AudioEffectReverbSc_F32			reverbSC = AudioEffectReverbSc_F32(true);	
AudioEffectXfaderStereo_F32		xfader_FX;
AudioEffectXfaderStereo_F32		xfader_main;
AudioFilterIRCabsim_F32			cabsim;
AudioFilterBiquadStereo_F32		masterLowCut=AudioFilterBiquadStereo_F32(1); // 1 stage only

AudioEffectGainStereo_F32		masterVol;
#if defined(USE_I2S1)
	AudioOutputI2S_ext_F32    		i2s_out; 
#elif defined(USE_I2S2)
	AudioOutputI2S2_F32     		i2s_out;
#endif
/**
 * -----------------------------------------------------------------------------
 * >>>>> Codec is delcared and defined in the hardware controller class <<<<<<<<
 * -----------------------------------------------------------------------------
 */

// --------------------------------------------------------------
AudioConnection_F32 	cable1(i2s_in, 0, DCblocker, 0);
AudioConnection_F32 	cable2(i2s_in, 1, DCblocker, 1);
AudioConnection_F32 	cable3(DCblocker, 0, inputSwitch, 0);
AudioConnection_F32 	cable4(DCblocker, 1, inputSwitch, 1);
AudioConnection_F32     cable10(inputSwitch, 0, InBufferL, 0); 
AudioConnection_F32     cable11(inputSwitch, 0, compressor, 0);	
AudioConnection_F32     cable12(inputSwitch, 1, InBufferR, 0);
AudioConnection_F32     cable13(inputSwitch, 1, compressor, 1);
AudioConnection_F32     cable14(compressor, 0, wah, 0);
AudioConnection_F32     cable15(compressor, 1, wah, 1);
AudioConnection_F32		cable16(wah, 0, booster, 0);
AudioConnection_F32		cable17(wah, 1, booster, 1);
AudioConnection_F32     cable30(booster, 0, amp, 0);
AudioConnection_F32     cable31(booster, 1, amp, 1);
AudioConnection_F32		cable32(amp, 0, toneStack, 0); 
AudioConnection_F32		cable33(amp, 1, toneStack, 1);
AudioConnection_F32 	cable34(toneStack, 0, gate, 0);
AudioConnection_F32 	cable35(toneStack, 1, gate, 1); 
AudioConnection_F32		cable40(gate, 0, echo, 0);
AudioConnection_F32		cable41(gate, 0, reverbSP, 0);	
AudioConnection_F32		cable42(gate, 0, xfader_main, 0);

AudioConnection_F32		cable50(gate, 1, echo, 1);
AudioConnection_F32		cable51(gate, 1, reverbSP, 1);
AudioConnection_F32		cable52(gate, 1, xfader_main, 1);

AudioConnection_F32		cable60(echo, 0, xfader_FX, 0);
AudioConnection_F32		cable61(echo, 1, xfader_FX, 1);

AudioConnection_F32		cable70(reverbSP, 0, reverbPL, 0);
AudioConnection_F32		cable71(reverbSP, 1, reverbPL, 1);

AudioConnection_F32		cable72(reverbPL, 0, reverbSC, 0);
AudioConnection_F32		cable73(reverbPL, 1, reverbSC, 1);

AudioConnection_F32		cable74(reverbSC, 0, xfader_FX, 2);
AudioConnection_F32		cable75(reverbSC, 1, xfader_FX, 3);

AudioConnection_F32		cable80(xfader_FX, 0, xfader_main, 2);
AudioConnection_F32		cable81(xfader_FX, 1, xfader_main, 3);

AudioConnection_F32     cable82(xfader_main, 0, cabsim, 0);
AudioConnection_F32     cable83(xfader_main, 1, cabsim, 1);

AudioConnection_F32     cable90(cabsim, 0, masterLowCut, 0);
AudioConnection_F32     cable91(cabsim, 1, masterLowCut, 1);

AudioConnection_F32     cable92(masterLowCut, 0, masterVol, 0);
AudioConnection_F32     cable93(masterLowCut, 1, masterVol, 1);

AudioConnection_F32     cable94(masterVol, 0, i2s_out, 0);
AudioConnection_F32     cable95(masterVol, 1, i2s_out, 1);


// --------------------------------------------------------------
void setup()
{
	update_psram_speed(132); // set the PSRAM speed to 132MHz
	DBG_SERIAL.begin(115200);
	AudioMemory_F32(26);
	console_init();

	#if defined(ARDUINO_TEENSY41) || defined (USE_T40_SDIO_ADAPTER)
		if (!SD.begin(BUILTIN_SDCARD))
	#else 
		if (!SD.begin(10)) // standard SPI SD card used on the Teensy Audio Adaptor board
	#endif
		{
			DBG_println("Failed to initialize the SD card!\r\nUsing EEPROM only!"); 
		}
		else 
		{
			DBG_println("SD card initialized."); 
		}
	
	hw.init(); // will init the hardware depending on the configuration
	
	masterVol.phase_inv(hw.phase_invert); // invert output phase if HW requires it
	masterLowCut.setLowShelf(0, 20.0f, -15.0f, 1.0f);
	// software fixes for hardware issues:
	#if defined(USE_HEXEFX_T41GFX)
		i2s_out.set_channel_swap(false);
	#endif
	// set callbacks for USB MIDI
	usbMIDI.setHandleProgramChange(cb_ProgramChange);
	usbMIDI.setHandleNoteOn(cb_NoteOn);
	usbMIDI.setHandleControlChange(cb_ControlChange);
	usbMIDI.setHandleClock(cb_MidiClock);

	presetSystem.setFlagCallback(PRESET_FLAG_BYPASS, [](uint8_t value)
						{ hw.state_set((hw_state_t)!value); presetSystem.resetPreload();});	
	presetSystem.setFlagCallback(PRESET_FLAG_INSELECT, [](uint8_t value)
						{ inputSwitch.setMode((AudioSwitchSelectorStereo::selector_mode_t)value);});
	presetSystem.setFlagCallback(PRESET_FLAG_COMP_EN, [](uint8_t value)
						{ compressor.bypass_set(value ? false : true); });
	presetSystem.setFlagCallback(PRESET_FLAG_WAH_EN, [](uint8_t value)
						{ wah.bypass_set(value ? false : true); });
	presetSystem.setFlagCallback(PRESET_FLAG_WAH_MODEL, [](uint8_t value)
						{ wah.setModel((wahModel_t)value); });
	presetSystem.setFlagCallback(PRESET_FLAG_BOOST_EN, [](uint8_t value)
						{ booster.bypass_set(value ? false : true); });
	presetSystem.setFlagCallback(PRESET_FLAG_OCTAVE_EN, [](uint8_t value)
						{ booster.octave_set(value); });
	presetSystem.setFlagCallback(PRESET_FLAG_AMPMODEL_NO, [](uint8_t value)
						{ amp.changeModel(value);});
	presetSystem.setFlagCallback(PRESET_FLAG_DOUBLER_EN, [](uint8_t value)
						{ cabsim.doubler_set(value); });
	presetSystem.setFlagCallback(PRESET_FLAG_CABIR_NO, [](uint8_t value)
						{ cabsim.ir_load(value); });
	presetSystem.setFlagCallback(PRESET_FLAG_DELAY_EN, [](uint8_t value)
						{ echo.bypass_set(value ? false : true); });
	presetSystem.setFlagCallback(PRESET_FLAG_REVBTYPE, [](uint8_t value)
						{ reverbs_set((active_reverb_e)value, reverb_getBypass()); });
	presetSystem.setFlagCallback(PRESET_FLAG_REVB_EN, [](uint8_t value)
						{ reverb_setBypass(value ? false : true); });
	presetSystem.setFlagCallback(PRESET_FLAG_DELAY_FREEZE_EN, [](uint8_t value)
						{ echo.freeze(value); });
	presetSystem.setFlagCallback(PRESET_FLAG_REVB_FREEZE_EN, [](uint8_t value)
						{ reverb_setFreeze(value); });
	// presetSystem.setFlagCallback(PRESET_FLAG_POT1_ASSIGN, [](uint8_t value)
	// 					{ return; }); //TODO:
	// presetSystem.setFlagCallback(PRESET_FLAG_POT2_ASSIGN, [](uint8_t value)
	// 					{ return; }); //TODO:
	// presetSystem.setFlagCallback(PRESET_FLAG_POT3_ASSIGN, [](uint8_t value)
	// 					{ return; }); //TODO:
	// presetSystem.setFlagCallback(PRESET_FLAG_EXP_ASSIGN, [](uint8_t value)
	// 					{ return; }); //TODO:
	presetSystem.setFlagCallback(PRESET_FLAG_FT1_ASSIGN, [](uint8_t value)
	 					{  hw.led_set(1, presetSystem.getFootswAssignFlagValue(value) ? HW_STATE_ON : HW_STATE_OFF); });
	presetSystem.setFlagCallback(PRESET_FLAG_FT2_ASSIGN, [](uint8_t value)
	 					{  hw.led_set(2, presetSystem.getFootswAssignFlagValue(value) ? HW_STATE_ON : HW_STATE_OFF); });
	presetSystem.setFlagCallback(PRESET_FLAG_DRYSIG, [](uint8_t value)
						{ hw.dry_set((hw_state_t)value); });
	// param change callbacks
	presetSystem.setParamCallback(PRESET_PARAM_COMP_PREGAIN, [](float32_t value)
						{ compressor.setPreGain_normalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_COMP_THRES, [](float32_t value)
						{ compressor.setThresh_normalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_COMP_RATIO, [](float32_t value)
						{ compressor.setCompressionRatio_normalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_COMP_ATTACK, [](float32_t value)
						{ compressor.setAttack_normalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_COMP_RELEASE, [](float32_t value)
						{ compressor.setRelease_normalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_COMP_POSTGAIN, [](float32_t value)
						{ compressor.setPostGain_normalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_WAH_FREQ, [](float32_t value)
						{ wah.setFreq(value); });
	presetSystem.setParamCallback(PRESET_PARAM_WAH_MIX, [](float32_t value)
						{ wah.setMix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_BOOST_DRIVE, [](float32_t value)
						{ booster.drive_normalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_BOOST_BOTTOM, [](float32_t value)
						{ booster.bottom(value); });
	presetSystem.setParamCallback(PRESET_PARAM_BOOST_PRESENCE, [](float32_t value)
						{ booster.tone(value); });
	presetSystem.setParamCallback(PRESET_PARAM_BOOST_MIX, [](float32_t value)
						{ booster.mix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_AMP_GATE, [](float32_t value)
						{ gate.setThreshold_normalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_AMP_GAIN, [](float32_t value)
						{ amp.gain(value); });
	presetSystem.setParamCallback(PRESET_PARAM_AMP_BASS, [](float32_t value)
						{ toneStack.bass(value); });
	presetSystem.setParamCallback(PRESET_PARAM_AMP_MID, [](float32_t value)
						{ toneStack.mid(value); });
	presetSystem.setParamCallback(PRESET_PARAM_AMP_TREBLE, [](float32_t value)
						{ toneStack.treble(value); });
	presetSystem.setParamCallback(PRESET_PARAM_AMP_FXMIX, [](float32_t value)
						{ xfader_main.mix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_TIME, [](float32_t value)
						{ echo.time(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_INERTIA, [](float32_t value)
						{ echo.inertia(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_REPEATS, [](float32_t value)
						{ echo.feedback(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_MIX, [](float32_t value)
						{ echo.mix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_HICUT, [](float32_t value)
						{ echo.treble_cut(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_LOWCUT, [](float32_t value)
						{ echo.bass_cut(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_MIX, [](float32_t value)
						{ echo.mix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_TREBLE, [](float32_t value)
						{ echo.treble(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_BASS, [](float32_t value)
						{ echo.bass(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_MODRATE, [](float32_t value)
						{ echo.mod_rate(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_MODDEPTH, [](float32_t value)
						{ echo.mod_depth(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_SIZE, [](float32_t value)
						{ reverbPL.size(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_DIFF, [](float32_t value)
						{ reverbPL.diffusion(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_MIX, [](float32_t value)
						{ reverbPL.mix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_BASSCUT, [](float32_t value)
						{ reverbPL.lodamp(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_TREBLECUT, [](float32_t value)
						{ reverbPL.hidamp(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_TREBLE, [](float32_t value)
						{ reverbPL.lowpass(1.0f - value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_BASS, [](float32_t value)
						{ reverbPL.hipass(1.0f - value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_BLEEDIN, [](float32_t value)
						{ reverbPL.freezeBleedIn(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_PITCH, [](float32_t value)
						{ reverbPL.pitchNormalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_PITCHMIX, [](float32_t value)
						{ reverbPL.pitchMix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_SHIMMER, [](float32_t value)
						{ reverbPL.shimmer(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBPL_PITCHSHIMM, [](float32_t value)
						{ reverbPL.shimmerPitchNormalized(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBSC_TIME, [](float32_t value)
						{ reverbSC.feedback(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBSC_MIX, [](float32_t value)
						{ reverbSC.mix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBSC_LOWPASSF, [](float32_t value)
						{ reverbSC.lowpass(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBSP_TIME, [](float32_t value)
						{ reverbSP.time(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBSP_MIX, [](float32_t value)
						{ reverbSP.mix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBSP_BASSCUT, [](float32_t value)
						{ reverbSP.bass_cut(value); });
	presetSystem.setParamCallback(PRESET_PARAM_REVERBSP_TREBLECUT, [](float32_t value)
						{ reverbSP.treble_cut(value); });
	presetSystem.setParamCallback(PRESET_PARAM_MASTER_VOLUME, [](float32_t value)
						{ masterVol.setGain(value); });
	presetSystem.setParamCallback(PRESET_PARAM_MASTER_PAN, [](float32_t value)
						{ masterVol.setPan(value); });
	presetSystem.setParamCallback(PRESET_PARAM_DELAY_REVERB_MIX, [](float32_t value)
						{ xfader_FX.mix(value); });
	presetSystem.setParamCallback(PRESET_PARAM_MASTER_LOWCUT, [](float32_t value)
						{ masterLowCut.makeupGain(1.0f + value*0.5f);
						  masterLowCut.setLowShelf(0, MASTER_LOWCUT_FMIN +\
						   value*MASTER_LOWCUT_FMAX, -15.0f, 1.0f); });
	delay(300);				// add a bit of delay before initializine the SD card. Some pedalboard power supplies slowly ramp up the voltage.
	presetSystem.init(); // loads the last used preset - needs the callbacks defined, hence placed here
	
	gui.init();	// init gui after the preset has been loaded
}
// --------------------------------------------------------------
void loop()
{
	 if (CrashReport)
	{
		Serial.print(CrashReport);
		delay(3000);
	}
	usbMIDI.read();
	console_process();
	presetSystem.process();
	hw.process();
	gui.process();
}


