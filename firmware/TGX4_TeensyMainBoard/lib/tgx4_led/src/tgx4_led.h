#ifndef _TGX4_LED_H
#define _TGX4_LED_H

#include <Arduino.h>

#define LED_BLINK_INF (0xFF) // used as count, makes the led blink continuously 

typedef enum
{
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINK
}tgx4Led_state_t;

typedef enum
{
	LED_ACTIVE_HIGH,
	LED_ACTIVE_LOW
}tgx4Led_activeLevel_t;


class tgx4Led
{
public:
    tgx4Led(uint8_t gpio, tgx4Led_activeLevel_t act=LED_ACTIVE_LOW) : pin(gpio), ctrl_inv(act)
	{
		state = LED_STATE_OFF;
    	pinMode(pin, OUTPUT);
   		digitalWrite(pin, LOW^ctrl_inv);		
	}
    ~tgx4Led(){};
    void set(bool s) 
	{
		if (s) 	{ state =  LED_STATE_ON; digitalWriteFast(pin, HIGH^ctrl_inv);}
		else 	{ state =  LED_STATE_OFF; digitalWriteFast(pin, LOW^ctrl_inv);}
	}
	void pulse(uint16_t period_ms, uint8_t count)
	{
		blink_period_ms = period_ms;
		blinks = count;
		t_last = millis();
		state_last = state;
		state = LED_STATE_BLINK;
	}

    void process()
	{
		if (state == LED_STATE_BLINK)
		{
			uint32_t t_now = millis();
			if (t_now - t_last > blink_period_ms)
			{
				digitalToggleFast(pin);
				if (blinks != LED_BLINK_INF)
				{
					if (blinks) blinks--;
					else 		set( state_last == LED_STATE_ON ? true : false);
				}
				t_last = t_now;
			}
		}
	}

private:
    const uint8_t pin;
    const uint8_t ctrl_inv;
	tgx4Led_state_t state;
	tgx4Led_state_t state_last;
    uint32_t t_last;
    uint16_t blink_period_ms;
    uint8_t blinks;
};

#endif // _LEDS_H
