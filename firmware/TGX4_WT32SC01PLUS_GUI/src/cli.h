#ifndef _CLI_H_
#define _CLI_H_

#include <Arduino.h>

typedef enum
{
	CLI_COLOR_BLACK = 0,
	CLI_COLOR_RED,
	CLI_COLOR_GREEN,
	CLI_COLOR_YELLOW,
	CLI_COLOR_BLUE,
	CLI_COLOR_MAGENTA,
	CLI_COLOR_CYAN,
	CLI_COLOR_WHITE,
	CLI_COLOR_DEFAULT = 0xFF
}cli_color_t;

void cli_init();
void cli_process();


inline void cli_reset_color() 
{
    Serial.print(F("\x1b[0m"));
}
inline void cli_reset_color();

inline void cli_set_color(uint8_t color) 
{
    Serial.print(F("\x1b["));
    Serial.print(30 + color);
    Serial.print("m");
}
inline void cli_set_color(uint8_t color);


#endif // _CLI_H_
