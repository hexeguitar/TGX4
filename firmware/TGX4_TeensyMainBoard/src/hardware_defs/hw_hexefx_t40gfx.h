#ifndef _HW_HEXEFX_T40GFX_H_
#define _HW_HEXEFX_T40GFX_H_

#include <Wire.h>
#include "hexefx_audio_F32.h"
#include "hw_proto.h"

class hw_hexefx_t40GFX : public hw_TGX4_prototype
{
public:
	hw_hexefx_t40GFX(){};
	~hw_hexefx_t40GFX(){};

	/**
	 * @brief main init method - 
	 * @return true - success
	 * @return false - fail
	 */
	bool init(void)
	{
		pinMode(bp_ctrl_pin, OUTPUT);
		pinMode(led0_pin, OUTPUT);
		state_set(HW_STATE_ON);
		if (!codec.enable(&Wire1, 0x0A, 0, 0))
		{
			DBG_SERIAL.println("Codec init error!");
			return false;
		}
		return true;
	}
	// No separate dry/wet switches, use bypass for all
	bool dry_set(hw_state_t newState) { return state_set(newState); }
	bool dry_get() { return state; }

	bool wet_set(hw_state_t newState) {	return state_set(newState);	}
	bool wet_get() { return state; }

	bool state_set(hw_state_t newState)
	{
		if (newState == HW_STATE_TOGGLE) 	state ^= 1;
		else 								state = (bool) newState;	
		uint8_t out = state ^ ctrl_inv;
		digitalWriteFast(bp_ctrl_pin, out);
		digitalWriteFast(led0_pin, out);		
		return state;
	}
	bool state_get() { return state; }

	bool process(void)
	{
		return false;
	}

	bool phase_invert = true; // Output phase invert setting

private:
	bool state = true;
	static const uint8_t ctrl_inv = 1;
	// pin definitions:
	static const uint8_t bp_ctrl_pin = 2;
	static const uint8_t led0_pin = 5;
	const uint8_t led1_pin = 6;
	const uint8_t ftswL_pin = 22;
	const uint8_t ftswR_pin = 25;
	const uint8_t pot0_pin = 14;
	const uint8_t pot1_pin = 15;
	const uint8_t pot2_pin = 18;
	const uint8_t pot3_pin = 19;
	// SGTL5000 on Wire1
	AudioControlSGTL5000_F32 codec;
};

#if defined (USE_HEXEFX_T40GFX)
	extern hw_hexefx_t40GFX hw;
#endif

#endif // _HW_HEXEFX_T40GFX_H_
