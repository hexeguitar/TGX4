/**
 * @file hw_hexefx_tgx4.h
 * @author Piotr Zapart (www.hexefx.com)
 * @brief TGX4 Teensy Guitar Amp Project - dedicated hardware driver
 * @version 1.0
 * @date 2024-07-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _HW_HEXEFX_TGX4_H_
#define _HW_HEXEFX_TGX4_H_

#include <Arduino.h>
#include <Wire.h>
#include "hexefx_audio_F32.h"
#include "hw_proto.h"
#include "tgx4_led.h"
#include "tgx4_pot.h"
#include "tgx4_button.h"
#include "debug_log.h"

class hw_hexefx_TGX4 : public hw_TGX4_prototype
{
public:
	hw_hexefx_TGX4(TwoWire* i2cBus=&Wire1) : bus(i2cBus){};
	~hw_hexefx_TGX4(){};
	/**
	 * @brief main init method - 
	 * @return true - success
	 * @return false - fail
	 */
	bool init();
	
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
		digitalWrite(wet_ctrl_pin, out);
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
		digitalWriteFast(dry_ctrl_pin, state ? dry_state^ctrl_inv : HIGH^ctrl_inv);
		digitalWriteFast(wet_ctrl_pin, out);
		ledL.set(state);
		return state;
	}
	bool state_get() { return state; }

	bool process(void);

	bool phase_invert = false; // Output phase invert setting
private:

	bool state = true;
	bool dry_state = false;
	bool wet_state = true;

	static const uint8_t ctrl_inv = 1;
	const uint8_t dry_ctrl_pin = 9;
	const uint8_t wet_ctrl_pin = 6;	

	tgx4Button ftswL = tgx4Button(24);
	tgx4Button ftswM = tgx4Button(25);
	tgx4Button ftswR = tgx4Button(26);
	void footswPressHandler(uint8_t assign, uint8_t ftNo);
	void footswHoldHandler(uint8_t assign, uint8_t ftNo);

	const uint8_t expSense_Pin = 14;

	tgx4Led ledL = tgx4Led(30);
	tgx4Led ledM = tgx4Led(31);
	tgx4Led ledR = tgx4Led(32);

	// POT1, POT2, POT3, EXP_PEDAL_IN
	typedef enum
	{
		POT1 = 0, POT2, POT3, EXP_IN
	}pot_inputs_t;

	const uint8_t adcIn_pinMap[4] = 
		{27, 40, 41, 15};

	tgx4Pot pots[sizeof(adcIn_pinMap)];

	const uint8_t i2cS_addr = 0x55;	// i2c slave address for the gui control
	const uint8_t codecPDown_pin = 29;

	// Timing
	uint32_t t_last;

	// --- define the used codec here ---
	AudioControlAK5552_F32 audio_adc;
	AudioControlAK4452_F32 audio_dac;
	// I2C bus used to config the ADC+DAC
	TwoWire *bus;
	
};
#if defined(USE_HEXEFX_TGX4)
	extern hw_hexefx_TGX4 hw;
#endif

#endif // _HW_HEXEFX_TGX4_H_
