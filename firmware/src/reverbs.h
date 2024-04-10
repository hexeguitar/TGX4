#ifndef _REVERBS_H_
#define _REVERBS_H_
#include <arm_math.h>

//#define REVERBS_USE_TRAILS
// only one reverb is allowed to be active
typedef enum
{
	REVERB_PLATE  = 0,
	REVERB_SC	  = 1,
	REVERB_SPRING = 2
}active_reverb_e;

active_reverb_e reverbs_set(active_reverb_e model, bool bypass = false); // set new reverb algorithm and turn it on/off
active_reverb_e reverbs_getActive(); 									// get the active reverb algorithm
active_reverb_e reverbs_inc(); 											// increase reverb algorithm

const char* reverbs_getName();

int8_t reverb_getPitch(float32_t input); // convert 1.0 scale to pitch intervals

void reverb_setBypass(bool state);
bool reverb_getBypass();
bool reverb_tglBypass();
bool reverb_setFreeze(bool state);
bool reverb_tglFreeze();
bool reverb_getFreeze();

#endif // _REVERBS_H_
