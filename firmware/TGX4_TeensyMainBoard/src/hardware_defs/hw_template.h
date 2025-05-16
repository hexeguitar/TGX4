#ifndef _HW_TEMPLATE_H_
#define _HW_TEMPLATE_H_

#include <Wire.h>
#include "hexefx_audiolib_F32.h"
#include "hw_proto.h"
//#include "yourCodecDriver.h" - if it's not the part of the library

class hw_templateHardwarePlatorm : public hw_TGX4_prototype
{
public:
	hw_templateHardwarePlatorm(){};
	~hw_templateHardwarePlatorm(){};
	/**
	 * @brief main init method - 
	 * @return true - success
	 * @return false - fail
	 */
	bool init(void)
	{
		
		/* configure the codec chip here, required settings:
		 * - I2S mode
		 * - 256 x fs
		 * - 32bit word length, lsb first mode
		 * - 24bit 
		 * - 44.1kHz
		 * 
		 *  Configure additional hardware like logic ouputs, inputs, pots etc
		 * 	
		 * 	return true if success
		 */
		return false;
	}

	/**
	 * @brief Dry signal analog switch control
	 * 
	 * @param newState on, off, or toggle
	 * @return true 	new state
	 * @return false 
	 */
	bool dry_set(hw_state_t newState)
	{
		return dry_state;
	}
	bool dry_get() { return dry_state; } 

	/**
	 * @brief Wet signal analog switch control
	 * 
	 * @param newState on, off, or toggle
	 * @rn false 
	 */
	bool wet_set(hw_state_t newState)
	{
		if (newState == HW_STATE_TOGGLE) 	wet_state ^= 1;
		else 								wet_state = (bool) newState;
		uint8_t out = wet_state ^ ctrl_inv;
		// set the new state here via GPIO or by other means

		return wet_state;
	}
	bool wet_get() { return wet_state; }

	/**
	 * @brief Master bypass control. Works it two ways:
	 * 		1. If the Dry signal is enabled, toggles the WET only, 
	 * 			used for time based effects which are added in parallel
	 * 			to the original sound
	 *		2. Dry/Wet switch, mutes the Dry signal when the FX is on
	 * 
	 * @param newState 
	 * @return true 
	 * @return false 
	 */
	bool state_set(hw_state_t newState)
	{
		if (newState == HW_STATE_TOGGLE) 	state ^= 1;
		else 								state = (bool) newState;	
		uint8_t out = state ^ ctrl_inv;
		// Set matster bypass, ie:
		// digitalWriteFast(masterBypassPin, out)
		// Some codecs like the ES8388 or WM8731 have internal switches 
		// which can be used for analog bypass

		return state;
	}
	bool state_get() { return state; }

	bool process(void)
	{
		return true;
	}
	// used to set any available leds
	void led_set(uint8_t ledNo, hw_state_t state) {};

	bool phase_invert = false; // Output phase invert setting
private:
	bool state = false;
	bool dry_state = false;
	bool wet_state = false;
	bool ctrl_inv = 0; // set to one for active low GPIO outputs

	// --- define the used codec here ---
	// create the codec class instance here
	// YourCodecDriverClass codec;
};

/*
 * Change the names accordingly:
 * USE_TEMPLATE_HARDWARE_PLATFORM to the defined name of your hw platform
 * in the platformio.ini file, eg. -DUSE_TEMPLATE_HARDWARE_PLATFORM
 * 
 */ 

#if defined (USE_TEMPLATE_HARDWARE_PLATFORM)
	extern	hw_templateHardwarePlatorm hw;
#endif
#endif // _HW_TEMPLATE_H_
