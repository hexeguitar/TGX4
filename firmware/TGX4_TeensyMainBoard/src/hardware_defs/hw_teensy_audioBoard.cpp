#include "hw_teensy_audioBoard.h"

// instantiate the class if the hw is defined in the platformio.ini file
#if defined (USE_TEENSY_AUDIO_BOARD)
	hw_Teensy_AudioBoard hw;
#endif