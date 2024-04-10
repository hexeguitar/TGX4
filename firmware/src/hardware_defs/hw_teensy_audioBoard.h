#ifndef _HW_TEENSY_AUDIOBOARD_H_
#define _HW_TEENSY_AUDIOBOARD_H_

#include <Wire.h>
#include "hexefx_audio_F32.h"
#include "hw_proto.h"

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
		return false;
	}	
	// In case the output signal phase has to be inverted due to 
	// ie. inverting mixer or DAC filter set it here to true  
	bool phase_invert = false;	

private:
	bool state = true;
	bool dry_state = false;
	bool wet_state = true;
	bool ctrl_inv = 0; // set to one for active low GPIO outputs
	// SGTL5000 on Wire
	AudioControlSGTL5000_F32 codec;
};


#endif // _HW_TEENSY_AUDIOBOARD_H_
