#ifndef _HW_T40_ES8388_H_
#define _HW_T40_ES8388_H_

#include <Wire.h>
#include "hexefx_audiolib_F32.h"
#include "hw_proto.h"

/* Teensy 4.0 + ES8388 on I2S1, config via I2C (Wire)
 * Differential ADC input
 * 24bit, 44.1kHz
 * Analog bypass using internal switches
 */
class hw_t40_ES8388 : public hw_TGX4_prototype 
{
public:
	hw_t40_ES8388(){};
	~hw_t40_ES8388(){};
	bool init(void)
	{	
		// use differential ADC input
		if (!codec.enable(&Wire, ES8388_I2C_ADDR_H, AudioControlES8388_F32::ES8388_CFG_LINEIN_DIFF))
		{
			DBG_SERIAL.println("Codec init error!");
			return false;
		}
		return true;
	}

	bool dry_set(hw_state_t newState) { return state_set(newState); }
	bool dry_get() { return dry_state; } 

	bool wet_set(hw_state_t newState) { return state_set(newState);	}
	bool wet_get() { return wet_state; }

	bool state_set(hw_state_t newState)
	{
		if (newState == HW_STATE_TOGGLE) 	state ^= 1;
		else 								state = (bool) newState;	
		uint8_t out = state ^ ctrl_inv;
		codec.analogBypass(!out);

		return state;
	}
	bool state_get() { return state; }

	bool process(void)
	{
		return false;
	}

	bool phase_invert = false;	// output signal phase invert

private:
	bool state = true;
	bool dry_state = false;
	bool wet_state = true;
	bool ctrl_inv = 0; // set to one for active low GPIO outputs
	// ES8388 on Wire
	AudioControlES8388_F32 codec;
};

#if defined (USE_T40_ES8388)
	extern 	hw_t40_ES8388 hw;
#endif

#endif // _HW_ES8388_H_
