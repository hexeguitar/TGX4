#ifndef _HW_PROTO_H_
#define _HW_PROTO_H_

#include <Arduino.h>

typedef enum 
{
	HW_STATE_OFF = 0,
	HW_STATE_ON,
	HW_STATE_TOGGLE
}hw_state_t;

extern uint8_t external_psram_size;

class hw_TGX4_prototype
{
public:
	virtual bool init(void) = 0;
	virtual bool process(void) = 0;
	virtual bool dry_set(hw_state_t state) = 0;
	virtual bool dry_get(void) = 0;
	virtual bool wet_set(hw_state_t state) = 0;
	virtual bool wet_get(void) = 0;
	virtual bool state_set(hw_state_t state) = 0;
	virtual bool state_get(void) = 0;
	virtual void led_set(uint8_t ledNo, hw_state_t state) = 0;
	bool phase_invert = false; 					// Output phase invert setting
private:
	
};

#endif // _HW_PROTO_H_
