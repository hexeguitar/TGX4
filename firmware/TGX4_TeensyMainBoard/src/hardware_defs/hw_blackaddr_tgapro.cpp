#include "hw_blackaddr_tgapro.h"
#include "audioComponents.h" // (echo tap tempo)


// instantiate the class if the hw is defined in the platformio.ini file
#if defined (USE_BLACKADDR_TGAPRO)
	hw_Blackaddr_TGAPro hw;
#endif
	hw_Blackaddr_TGAPro*  hw_Blackaddr_TGAPro_ptr;

FLASHMEM bool hw_Blackaddr_TGAPro::init(void)
{	
	hw_Blackaddr_TGAPro_ptr = this;
	if (!codec.enable()) 	// codec init
	{
		DBG_SERIAL.println("Codec init error!");
		return false;
	}
	codec.inputSelect(AUDIO_INPUT_LINEIN);
	codec.inputLevel(0.77f); 

	pots[POT1].begin(A0, "POT1", true, 0.01f, POT_TAPER_LIN, 16);
	pots[POT2].begin(A1, "POT2", true, 0.01f, POT_TAPER_LIN, 16);
	pots[POT3].begin(A2, "POT3", true, 0.01f, POT_TAPER_LIN, 16);

	sw1.set_press_cb( []() { hw_Blackaddr_TGAPro_ptr->swPressHandler(presetSystem.getFlag(PRESET_FLAG_FT1_ASSIGN), 1);});
	sw2.set_press_cb( []() { hw_Blackaddr_TGAPro_ptr->swPressHandler(presetSystem.getFlag(PRESET_FLAG_FT2_ASSIGN), 2);});

	return true;
}


FLASHMEM bool hw_Blackaddr_TGAPro::process(void)
{
	uint16_t i;
	uint8_t assign = 0;
	float32_t value = 0.0f;
	for (i = 0; i<=POT3; i++)
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
			}
			if (assign && millis() > 1000) presetSystem.updateFXparam( (preset_param_t)(assign-1),  value);
		}
	}
	sw1.update();
	sw2.update();

	return false;
}	

FLASHMEM void hw_Blackaddr_TGAPro::led_set(uint8_t ledNo, hw_state_t state)
{
	tgx4Led* ledPtr = NULL;
	switch(ledNo)
	{
		case 0: ledPtr = &led1; break;
		case 1: ledPtr = &led2; break;
		default: ledPtr = NULL; break;
	}
	if (ledPtr)
	{
		switch(state)
		{
			case HW_STATE_OFF:	ledPtr->set(false); break;
			case HW_STATE_ON:	ledPtr->set(true); break;
			case HW_STATE_TOGGLE:  ledPtr->toggle(); break;
		}
	}
}


void hw_Blackaddr_TGAPro::swPressHandler(uint8_t assign, uint8_t swNo)
{
	if (assign >= PRESET_FTASSIGN_LAST) return;
	tgx4Led* led = swNo == 1 ? &led1 : &led2;
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
void hw_Blackaddr_TGAPro::swHoldHandler(uint8_t assign, uint8_t swNo)
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