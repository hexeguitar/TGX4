#ifndef _HARDWARE_DEFS_H_
#define _HARDWARE_DEFS_H_

// list of all avaialble hardware platforms

#include "hw_hexefx_t40gfx.h"
#include "hw_hexefx_t41gfx.h"
#include "hw_teensy_audioBoard.h"
#include "hw_blackaddr_tgapro.h"
#include "hw_t40_ES8388.h"

// add yours here:
// #include "hw_mycustomplatform.h"

// The USE_xxxxx flags creating the hw class are defined in the platformio.ini file
// by choosing the build environment.

#if defined(USE_HEXEFX_T41GFX)
	hw_hexefx_t41GFX hw;
#elif defined(USE_HEXEFX_T40GFX)
	hw_hexefx_t40GFX hw;
#elif defined(USE_TEENSY_AUDIO_BOARD)
	hw_Teensy_AudioBoard hw;
#elif defined(USE_BLACKADDR_TGAPRO)
	hw_Blackaddr_TGAPro hw;
#elif defined(USE_ES8388)
	hw_t40_ES8388 hw;	
#elif defined(USE_YOUR_CUSTOM_HARDWARE_PLATFORM)
	hw_YourCustomHardwarePlatform hw;			
#endif

#endif // _HARDWARE_DEFS_H_
