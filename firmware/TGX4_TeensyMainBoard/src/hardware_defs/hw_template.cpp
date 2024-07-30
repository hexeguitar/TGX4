#include "hw_template.h"

// instantiate the class if it's defined in the platformio.ini file
#if defined (USE_TEMPLATE_HARDWARE_PLATFORM)
	hw_templateHardwarePlatorm hw;
#endif