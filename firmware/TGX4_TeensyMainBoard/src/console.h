#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <Arduino.h>

typedef enum
{
	CONSOLE_MODE_INFO,
	CONSOLE_MODE_CONSOLE
}console_mode_t;

typedef enum
{
	CONSOLE_COLOR_BLACK = 0,
	CONSOLE_COLOR_RED,
	CONSOLE_COLOR_GREEN,
	CONSOLE_COLOR_YELLOW,
	CONSOLE_COLOR_BLUE,
	CONSOLE_COLOR_MAGENTA,
	CONSOLE_COLOR_CYAN,
	CONSOLE_COLOR_WHITE,
	CONSOLE_COLOR_DEFAULT = 0xFF
}console_color_t;

void console_init();
void console_setMode(console_mode_t mode);
void console_tglMode();
void console_process();
void console_displayMsg(const char *msg, console_color_t color=CONSOLE_COLOR_DEFAULT);
void console_printComponents();
void console_printBin(uint64_t v, uint8_t num_places);
#endif // _CONSOLE_H_
