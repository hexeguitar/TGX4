#include "tgx4_button.h"

using namespace TeensyTimerTool;

PeriodicTimer BtTimer;

// init the static variables
uint8_t tgx4Button::cnt = 0; // footswitch count.
uint8_t tgx4Button::press = 0;
uint8_t tgx4Button::press_long = 0;
uint8_t tgx4Button::state = 0;
uint8_t tgx4Button::latch = 0;
uint8_t tgx4Button::input_mask = 0;
uint32_t tgx4Button::long_delay = TGX4_BUTTONS_TRIG_DLY_SHORT;
bool tgx4Button::initDone = false;
uint8_t tgx4Button::pinTable[TGX4_BUTTONS_MAX_NOOF] = {0, 0, 0};
PeriodicTimer tgx4Button::timer = BtTimer;

tgx4Button::tgx4Button(const uint8_t pin)
{
	if (cnt >= TGX4_BUTTONS_MAX_NOOF)
		return; // do not allow more buttons than max
	pinTable[cnt] = pin;
	mask = 1 << cnt;
	input_mask |= mask;
	cnt++;
	press_cb = nullptr;
	pressLong_cb = nullptr;
}

void tgx4Button::init(void)
{
	pinMode(pinTable[idx], INPUT_PULLUP);
	if (!initDone)
	{
		timer.begin(timer_callback, 8ms);
		initDone = true;
	}
}

void tgx4Button::set_press_cb(void (*cbPtr)(void))
{
    press_cb = cbPtr;
}

void tgx4Button::set_pressLong_cb(void (*cbPtr)(void))
{
    pressLong_cb = cbPtr;
}


void tgx4Button::update(void)
{
	if (get_release() && press_cb)
		(*press_cb)();
	if (get_pressLong() && pressLong_cb)
		(*pressLong_cb)();
}

void tgx4Button::reset(void)
{
	press = 0;
	press_long = 0;
	state = 0;
	latch = 0;
}

uint8_t tgx4Button::get_state()
{
	return (state & mask);
}

uint8_t tgx4Button::get_pressShort()
{
	uint8_t result;
	noInterrupts();
	result = press & mask;
	interrupts();

	if (result)
	{
		latch |= mask;
		press &= ~mask;
	}

	return result;
}

uint8_t tgx4Button::get_release()
{
	uint8_t result;
	get_pressShort();
	result = (latch & mask) && !(state & mask);
	if (result)
		latch &= ~mask;
	return result;
}

uint8_t tgx4Button::get_pressLong()
{
	uint8_t result;
	noInterrupts();
	result = press_long & mask;
	interrupts();

	if (result)
	{
		press_long &= ~mask;
		press &= ~mask;
		latch &= ~mask;
	}
	return result;
}

void tgx4Button::set_longDly(uint32_t t_ms)
{
	t_ms = (t_ms + 4) / 8;
	if (t_ms < TGX4_BUTTONS_TRIG_DLY_MIN)
		t_ms = TGX4_BUTTONS_TRIG_DLY_MIN;
	long_delay = t_ms;
}

/**
 * @brief main debouncer callback invoked in timer ISR, static functions
 *
 */
void tgx4Button::timer_callback()
{
	static volatile uint8_t ct0, ct1, rpt, released;

	uint8_t i = 0, btn_in = 0;
	uint8_t longPressDelay = long_delay;

	uint8_t _state = state;
	uint8_t _press = press;
	uint8_t _press_long = press_long;

	i = 0;
	while (i < cnt)
	{
		btn_in += digitalReadFast(pinTable[i]) << i;
		i++;
	}

	i = _state ^ ((~btn_in) & input_mask);
	ct0 = ~(ct0 & i);
	ct1 = ct0 ^ (ct1 & i);
	i &= ct0 & ct1;
	_state ^= i;
	// now debouncing is finished
	_press |= _state & i;

	if (_state == 0) // long press buttons
	{
		rpt = longPressDelay;
		released = 1;
	}
	else
	{
		if (rpt)
			rpt--;
		if (rpt == 0 && released) // long press detected
		{
			_press_long |= _state;
			released = 0;
		}
	}

	state = _state;
	press = _press;
	press_long = _press_long;
}