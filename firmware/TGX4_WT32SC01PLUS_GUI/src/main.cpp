#include <Wire.h>
#include "display.h"
#include "gui.h"
#include "utils.h"
#include "cli.h"
#include "TGX4.h"

void setup()
{
	Serial.begin(115200);
	display_init();
	gui_init();
	cli_init();
	teensy.init();
}

void loop()
{
	gui_process();
	cli_process();	
	teensy.process();
}