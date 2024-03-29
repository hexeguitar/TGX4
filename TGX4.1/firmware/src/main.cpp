/**
 * @file main.cpp
 * @author Piotr Zapart
 * @brief Teensy4.1 guitar amp emulation project
 * 		--------------------------------------------------------
 * 		----- REQUIRES TEENSY4.1 with PSRAM installed ! --------		
 * 		--------------------------------------------------------
 * 		required libraries: 
 * 			OpenAudio_ArduinoLibrary: https://github.com/chipaudette/OpenAudio_ArduinoLibrary
 * 			HexeFX_audiolib_F32: https://github.com/hexeguitar/hexefx_audiolib_F32
 * 			
 * 		Neural A Amp modeler based on Seed pedal design by
 * 			Keith Bloemer (GuitarML) https://github.com/GuitarML/Seed
 *  
 * @version 1.3
 * @date 2024-03-20
 * 
 * @copyright Copyright www.hexefx.com (c) 2024
 * ----------------------------------------------------------------------------
 * 	There are two hardware options:
 * 	1. My custom designed pedal using I2S2 and WM8731 codec (currently obsolete!)
 *  2. Generinc Teensy4.1 + Audio Adapter board. To enable this configuration 
 * 		uncomment the -DUSE_TEENSY_AUDIO_BOARD in the platformio.ini file	
 * 		This optin is enabled by default
 *  3. TGA Pro board by Black Addr Audio - using WM8731 codec on the default I2S bus
 * 
 * 	To build the project for one of the hardware options open the terminal in Platformio
 * 	and run:
 * 	1. For Teensy4.1 + Original Audio Adaptor board (SGTL5000, I2S1)
 * 			pio run -e teensy_audio_board
 * 	2. For HexeFX custom pedal using Teensy4.1 and WM8731 codec on I2S2
 * 			pio run -e hexefx_t41gfx
 *  3. For Blackaddr TGA Pro board
 * 			pio tun -e blackaddr_tgapro
 * 
 *  Alternatively, set the default build option on the top of the platformio.ini file:
 * 
 * 		[platformio]
 * 		default_envs = teensy_audio_board  ;set the build environment here
 * 
 */


#include <Arduino.h>
#include "audioComponents.h"
#include "stats.h"
#include "reverbs.h"
#include "midi_mapping.h"	
#include "presetSystem.h"


// analog bypass controls
#define DRY_CTRL_PIN    28
#define WET_CTRL_PIN    29
#define CTRL_HI     	LOW
#define CTRL_LO     	HIGH

// Teensy Audio Adaptor and TGAPro use I2S
#if defined(USE_TEENSY_AUDIO_BOARD) || defined(USE_BLACKADDR_TGAPRO)
	AudioInputI2S_F32				i2s_in;
// T41GFX uses I2S2
#elif defined(USE_HEXEFX_T41GFX)
	AudioInputI2S2_F32				i2s_in;
#endif
	AudioSwitchSelectorStereo		inputSwitch;
	AudioBasicTempBuffer_F32		InBufferL;
	AudioBasicTempBuffer_F32		InBufferR;
	AudioEffectCompressorStereo_F32 compressor;
	AudioEffectGuitarBooster_F32	booster;
	AudioEffectRTNeural_F32			amp;
	AudioFilterEqualizer3bandStereo_F32	toneStack;
	AudioEffectNoiseGateStereo_F32	gate(InBufferL.dataPtr, InBufferR.dataPtr);
	AudioEffectDelayStereo_F32		echo = AudioEffectDelayStereo_F32(1000, true); // 1 sec delay, buffer in PSRAM;
	AudioEffectSpringReverb_F32		reverbSP;
	AudioEffectPlateReverb_F32		reverbPL;
	AudioEffectReverbSc_F32			reverbSC = AudioEffectReverbSc_F32(true);
	AudioEffectXfaderStereo_F32		xfader_FX;
	AudioEffectXfaderStereo_F32		xfader_main;
	AudioFilterBiquadStereo_F32		masterLowCut=AudioFilterBiquadStereo_F32(1); // 1 stage only
	AudioFilterIRCabsim_F32			cabsim;
	AudioEffectGainStereo_F32		masterVol;
#if defined(USE_TEENSY_AUDIO_BOARD)
	AudioOutputI2S_F32     			i2s_out;
	AudioControlSGTL5000_F32		codec;
#elif defined(USE_BLACKADDR_TGAPRO)
	AudioOutputI2S_F32     			i2s_out;
	AudioControlWM8731_F32			codec;
#elif defined(USE_HEXEFX_T41GFX)
	AudioOutputI2S2_F32     		i2s_out;
	AudioControlWM8731_F32			codec;
#endif

AudioConnection_F32 	cable1(i2s_in, 0, inputSwitch, 0);
AudioConnection_F32 	cable2(i2s_in, 1, inputSwitch, 1);
AudioConnection_F32     cable10(inputSwitch, 0, InBufferL, 0); 
AudioConnection_F32     cable11(inputSwitch, 0, compressor, 0);	
AudioConnection_F32     cable12(inputSwitch, 1, InBufferR, 0);
AudioConnection_F32     cable13(inputSwitch, 1, compressor, 1);
AudioConnection_F32     cable14(compressor, 0, booster, 0);
AudioConnection_F32     cable15(compressor, 1, booster, 1);
AudioConnection_F32     cable16(booster, 0, amp, 0);
AudioConnection_F32     cable17(booster, 1, amp, 1);
AudioConnection_F32		cable18(amp, 0, toneStack, 0); 
AudioConnection_F32		cable19(amp, 1, toneStack, 1);
AudioConnection_F32 	cable20(toneStack, 0, gate, 0);
AudioConnection_F32 	cable21(toneStack, 1, gate, 1); 

AudioConnection_F32		cable22(gate, 0, echo, 0);
AudioConnection_F32		cable23(gate, 0, reverbSP, 0);	
AudioConnection_F32		cable24(gate, 0, xfader_main, 0);

AudioConnection_F32		cable25(gate, 1, echo, 1);
AudioConnection_F32		cable26(gate, 1, reverbSP, 1);
AudioConnection_F32		cable27(gate, 1, xfader_main, 1);

AudioConnection_F32		cable28(echo, 0, xfader_FX, 0);
AudioConnection_F32		cable29(echo, 1, xfader_FX, 1);

AudioConnection_F32		cable30(reverbSP, 0, reverbPL, 0);	// spring reverb -> plate reverb
AudioConnection_F32		cable31(reverbSP, 1, reverbPL, 1);
AudioConnection_F32		cable32(reverbPL, 0, reverbSC, 0);	// plate reverb -> reverbSC
AudioConnection_F32		cable33(reverbPL, 1, reverbSC, 1);
AudioConnection_F32		cable34(reverbSC, 0, xfader_FX, 2);
AudioConnection_F32		cable35(reverbSC, 1, xfader_FX, 3);

AudioConnection_F32		cable36(xfader_FX, 0, xfader_main, 2);
AudioConnection_F32		cable37(xfader_FX, 1, xfader_main, 3);
AudioConnection_F32     cable40(xfader_main, 0, cabsim, 0);
AudioConnection_F32     cable41(xfader_main, 1, cabsim, 1);

AudioConnection_F32     cable50(cabsim, 0, masterLowCut, 0);
AudioConnection_F32     cable51(cabsim, 1, masterLowCut, 1);

AudioConnection_F32     cable60(masterLowCut, 0, masterVol, 0);
AudioConnection_F32     cable61(masterLowCut, 1, masterVol, 1);


#ifdef USE_TEENSY_AUDIO_BOARD
AudioConnection_F32     cable70(masterVol, 0, i2s_out, 0);
AudioConnection_F32     cable71(masterVol, 1, i2s_out, 1);
#else
AudioConnection_F32     cable70(masterVol, 0, i2s_out, 1); // PCB error fix (channel swapped)
AudioConnection_F32     cable71(masterVol, 1, i2s_out, 0);
#endif
// Callbacks for MIDI
void cb_NoteOn(byte channel, byte note, byte velocity);
void cb_ControlChange(byte channel, byte control, byte value);
void cb_MidiClock(void);

// --------------------------------------------------------------
void setup()
{
	DBG_SERIAL.begin(115200);
	AudioMemory_F32(26);
	stats_init();

#ifdef USE_TEENSY_AUDIO_BOARD
	if (!codec.enable()) DBG_SERIAL.println("Codec init error!");
	codec.set_bitDepth(AudioControlSGTL5000_F32::I2S_BITS_32);
	codec.inputSelect(AUDIO_INPUT_LINEIN);
	codec.volume(0.8f);
	codec.lineInLevel(10, 10);
	codec.adcHighPassFilterDisable();
#else
	#ifdef USE_HEXEFX_T41GFX
		// analog IO setup - depends on used hardware
		pinMode(DRY_CTRL_PIN, OUTPUT);
		pinMode(WET_CTRL_PIN, OUTPUT);
		digitalWriteFast(DRY_CTRL_PIN, CTRL_LO); 	// mute analog dry passthrough
		digitalWriteFast(WET_CTRL_PIN, CTRL_HI);	// turn on wet signal
	#endif
    if (!codec.enable(AudioControlWM8731_F32::I2S_BITS_32)) DBG_SERIAL.println("Codec init error!");
    codec.inputSelect(AudioControlWM8731_F32::INPUT_SELECT_LINEIN);
    codec.inputLevel(0.77f);
#endif
	// set callbacks for USB MIDI
    usbMIDI.setHandleNoteOn(cb_NoteOn);
    usbMIDI.setHandleControlChange(cb_ControlChange);
	usbMIDI.setHandleClock(cb_MidiClock);

	preset_init();
}
// --------------------------------------------------------------
void loop()
{
	usbMIDI.read();
	stats_process();
	presetSystem_process();
}
// --------------------------------------------------------------
/**
 * @brief USB MIDI NoteOn callback
 * 
 * @param channel 
 * @param note 
 * @param velocity 
 */
void cb_NoteOn(byte channel, byte note, byte velocity)
{
	uint8_t tmp8;
	if (channel != midiChannel_get() && channel != 16) return;
    switch(note)
    {
		// --- Amp + IR ---
		case MIDI_NOTEON_AMP_OFF ... MIDI_NOTEON_AMP_LEAD2:
			tmp8 = note - MIDI_NOTEON_AMP_OFF;
			amp.changeModel(tmp8);
			preset_updateFlag(PRESET_FLAG_AMPMODEL_NO, tmp8);
			break;
		case MIDI_NOTEON_IR_G1 ... MIDI_NOTEON_IR_OFF:
			tmp8 = note - MIDI_NOTEON_IR_G1;
			cabsim.ir_load(tmp8);
			preset_updateFlag(PRESET_FLAG_CABIR_NO, tmp8);
			break;
		case MIDI_NOTEON_IR_DOUBLER_TGL:
			tmp8 = cabsim.doubler_tgl();
			preset_updateFlag(PRESET_FLAG_DOUBLER_EN, tmp8);
			break;
		case MIDI_NOTEON_IR_DOUBLER_OFF:
			cabsim.doubler_set(false);
			preset_updateFlag(PRESET_FLAG_DOUBLER_EN, false);
			break;
		case MIDI_NOTEON_IR_DOUBLER_ON:
			cabsim.doubler_set(true);
			preset_updateFlag(PRESET_FLAG_DOUBLER_EN, true);
			break;
		// --- Compressor ---
		case MIDI_NOTEON_COMP_TGL:
			tmp8 = !compressor.bypass_tgl();
			preset_updateFlag(PRESET_FLAG_COMP_EN, tmp8);
			break;
		case MIDI_NOTEON_COMP_ON:
			compressor.bypass_set(false);
			preset_updateFlag(PRESET_FLAG_COMP_EN, true);
			break;
		case MIDI_NOTEON_COMP_OFF:
			compressor.bypass_set(true);
			preset_updateFlag(PRESET_FLAG_COMP_EN, false);
			break;
		// --- Booster ---
		case MIDI_NOTEON_BOOST_ON:
			booster.bypass_set(false);
			preset_updateFlag(PRESET_FLAG_BOOST_EN, true);
			break;
		case MIDI_NOTEON_BOOST_OFF:
			booster.bypass_set(true);
			preset_updateFlag(PRESET_FLAG_BOOST_EN, false);
			break;
		case MIDI_NOTEON_BOOST_TGL:
			tmp8 = !booster.bypass_tgl();
			preset_updateFlag(PRESET_FLAG_BOOST_EN, tmp8);
			break;
		case MIDI_NOTEON_OCTAVE_ON:
			booster.octave_set(true);
			preset_updateFlag(PRESET_FLAG_OCTAVE_EN, true);
			break;
		case MIDI_NOTEON_OCTAVE_OFF:		
			booster.octave_set(false);
			preset_updateFlag(PRESET_FLAG_OCTAVE_EN, false);
			break;
		case MIDI_NOTEON_OCTAVE_TGL:
			tmp8 = booster.octave_tgl();
			preset_updateFlag(PRESET_FLAG_OCTAVE_EN, tmp8);
			break;
		// --- Reverb ---
		case MIDI_NOTEON_REVERB_TGL:
			tmp8 = !reverb_tglBypass();
			preset_updateFlag(PRESET_FLAG_REVB_EN, tmp8);
			break;			
		case MIDI_NOTEON_REVERB_OFF:
			reverb_setBypass(true); // reverb OFF
			preset_updateFlag(PRESET_FLAG_REVB_EN, false);	
			break;
		case MIDI_NOTEON_REVERB_ON:
			reverb_setBypass(false); // reverb OFF
			preset_updateFlag(PRESET_FLAG_REVB_EN, true);	
			break;
		case MIDI_NOTEON_REVERBPL_SET:
			reverbs_set(REVERB_PLATE, reverb_getBypass());
			preset_updateFlag(PRESET_FLAG_REVBTYPE, (uint8_t)REVERB_PLATE);
			break;
		case MIDI_NOTEON_REVERBSC_SET:
			reverbs_set(REVERB_SC, reverb_getBypass());
			preset_updateFlag(PRESET_FLAG_REVBTYPE, (uint8_t)REVERB_SC);
			break;
		case MIDI_NOTEON_REVERBSP_SET:
			reverbs_set(REVERB_SPRING, reverb_getBypass());
			preset_updateFlag(PRESET_FLAG_REVBTYPE, (uint8_t)REVERB_SPRING);
			break;
		case MIDI_NOTEON_REVERB_FREEZE_TGL:
			tmp8 = reverb_tglFreeze();
			preset_updateFlag(PRESET_FLAG_REVB_FREEZE_EN, tmp8);
			break;
		case MIDI_NOTEON_REVERB_FREEZE_ON:
			reverb_setFreeze(true);
			preset_updateFlag(PRESET_FLAG_REVB_FREEZE_EN, true);
			break;
		case MIDI_NOTEON_REVERB_FREEZE_OFF:
			reverb_setFreeze(false);
			preset_updateFlag(PRESET_FLAG_REVB_FREEZE_EN, false);
			break;
		// --- Delay ---
		case MIDI_NOTEON_DELAY_TGL:
			tmp8 = !echo.bypass_tgl();
			preset_updateFlag(PRESET_FLAG_DELAY_EN, tmp8);
			break;
		case MIDI_NOTEON_DELAY_OFF: 
			echo.bypass_set(true);
			preset_updateFlag(PRESET_FLAG_DELAY_EN, false);
			break;
		case MIDI_NOTEON_DELAY_ON:
			echo.bypass_set(false);
			preset_updateFlag(PRESET_FLAG_DELAY_EN, true);
			break;
		case MIDI_NOTEON_DELAY_TAP:
			echo.tap_tempo(true); // with averaging
			break;
		case MIDI_NOTEON_DELAY_FREEZE_TGL:
			tmp8 = echo.freeze_tgl();
			preset_updateFlag(PRESET_FLAG_DELAY_FREEZE_EN, tmp8);
			break;
		case MIDI_NOTEON_DELAY_FREEZE_ON:
			echo.freeze(true);
			preset_updateFlag(PRESET_FLAG_DELAY_FREEZE_EN, true);
			break;
		case MIDI_NOTEON_DELAY_FREEZE_OFF:
			echo.freeze(false);
			preset_updateFlag(PRESET_FLAG_DELAY_FREEZE_EN, false);
			break;
		// --- Preset System ---
		case MIDI_NOTEON_PRESET_1 ... MIDI_NOTEON_PRESET_8:
			tmp8 = note - MIDI_NOTEON_PRESET_1;
			preset_load(tmp8);
			break;
		case MIDI_NOTEON_PRESET_SAVE:
			preset_save();
			break;
		// --- Input Selector ---
		case MIDI_NOTEON_SELIN_LR:
			inputSwitch.setMode(AudioSwitchSelectorStereo::SIGNAL_SELECT_LR);
			preset_updateFlag(PRESET_FLAG_INSELECT, (uint8_t)AudioSwitchSelectorStereo::SIGNAL_SELECT_LR);
			break;
		case MIDI_NOTEON_SELIN_L:
			inputSwitch.setMode(AudioSwitchSelectorStereo::SIGNAL_SELECT_L);
			preset_updateFlag(PRESET_FLAG_INSELECT, (uint8_t)AudioSwitchSelectorStereo::SIGNAL_SELECT_L);
			break;
		case MIDI_NOTEON_SELIN_R:
			inputSwitch.setMode(AudioSwitchSelectorStereo::SIGNAL_SELECT_R);
			preset_updateFlag(PRESET_FLAG_INSELECT, (uint8_t)AudioSwitchSelectorStereo::SIGNAL_SELECT_R);
			break;
		// --- Diverse ---
		case MIDI_NOTEON_TERMINAL_TGL:
			stats_tglMode();
			break;
		// --- HW control ---
		case MIDI_NOTEON_CTRL_DRY:
			#if defined(USE_HEXEFX_T41GFX)
				digitalToggleFast(DRY_CTRL_PIN);
			#endif
			break;
		case MIDI_NOTEON_CTRL_WET:
			#if defined(USE_HEXEFX_T41GFX)
				digitalToggleFast(WET_CTRL_PIN);
			#endif
			break;
		case MIDI_NOTEON_MCU_RESET:
			SCB_AIRCR = 0x05FA0004; // MCU reset
			break;
		
        default:
            break;
    }
}
// --------------------------------------------------------------
void cb_ControlChange(byte channel, byte control, byte value)
{
	if (channel != midiChannel_get() && channel != 16) return;
    float32_t tmp = (float32_t) value / 127.0f;
	int8_t tmp8;
    switch(control)
    {
		// --- Delay --- 
		case MIDI_CC_DELAY_TIME:
            echo.time(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_TIME, tmp);
			break;
        case MIDI_CC_DELAY_INERTIA:
			echo.inertia(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_INERTIA, tmp);
            break;
        case MIDI_CC_DELAY_FEEDBACK: 
			echo.feedback(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_REPEATS, tmp);
            break;
        case MIDI_CC_DELAY_MIX:
			echo.mix(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_MIX, tmp);
            break;
        case MIDI_CC_DELAY_MOD_RATE:
			echo.mod_rate(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_MODRATE, tmp);
            break;
        case MIDI_CC_DELAY_MOD_DEPTH:
			echo.mod_depth(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_MODDEPTH, tmp);
            break;
        case MIDI_CC_DELAY_TREBLE_CUT:
			echo.treble_cut(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_HICUT, tmp);
            break;
        case MIDI_CC_DELAY_BASS_CUT:
			echo.bass_cut(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_LOWCUT, tmp);
            break;
        case MIDI_CC_DELAY_TREBLE:
			echo.treble(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_TREBLE, tmp);
            break;
        case MIDI_CC_DELAY_BASS:
			echo.bass(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_BASS, tmp);
            break;
		// ---- Booster ---
		case MIDI_CC_BOOST_GAIN:
			booster.drive(tmp*4.0f);
			preset_updateFXparam(PRESET_PARAM_BOOST_DRIVE, tmp);
			break;
		case MIDI_CC_BOOST_BOTTOM:
			booster.bottom(tmp);
			preset_updateFXparam(PRESET_PARAM_BOOST_BOTTOM, tmp);
			break;
		case MIDI_CC_BOOST_PRESENCE:
			booster.tone(tmp);
			preset_updateFXparam(PRESET_PARAM_BOOST_PRESENCE, tmp);
			break;
		case MIDI_CC_BOOST_MIX:
			booster.mix(tmp);
			preset_updateFXparam(PRESET_PARAM_BOOST_MIX, tmp);
			break;			
		case MIDI_CC_BOOST_LEVEL:
			booster.volume(tmp);
			preset_updateFXparam(PRESET_PARAM_BOOST_LEVEL, tmp);
			break;			
		// --- Compressor ---
		case MIDI_CC_COMP_PREGAIN:
			tmp *= 4.0f;
			compressor.setPreGain(tmp);
			preset_updateFXparam(PRESET_PARAM_COMP_PREGAIN, tmp);
			break;
		case MIDI_CC_COMP_THRES:
			tmp = map(tmp, 0.0f, 1.0f, -40.0f, 0.0f);
			compressor.setThresh_dBFS(tmp);
			preset_updateFXparam(PRESET_PARAM_COMP_THRES, tmp);
			break;
		case MIDI_CC_COMP_RATIO:
			tmp = 1.0f + tmp*9.0f; 
			compressor.setCompressionRatio(tmp);
			preset_updateFXparam(PRESET_PARAM_COMP_RATIO, tmp);
			break;
		case MIDI_CC_COMP_ATTACK:
			tmp = map(tmp, 0.0f, 1.0f, 0.001f, 0.2f);
			compressor.setAttack_sec(tmp);
			preset_updateFXparam(PRESET_PARAM_COMP_ATTACK, tmp);
			break;
		case MIDI_CC_COMP_RELEASE:
			tmp = map(tmp, 0.0f, 1.0f, 0.001f, 1.0f);
			compressor.setRelease_sec(tmp);
			preset_updateFXparam(PRESET_PARAM_COMP_RELEASE, tmp);
			break;
		case MIDI_CC_COMP_POSTGAIN:
			tmp *= 4.0f;
			compressor.setPostGain(tmp);
			preset_updateFXparam(PRESET_PARAM_COMP_POSTGAIN, tmp);
			break;
		// --- Amp ---
        case MIDI_CC_GATE_THRES:
			tmp *= -100.0f;
			gate.setThreshold(tmp);
			preset_updateFXparam(PRESET_PARAM_AMP_GATE, tmp);
            break;
        case MIDI_CC_AMP_GAIN:
			amp.gain(tmp);
			preset_updateFXparam(PRESET_PARAM_AMP_GAIN, tmp);
            break;			
        case MIDI_CC_AMP_BASS:
			toneStack.bass(tmp);
			preset_updateFXparam(PRESET_PARAM_AMP_BASS, tmp);
            break;
        case MIDI_CC_AMP_MID:
			toneStack.mid(tmp);
			preset_updateFXparam(PRESET_PARAM_AMP_MID, tmp);
            break;
        case MIDI_CC_AMP_TREBLE:
			toneStack.treble(tmp);
			preset_updateFXparam(PRESET_PARAM_AMP_TREBLE, tmp);
            break;
		case MIDI_CC_AMP_FXMIX:
			xfader_main.mix(tmp);
			preset_updateFXparam(PRESET_PARAM_AMP_FXMIX, tmp);
			break;
		// --- Reverb ---
		case MIDI_CC_REVERBPL_TIME:
			reverbPL.size(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_SIZE, tmp);
			break;
		case MIDI_CC_REVERBPL_DIFF:
			reverbPL.diffusion(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_DIFF, tmp);
			break;
		case MIDI_CC_REVERBPL_MIX:
			reverbPL.mix(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_MIX, tmp);
			break;
		case MIDI_CC_REVERBPL_BASS_CUT:
			reverbPL.lodamp(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_BASSCUT, tmp);
			break;
		case MIDI_CC_REVERBPL_TREBLE_CUT:
			reverbPL.hidamp(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_TREBLECUT, tmp);
			break;
		case MIDI_CC_REVERBPL_BASS:
			tmp = 1.0f - tmp;
			reverbPL.hipass(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_BASS, tmp);
			break;
		case MIDI_CC_REVERBPL_TREBLE:
			tmp = 1.0f - tmp;
			reverbPL.lowpass(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_TREBLE, tmp);
			break;
		case MIDI_CC_REVERBPL_BLEEDIN:
			reverbPL.freezeBleedIn(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_BLEEDIN, tmp);
			break;
		case MIDI_CC_REVERBPL_PITCH:
			tmp8 = reverb_getPitch(tmp);
			reverbPL.pitchSemitones(tmp8);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_PITCH, (float32_t)tmp8);
			break;
		case  MIDI_CC_REVERBPL_PITCHMIX:
			reverbPL.pitchMix(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_PITCHMIX, tmp);
			break;
		case MIDI_CC_REVERBPL_SHIMMER:
			reverbPL.shimmer(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_SHIMMER, tmp);
			break;
		case MIDI_CC_REVERBPL_SHIMM_PITCH:
			tmp8 = reverb_getPitch(tmp);
			reverbPL.shimmerPitchSemitones(tmp8);
			preset_updateFXparam(PRESET_PARAM_REVERBPL_PITCHSHIMM, (float32_t)tmp8);
			break;
		case MIDI_CC_REVERBSC_TIME:
			reverbSC.feedback(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBSC_TIME, tmp);
			break;
		case MIDI_CC_REVERBSC_MIX:
			reverbSC.mix(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBSC_MIX, tmp);
			break;
		case MIDI_CC_REVERBSC_TREBLE:
			tmp = 1.0f - tmp;
			reverbSC.lowpass(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBSC_LOWPASSF, tmp);
			break;
		case MIDI_CC_REVERBSP_TIME:
			reverbSP.time(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBSP_TIME, tmp);
			break;
		case MIDI_CC_REVERBSP_MIX:
			reverbSP.mix(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBSP_MIX, tmp);
			break;
		case MIDI_CC_REVERBSP_BASS_CUT:
			reverbSP.bass_cut(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBSP_BASSCUT, tmp);
			break;
		case MIDI_CC_REVERBSP_TREBLE_CUT:
			reverbSP.treble_cut(tmp);
			preset_updateFXparam(PRESET_PARAM_REVERBSP_TREBLECUT, tmp);
			break;
		case MIDI_CC_DELAY_REVERB_MIX:
			xfader_FX.mix(tmp);
			preset_updateFXparam(PRESET_PARAM_DELAY_REVERB_MIX, tmp);
			break;
		case MIDI_CC_MASTER_VOLUME:
			masterVol.setGain(tmp);
			preset_updateFXparam(PRESET_PARAM_MASTER_VOLUME, tmp);
			break;
		case MIDI_CC_MASTER_PAN:
			tmp = 2.0f * tmp - 1.0f;
			masterVol.setPan(tmp);
			preset_updateFXparam(PRESET_PARAM_MASTER_PAN, tmp);
			break;
		case MIDI_CC_MASTER_LOWCUT:
			masterLowCut.makeupGain(1.0f + tmp*0.5f);
			masterLowCut.setLowShelf(0, MASTER_LOWCUT_FMIN + tmp*MASTER_LOWCUT_FMAX, -15.0f, 1.0f);
			preset_updateFXparam(PRESET_PARAM_MASTER_LOWCUT, tmp);
			break;
        default:    break;
    }
}
// --------------------------------------------------------------
void cb_MidiClock(void)
{
	static uint32_t clk_count = 0;
	if (++clk_count >= 24)
	{
		echo.tap_tempo(false);
		// do someting, ie: Delay tap tempo
		clk_count = 0;
	}
}
