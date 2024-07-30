#ifndef _HW_BLACKADDR_TGAPRO_H_
#define _HW_BLACKADDR_TGAPRO_H_

#include <Wire.h>
#include "hexefx_audio_F32.h"
#include "hw_proto.h"
#include "tgx4_led.h"
#include "tgx4_pot.h"
#include "tgx4_button.h"
#include "presetSystem.h"

class hw_Blackaddr_TGAPro : public hw_TGX4_prototype
{
public:
	hw_Blackaddr_TGAPro(){};
	~hw_Blackaddr_TGAPro(){};
	
/**
	 * @brief main init method - 
	 * @return true - success
	 * @return false - fail
	 */
	bool init(void);

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
		codec.dry_enable(dry_state);
		return dry_state;
	}
	bool dry_get() { return dry_state; }

	/**
	 * @brief Wet signal analog switch control
	 * 
	 * @param newState on, of or toggle
	 * @return true 	new state
	 * @return false 
	 */
	bool wet_set(hw_state_t newState)
	{
		if (newState == HW_STATE_TOGGLE) 	wet_state ^= 1;
		else 								wet_state = (bool) newState;	
		codec.dac_enable(wet_state);
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
		codec.bypass_set(!state);
		return state;
	}
	bool state_get() { return state; }

	/**
	 * @brief process method, use to read pots, footswitches ets
	 * 
	 * @return true sign that a change occured
	 * @return false no change in params
	 */
	bool process(void);

	// In case the output signal phase has to be inverted due to 
	// ie. inverting mixer or DAC filter set it here to true  
	bool phase_invert = false;	

private:
	bool state = true;
	bool dry_state = false;
	bool wet_state = true;

	// WM8731
	AudioControlWM8731_F32 codec;

	typedef enum
	{
		POT1 = 0, POT2, POT3
	}pot_inputs_t;

	tgx4Pot pots[3];

	tgx4Led led1 = tgx4Led(4);
	tgx4Led led2 = tgx4Led(5);

	tgx4Button sw1 = tgx4Button(2);
	tgx4Button sw2 = tgx4Button(3);
	void swPressHandler(uint8_t assign, uint8_t ftNo);
	void swHoldHandler(uint8_t assign, uint8_t ftNo);

};

#if defined (USE_BLACKADDR_TGAPRO)
	extern hw_Blackaddr_TGAPro hw;
#endif

#endif // _HW_BLACKADDR_TGAPRO_H_
