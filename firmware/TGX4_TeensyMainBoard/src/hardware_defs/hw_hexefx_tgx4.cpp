/**
 * @file hx_hexefx_tgx4.cpp
 * @author Piotr Zapart (www.hexefx.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-15
 * 
 * @copyright Copyright (c) 2024
 *
 */
#include "hw_hexefx_tgx4.h"
#include "midi.h"
#include "debug_log.h"
#include "presetSystem.h"
#include "audioComponents.h" // (echo tap tempo)

#if defined(USE_HEXEFX_TGX4)
	hw_hexefx_TGX4 hw;
#endif

hw_hexefx_TGX4*  hw_hexefx_TGX4_ptr;

FLASHMEM bool hw_hexefx_TGX4::init()
{
	hw_hexefx_TGX4_ptr = this;
	pinMode(dry_ctrl_pin, OUTPUT);
	pinMode(wet_ctrl_pin, OUTPUT);
	dry_set(HW_STATE_OFF);
	wet_set(HW_STATE_ON);
	pinMode(expSense_Pin, INPUT_PULLUP);
	ftswL.init();
	ftswM.init();
	ftswR.init();

	pinMode(codecPDown_pin, OUTPUT);
	digitalWriteFast(codecPDown_pin, LOW); // Power down the ADC+DAC
	delay(10);
	digitalWriteFast(codecPDown_pin, HIGH); // Power up the ADC+DAC
	delay(10);
	if (!audio_adc.enable())
	{
		DBG_println("AK5552 ADC init error!");
		return false;		
	}
	if (!audio_dac.enable())
	{
		DBG_println("AK4452 DAC init error!");
		return false;		
	}
	// pots
	pots[POT1].begin(adcIn_pinMap[POT1], "POT1", true, 0.01f, POT_TAPER_LIN, 16);
	pots[POT2].begin(adcIn_pinMap[POT2], "POT2", true, 0.01f, POT_TAPER_LIN, 16);
	pots[POT3].begin(adcIn_pinMap[POT3], "POT3", true, 0.01f, POT_TAPER_LIN, 16);
	pots[EXP_IN].begin(adcIn_pinMap[EXP_IN], "EXP", true, 0.01f, POT_TAPER_LIN, 16);

	// disable not used POT inputs
	pots[POT2].enable(false);
	pots[POT3].enable(false);

	ftswL.set_press_cb( []() { presetSystem.updateFlag(PRESET_FLAG_BYPASS, !hw_hexefx_TGX4_ptr->state_set(HW_STATE_TOGGLE));});
	ftswM.set_press_cb( []() { hw_hexefx_TGX4_ptr->footswPressHandler(presetSystem.getFlag(PRESET_FLAG_FT1_ASSIGN), 1);});
	ftswR.set_press_cb( []() { hw_hexefx_TGX4_ptr->footswPressHandler(presetSystem.getFlag(PRESET_FLAG_FT2_ASSIGN), 2);});
	ftswM.set_pressLong_cb( []() { hw_hexefx_TGX4_ptr->footswHoldHandler(presetSystem.getFlag(PRESET_FLAG_FT1_ASSIGN), 1);});
	ftswR.set_pressLong_cb( []() { hw_hexefx_TGX4_ptr->footswHoldHandler(presetSystem.getFlag(PRESET_FLAG_FT2_ASSIGN), 2);});	
	return true;
}

FLASHMEM bool hw_hexefx_TGX4::process(void)
{
	uint16_t i;
	uint8_t assign = 0;
	float32_t value = 0.0f;
	for (i = 0; i<sizeof(adcIn_pinMap); i++)
	{
		pots[i].update();
		if (pots[i].hasChanged())
		{
			value = pots[i].getValue_f();
			switch(i)
			{
				case POT1:
					assign = presetSystem.getFlag(PRESET_FLAG_POT1_ASSIGN);
					break;
				case POT2:
					assign = presetSystem.getFlag(PRESET_FLAG_POT2_ASSIGN);	
					break;
				case POT3:
					assign = presetSystem.getFlag(PRESET_FLAG_POT1_ASSIGN);	
					break;
				case EXP_IN:
				if (digitalRead(expSense_Pin))
						assign = presetSystem.getFlag(PRESET_FLAG_EXP_ASSIGN);
					break;
			}
			if (assign && millis() > 1000) presetSystem.updateFXparam( (preset_param_t)(assign-1),  value);
		}
	}
	ftswL.update();
	ftswM.update();
	ftswR.update();
	return false;
}

void hw_hexefx_TGX4::footswPressHandler(uint8_t assign, uint8_t ftNo)
{
	if (assign >= PRESET_FTASSIGN_LAST) return;
	tgx4Led* led = ftNo == 1 ? &ledM : &ledR;
	uint8_t state;
	switch(assign)
	{
		case PRESET_FTASSIGN_OFF:
			break;
		case PRESET_FTASSIGN_COMP:
			state = !presetSystem.getFlag(PRESET_FLAG_COMP_EN);
			presetSystem.updateFlag(PRESET_FLAG_COMP_EN, state);
			led->set(state);
			break;
		case PRESET_FTASSIGN_WAH:
			state = !presetSystem.getFlag(PRESET_FLAG_WAH_EN);
			presetSystem.updateFlag(PRESET_FLAG_WAH_EN, state);
			led->set(state);
			break;
		case PRESET_FTASSIGN_BOOST:
			state = !presetSystem.getFlag(PRESET_FLAG_BOOST_EN);
			presetSystem.updateFlag(PRESET_FLAG_BOOST_EN, state);
			led->set(state);		
			break;
		case PRESET_FTASSIGN_OCT:
			state = !presetSystem.getFlag(PRESET_FLAG_OCTAVE_EN);
			presetSystem.updateFlag(PRESET_FLAG_OCTAVE_EN, state);
			led->set(state);		
			break;
		case PRESET_FTASSIGN_DELAY:
			state = !presetSystem.getFlag(PRESET_FLAG_DELAY_EN);
			presetSystem.updateFlag(PRESET_FLAG_DELAY_EN, state);
			led->set(state);		
			break;
		case PRESET_FTASSIGN_DELAY_FREEZE:
			state = !presetSystem.getFlag(PRESET_FLAG_DELAY_FREEZE_EN);
			presetSystem.updateFlag(PRESET_FLAG_DELAY_FREEZE_EN, state);
			led->set(state);		
			break;
		case PRESET_FTASSIGN_DELAY_TAP:
			echo.tap_tempo();
			break;
		case PRESET_FTASSIGN_REVERB:
			state = !presetSystem.getFlag(PRESET_FLAG_REVB_EN);
			presetSystem.updateFlag(PRESET_FLAG_REVB_EN, state);
			led->set(state);
			break;
		case PRESET_FTASSIGN_REVERB_FREEZE:
			state = !presetSystem.getFlag(PRESET_FLAG_REVB_FREEZE_EN);
			presetSystem.updateFlag(PRESET_FLAG_REVB_FREEZE_EN, state);
			led->set(state);		
			break;
		case PRESET_FTASSIGN_PRESET_UP:
			presetSystem.preLoad_next();
			break;
		case PRESET_FTASSIGN_PRESET_DOWN:
			presetSystem.preLoad_previous();
			break;	
	}
}
void hw_hexefx_TGX4::footswHoldHandler(uint8_t assign, uint8_t ftNo)
{
	if (assign >= PRESET_FTASSIGN_LAST) return;
	uint8_t state;
	switch(assign)
	{
		case PRESET_FTASSIGN_OFF:
			break;
		case PRESET_FTASSIGN_COMP:

			break;
		case PRESET_FTASSIGN_WAH:

			break;
		case PRESET_FTASSIGN_BOOST:
		
			break;
		case PRESET_FTASSIGN_OCT:
	
			break;
		case PRESET_FTASSIGN_DELAY:
			state = !presetSystem.getFlag(PRESET_FLAG_DELAY_FREEZE_EN);
			presetSystem.updateFlag(PRESET_FLAG_DELAY_FREEZE_EN, state);		
			break;
		case PRESET_FTASSIGN_DELAY_FREEZE:
	
			break;
		case PRESET_FTASSIGN_DELAY_TAP:
			break;
		case PRESET_FTASSIGN_REVERB:
			state = !presetSystem.getFlag(PRESET_FLAG_REVB_FREEZE_EN);
			presetSystem.updateFlag(PRESET_FLAG_REVB_FREEZE_EN, state);
			break;
		case PRESET_FTASSIGN_REVERB_FREEZE:
	
			break;
		case PRESET_FTASSIGN_PRESET_UP:	
		case PRESET_FTASSIGN_PRESET_DOWN:
			presetSystem.activate();
			break;
	}
}