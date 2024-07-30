#ifndef _TGX4_BUTTON_H_
#define _TGX4_BUTTON_H_

#include <Arduino.h>
#include "TeensyTimerTool.h"

#define TGX4_BUTTONS_MAX_NOOF				(3) // max 3 footswitches

#define TGX4_BUTTONS_TRIG_DLY_MIN		    (40)
#define TGX4_BUTTONS_TRIG_DLY_SHORT			(320)
#define TGX4_BUTTONS_TRIG_DLY_LONG			(2400)

class tgx4Button : public TeensyTimerTool::PeriodicTimer
{
public:
    tgx4Button(const uint8_t pin);
    void init(void);
    static void reset(void);
    uint8_t get_state();
    uint8_t get_pressShort();
    uint8_t get_release();
    uint8_t get_pressLong();
    void update();
    static void set_longDly(uint32_t t_ms);
    void set_press_cb(void (*cbPressPtr)(void));
    void set_pressLong_cb(void (*cbPressLongPtr)(void));

private:
    static bool initDone; 
    static uint8_t pinTable[TGX4_BUTTONS_MAX_NOOF];
    static uint8_t cnt;
    uint8_t idx;
    uint8_t mask;
    static uint8_t input_mask;
	static uint8_t press;
	static uint8_t press_long;
	static uint8_t state;
    static uint8_t latch;
	static uint32_t long_delay;
    static PeriodicTimer timer;
    static void timer_callback(void);
    void (*press_cb)(void);
    void (*pressLong_cb)(void);
};


#endif // _TGX4_BUTTONS_H_
