#include "midi.h"
#include "presetSystem.h"
#include "audioComponents.h"
#include "reverbs.h"
#include "console.h"

/**
 * @brief USB MIDI NoteOn callback
 * 
 * @param channel 
 * @param note 
 * @param velocity 
 */
void cb_NoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
	DBG_SERIAL.printf("USB MIDI received NoteOn %d %d\r\n", channel, note);
	if (channel != presetSystem.getMidiChannel() && channel != 16) return;
uint8_t tmp8;
	switch(note)
	{
		// --- Amp + IR ---
		case MIDI_NOTEON_AMP_OFF ... MIDI_NOTEON_AMP_LEAD2:
			tmp8 = note - MIDI_NOTEON_AMP_OFF;
			presetSystem.updateFlag(PRESET_FLAG_AMPMODEL_NO, tmp8);
			break;
		case MIDI_NOTEON_IR_G1 ... MIDI_NOTEON_IR_OFF:
			tmp8 = note - MIDI_NOTEON_IR_G1;
			presetSystem.updateFlag(PRESET_FLAG_CABIR_NO, tmp8);
			break;
		case MIDI_NOTEON_IR_DOUBLER_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_DOUBLER_EN);
			presetSystem.updateFlag(PRESET_FLAG_DOUBLER_EN, !tmp8);
			break;
		case MIDI_NOTEON_IR_DOUBLER_OFF:
			presetSystem.updateFlag(PRESET_FLAG_DOUBLER_EN, false);
			break;
		case MIDI_NOTEON_IR_DOUBLER_ON:
			presetSystem.updateFlag(PRESET_FLAG_DOUBLER_EN, true);
			break;
		// --- Compressor ---
		case MIDI_NOTEON_COMP_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_COMP_EN);
			presetSystem.updateFlag(PRESET_FLAG_COMP_EN, !tmp8);
			break;
		case MIDI_NOTEON_COMP_ON:
			presetSystem.updateFlag(PRESET_FLAG_COMP_EN, true);
			break;
		case MIDI_NOTEON_COMP_OFF:
			presetSystem.updateFlag(PRESET_FLAG_COMP_EN, false);
			break;
		// --- Wah ---
		case MIDI_NOTEON_WAH_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_WAH_EN);
			presetSystem.updateFlag(PRESET_FLAG_WAH_EN, !tmp8);
			break;
		case MIDI_NOTEON_WAH_ON:
			presetSystem.updateFlag(PRESET_FLAG_WAH_EN, true);
			break;
		case MIDI_NOTEON_WAH_OFF:
			presetSystem.updateFlag(PRESET_FLAG_WAH_EN, false);
			break;
		// --- Booster ---
		case MIDI_NOTEON_BOOST_ON:
			presetSystem.updateFlag(PRESET_FLAG_BOOST_EN, true);
			break;
		case MIDI_NOTEON_BOOST_OFF:
			presetSystem.updateFlag(PRESET_FLAG_BOOST_EN, false);
			break;
		case MIDI_NOTEON_BOOST_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_BOOST_EN);
			presetSystem.updateFlag(PRESET_FLAG_BOOST_EN, !tmp8);
			break;
		case MIDI_NOTEON_OCTAVE_ON:
			presetSystem.updateFlag(PRESET_FLAG_OCTAVE_EN, true);
			break;
		case MIDI_NOTEON_OCTAVE_OFF:		
			presetSystem.updateFlag(PRESET_FLAG_OCTAVE_EN, false);
			break;
		case MIDI_NOTEON_OCTAVE_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_OCTAVE_EN);
			presetSystem.updateFlag(PRESET_FLAG_OCTAVE_EN, !tmp8);
			break;
		// --- Reverb ---
		case MIDI_NOTEON_REVERB_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_REVB_EN);
			presetSystem.updateFlag(PRESET_FLAG_REVB_EN, !tmp8);
			break;			
		case MIDI_NOTEON_REVERB_OFF:
			presetSystem.updateFlag(PRESET_FLAG_REVB_EN, false);	
			break;
		case MIDI_NOTEON_REVERB_ON:
			presetSystem.updateFlag(PRESET_FLAG_REVB_EN, true);	
			break;
		case MIDI_NOTEON_REVERBPL_SET:
			presetSystem.updateFlag(PRESET_FLAG_REVBTYPE, (uint8_t)REVERB_PLATE);
			break;
		case MIDI_NOTEON_REVERBSC_SET:
			presetSystem.updateFlag(PRESET_FLAG_REVBTYPE, (uint8_t)REVERB_SC);
			break;	
		case MIDI_NOTEON_REVERBSP_SET:
			presetSystem.updateFlag(PRESET_FLAG_REVBTYPE, (uint8_t)REVERB_SPRING);
			break;
		case MIDI_NOTEON_REVERB_FREEZE_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_REVB_FREEZE_EN);
			if (reverbs_getActive() != REVERB_SPRING)
				presetSystem.updateFlag(PRESET_FLAG_REVB_FREEZE_EN, !tmp8);
			break;
		case MIDI_NOTEON_REVERB_FREEZE_ON:
			presetSystem.updateFlag(PRESET_FLAG_REVB_FREEZE_EN, true);
			break;
		case MIDI_NOTEON_REVERB_FREEZE_OFF:
			presetSystem.updateFlag(PRESET_FLAG_REVB_FREEZE_EN, false);
			break;
		// --- Delay ---
		case MIDI_NOTEON_DELAY_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_DELAY_EN);
			presetSystem.updateFlag(PRESET_FLAG_DELAY_EN, !tmp8);
			break;
		case MIDI_NOTEON_DELAY_OFF: 
			presetSystem.updateFlag(PRESET_FLAG_DELAY_EN, false);
			break;
		case MIDI_NOTEON_DELAY_ON:
			presetSystem.updateFlag(PRESET_FLAG_DELAY_EN, true);
			break;
		case MIDI_NOTEON_DELAY_TAP:
			echo.tap_tempo(true); // with averaging
			break;
		case MIDI_NOTEON_DELAY_FREEZE_TGL:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_DELAY_FREEZE_EN);
			presetSystem.updateFlag(PRESET_FLAG_DELAY_FREEZE_EN, !tmp8);
			break;
		case MIDI_NOTEON_DELAY_FREEZE_ON:
			presetSystem.updateFlag(PRESET_FLAG_DELAY_FREEZE_EN, true);
			break;
		case MIDI_NOTEON_DELAY_FREEZE_OFF:
			presetSystem.updateFlag(PRESET_FLAG_DELAY_FREEZE_EN, false);
			break;
		// --- Preset System ---
		case MIDI_NOTEON_PRESET_1 ... MIDI_NOTEON_PRESET_8:
			tmp8 = note - MIDI_NOTEON_PRESET_1;
			presetSystem.load(tmp8);
			break;
		case MIDI_NOTEON_PRESET_SAVE:
			presetSystem.save();
			break;
		// --- Input Selector ---
		case MIDI_NOTEON_SELIN_LR:
			presetSystem.updateFlag(PRESET_FLAG_INSELECT, (uint8_t)AudioSwitchSelectorStereo::SIGNAL_SELECT_LR);
			break;
		case MIDI_NOTEON_SELIN_L:
			presetSystem.updateFlag(PRESET_FLAG_INSELECT, (uint8_t)AudioSwitchSelectorStereo::SIGNAL_SELECT_L);
			break;
		case MIDI_NOTEON_SELIN_R:
			presetSystem.updateFlag(PRESET_FLAG_INSELECT, (uint8_t)AudioSwitchSelectorStereo::SIGNAL_SELECT_R);
			break;
		// --- Diverse ---
		case MIDI_NOTEON_TERMINAL_TGL:
			console_tglMode();
			break;
		// --- HW control ---
		case MIDI_NOTEON_CTRL_DRY:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_DRYSIG);
			presetSystem.updateFlag(PRESET_FLAG_DRYSIG, !tmp8);
			break;
		case MIDI_NOTEON_CTRL_BYPASS:
			tmp8 = presetSystem.getFlag(PRESET_FLAG_BYPASS);
			presetSystem.updateFlag(PRESET_FLAG_BYPASS, !tmp8);
			break;
		case MIDI_NOTEON_MCU_RESET:
			SCB_AIRCR = 0x05FA0004; // MCU reset
			break;
		default:
			break;
	}
}

// --------------------------------------------------------------
void cb_ControlChange(uint8_t channel, uint8_t control, uint8_t value)
{
	if (channel != presetSystem.getMidiChannel() && channel != 16) return;
	float32_t valuef = (float32_t) value / 127.0f;
	switch(control)
	{
		// --- Delay --- 
		case MIDI_CC_DELAY_TIME:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_TIME, valuef);
			break;
		case MIDI_CC_DELAY_INERTIA:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_INERTIA, valuef);
			break;
		case MIDI_CC_DELAY_FEEDBACK: 
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_REPEATS, valuef);
			break;
		case MIDI_CC_DELAY_MIX:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_MIX, valuef);
			break;
		case MIDI_CC_DELAY_MOD_RATE:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_MODRATE, valuef);
			break;
		case MIDI_CC_DELAY_MOD_DEPTH:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_MODDEPTH, valuef);
			break;
		case MIDI_CC_DELAY_TREBLE_CUT:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_HICUT, valuef);
			break;
		case MIDI_CC_DELAY_BASS_CUT:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_LOWCUT, valuef);
			break;
		case MIDI_CC_DELAY_TREBLE:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_TREBLE, valuef);
			break;
		case MIDI_CC_DELAY_BASS:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_BASS, valuef);
			break;
		// --- Wah ---
		case MIDI_CC_WAH_FREQ:
			presetSystem.updateFXparam(PRESET_PARAM_WAH_FREQ, valuef);
			break;
		case MIDI_CC_WAH_MIX:
			presetSystem.updateFXparam(PRESET_PARAM_WAH_MIX, valuef);
			break;
		// ---- Booster ---
		case MIDI_CC_BOOST_GAIN:
			presetSystem.updateFXparam(PRESET_PARAM_BOOST_DRIVE, valuef);
			break;
		case MIDI_CC_BOOST_BOTTOM:
			presetSystem.updateFXparam(PRESET_PARAM_BOOST_BOTTOM, valuef);
			break;
		case MIDI_CC_BOOST_PRESENCE:
			presetSystem.updateFXparam(PRESET_PARAM_BOOST_PRESENCE, valuef);
			break;
		case MIDI_CC_BOOST_MIX:
			presetSystem.updateFXparam(PRESET_PARAM_BOOST_MIX, valuef);
			break;			
		case MIDI_CC_BOOST_LEVEL:
			presetSystem.updateFXparam(PRESET_PARAM_BOOST_LEVEL, valuef);
			break;			
		// --- Compressor ---
		case MIDI_CC_COMP_PREGAIN:
			presetSystem.updateFXparam(PRESET_PARAM_COMP_PREGAIN, valuef);
			break;
		case MIDI_CC_COMP_THRES:
			presetSystem.updateFXparam(PRESET_PARAM_COMP_THRES, valuef);
			break;
		case MIDI_CC_COMP_RATIO:
			presetSystem.updateFXparam(PRESET_PARAM_COMP_RATIO, valuef);
			break;
		case MIDI_CC_COMP_ATTACK:
			presetSystem.updateFXparam(PRESET_PARAM_COMP_ATTACK, valuef);
			break;
		case MIDI_CC_COMP_RELEASE:
			presetSystem.updateFXparam(PRESET_PARAM_COMP_RELEASE, valuef);
			break;
		case MIDI_CC_COMP_POSTGAIN:
			presetSystem.updateFXparam(PRESET_PARAM_COMP_POSTGAIN, valuef);
			break;
		// --- Amp ---
		case MIDI_CC_GATE_THRES:
			presetSystem.updateFXparam(PRESET_PARAM_AMP_GATE, valuef);
			break;
		case MIDI_CC_AMP_GAIN:
			presetSystem.updateFXparam(PRESET_PARAM_AMP_GAIN, valuef);
			break;			
		case MIDI_CC_AMP_BASS:
			presetSystem.updateFXparam(PRESET_PARAM_AMP_BASS, valuef);
			break;
		case MIDI_CC_AMP_MID:
			presetSystem.updateFXparam(PRESET_PARAM_AMP_MID, valuef);
			break;
		case MIDI_CC_AMP_TREBLE:
			presetSystem.updateFXparam(PRESET_PARAM_AMP_TREBLE, valuef);
			break;
		case MIDI_CC_AMP_FXMIX:
			presetSystem.updateFXparam(PRESET_PARAM_AMP_FXMIX, valuef);
			break;
		// --- Reverb ---
		case MIDI_CC_REVERBPL_TIME:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_SIZE, valuef);
			break;
		case MIDI_CC_REVERBPL_DIFF:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_DIFF, valuef);
			break;
		case MIDI_CC_REVERBPL_MIX:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_MIX, valuef);
			break;
		case MIDI_CC_REVERBPL_BASS_CUT:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_BASSCUT, valuef);
			break;
		case MIDI_CC_REVERBPL_TREBLE_CUT:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_TREBLECUT, valuef);
			break;
		case MIDI_CC_REVERBPL_BASS:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_BASS, valuef);
			break;
		case MIDI_CC_REVERBPL_TREBLE:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_TREBLE, valuef);
			break;
		case MIDI_CC_REVERBPL_BLEEDIN:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_BLEEDIN, valuef);
			break;
		case MIDI_CC_REVERBPL_PITCH:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_PITCH, valuef);
			break;
		case  MIDI_CC_REVERBPL_PITCHMIX:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_PITCHMIX, valuef);
			break;
		case MIDI_CC_REVERBPL_SHIMMER:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_SHIMMER, valuef);
			break;
		case MIDI_CC_REVERBPL_SHIMM_PITCH:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBPL_PITCHSHIMM, valuef);
			break;
		case MIDI_CC_REVERBSC_TIME:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBSC_TIME, valuef);
			break;
		case MIDI_CC_REVERBSC_MIX:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBSC_MIX, valuef);
			break;
		case MIDI_CC_REVERBSC_TREBLE:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBSC_LOWPASSF, valuef);
			break;
		case MIDI_CC_REVERBSP_TIME:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBSP_TIME, valuef);
			break;
		case MIDI_CC_REVERBSP_MIX:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBSP_MIX, valuef);
			break;
		case MIDI_CC_REVERBSP_BASS_CUT:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBSP_BASSCUT, valuef);
			break;
		case MIDI_CC_REVERBSP_TREBLE_CUT:
			presetSystem.updateFXparam(PRESET_PARAM_REVERBSP_TREBLECUT, valuef);
			break;
		case MIDI_CC_DELAY_REVERB_MIX:
			presetSystem.updateFXparam(PRESET_PARAM_DELAY_REVERB_MIX, valuef);
			break;
		case MIDI_CC_MASTER_VOLUME:
			presetSystem.updateFXparam(PRESET_PARAM_MASTER_VOLUME, valuef);
			break;
		case MIDI_CC_MASTER_PAN:
			presetSystem.updateFXparam(PRESET_PARAM_MASTER_PAN, valuef);
			break;
		case MIDI_CC_MASTER_LOWCUT:
			presetSystem.updateFXparam(PRESET_PARAM_MASTER_LOWCUT, valuef);
			break;
		default:    break;
	}	
}

/**
 * @brief MIDI Program Change callback
 * 			preset switching
 */
void cb_ProgramChange(uint8_t channel, uint8_t program)
{
	if (channel != presetSystem.getMidiChannel() && channel != 16) return;
	if (program < 128) 
	{
		presetSystem.load(program);
		presetSystem.triggerFullGuiUpdate();
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