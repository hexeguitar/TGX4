#ifndef _HW_TEENSY_AUDIOBOARD_H_
#define _HW_TEENSY_AUDIOBOARD_H_

#include <Wire.h>
#include "hexefx_audio_F32.h"
#include "tgx4_pot.h"
#include "hw_proto.h"
#include "presetSystem.h"

class hw_Teensy_AudioBoard : public hw_TGX4_prototype
{
public:
	hw_Teensy_AudioBoard(){};
	~hw_Teensy_AudioBoard(){};
	bool init(void)
	{
		if (!codec.enable())
		{
			DBG_SERIAL.println("Codec init error!");
			return false;
		}
		codec.inputSelect(AUDIO_INPUT_LINEIN);
		codec.volume(0.0f);
		codec.micGain(0);
		codec.muteHeadphone();
		codec.lineInLevel(1);
		codec.lineOutLevel(13);
		codec.adcHighPassFilterDisable();
		pot.begin(15, "POT1", true, 0.01f, POT_TAPER_LIN, 16);

		return true;
	}
	bool dry_set(hw_state_t newState)
	{
		dry_state = false;
		return dry_state;
	}
	bool dry_get() { return dry_state; }
	
	bool wet_set(hw_state_t newState)
	{
		wet_state = true;
		return wet_state;
	}
	bool wet_get() { return wet_state; }
	
	bool state_set(hw_state_t newState)
	{
		state = true;
		return state;
	}
	bool state_get() { return state; }

	bool process(void)
	{
		pot.update();
		if (pot.hasChanged())
		{
			float32_t value = pot.getValue_f();
			uint8_t assign = presetSystem.getFlag(PRESET_FLAG_EXP_ASSIGN);
			if (assign && millis() > 1000) presetSystem.updateFXparam( (preset_param_t)(assign-1),  value);
		}
		return true;
	}
	// In case the output signal phase has to be inverted due to 
	// ie. inverting mixer or DAC filter set it here to true  
	bool phase_invert = false;	

private:
	bool state = true;
	bool dry_state = false;
	bool wet_state = true;
	bool ctrl_inv = 0; // set to one for active low GPIO outputs

	tgx4Pot pot;


	// SGTL5000 on Wire
	AudioControlSGTL5000_F32 codec;
};

#if defined (USE_TEENSY_AUDIO_BOARD)
	extern hw_Teensy_AudioBoard hw;
#endif

#endif // _HW_TEENSY_AUDIOBOARD_H_
