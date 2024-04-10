#ifndef _HW_BLACKADDR_TGAPRO_H_
#define _HW_BLACKADDR_TGAPRO_H_

#include <Wire.h>
#include "hexefx_audio_F32.h"
#include "hw_proto.h"

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
	bool init(void)
	{
		if (!codec.enable()) 	// codec init
		{
			DBG_SERIAL.println("Codec init error!");
			return false;
		}
		codec.inputSelect(AUDIO_INPUT_LINEIN);
		codec.inputLevel(0.77f); 
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

	// WM8731
	AudioControlWM8731_F32 codec;
};


#endif // _HW_BLACKADDR_TGAPRO_H_
