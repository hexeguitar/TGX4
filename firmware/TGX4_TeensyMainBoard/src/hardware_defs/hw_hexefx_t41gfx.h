/**
 * @file hw_hexefx_t41gfx.h
 * @author Piotr Zapart
 * @brief 	hardware definitions for hexefx T41_GFX pedal
 * @version 0.1
 * @date 2024-04-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _HW_HEXEFX_T41GFX_H_
#define _HW_HEXEFX_T41GFX_H_

#include <Wire.h>
#include "hexefx_audio_F32.h"
#include "hw_proto.h"

class hw_hexefx_t41GFX : public hw_TGX4_prototype
{
public:
	hw_hexefx_t41GFX(){};
	~hw_hexefx_t41GFX(){};
	/**
	 * @brief main init method - 
	 * @return true - success
	 * @return false - fail
	 */
	bool init(void)
	{
		pinMode(dry_ctrl_pin, OUTPUT);
		pinMode(wet_ctrl_pin, OUTPUT);
		dry_set(HW_STATE_OFF);
		wet_set(HW_STATE_ON);
		
		if (!codec.enable()) 	// codec init
		{
			DBG_SERIAL.println("Codec init error!");
			return false;
		}
		codec.inputSelect(AUDIO_INPUT_LINEIN);
		codec.inputLevel(0.77f); 
		codec.hp_filter(false);
		return true;
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
		if (newState == HW_STATE_TOGGLE) 	dry_state ^= 1;
		else 								dry_state = (bool) newState;	
		uint8_t out = dry_state ^ ctrl_inv;
		digitalWriteFast(dry_ctrl_pin, out);
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
		digitalWriteFast(wet_ctrl_pin, out);
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
		digitalWriteFast(dry_ctrl_pin, state ? dry_state : HIGH^ctrl_inv);
		digitalWriteFast(wet_ctrl_pin, out);	
		return state;
	}
	bool state_get() { return state; }

	bool process(void)
	{
		return false;
	}

	bool phase_invert = false; // Output phase invert setting
private:
	bool state = true;
	bool dry_state = false;
	bool wet_state = true;

	static const uint8_t ctrl_inv = 1;

	// pin definitions:
	const uint8_t dry_ctrl_pin = 28;
	const uint8_t wet_ctrl_pin = 29;	

	const uint8_t ftswL_pin = 37;
	const uint8_t ftswM_pin = 36;
	const uint8_t ftswR_pin = 35;

	// --- define the used codec here ---
	// WM8731 on Wire
	AudioControlWM8731_F32 codec;
};
#if defined (USE_HEXEFX_T41GFX)
	extern hw_hexefx_t41GFX hw;
#endif

#endif // _HW_HEXEFX_T41GFX_H_
