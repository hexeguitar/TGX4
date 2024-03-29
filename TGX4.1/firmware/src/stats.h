#ifndef _STATS_H_
#define _STATS_H_

#include <Arduino.h>


typedef enum
{
	STATS_MODE_INFO,
	STATS_MODE_CONSOLE
}stats_mode_t;

typedef enum
{
	STATS_COLOR_BLACK = 0,
	STATS_COLOR_RED,
	STATS_COLOR_GREEN,
	STATS_COLOR_YELLOW,
	STATS_COLOR_BLUE,
	STATS_COLOR_MAGENTA,
	STATS_COLOR_CYAN,
	STATS_COLOR_WHITE,
	STATS_COLOR_DEFAULT = 0xFF
}stats_color_t;

void stats_init();
void stats_setMode(stats_mode_t mode);
void stats_tglMode();
void stats_process();
void stats_displayMsg(const char *msg, stats_color_t color=STATS_COLOR_DEFAULT);
void stats_printComponents();
#endif // _STATS_H_
