#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

#define DBG_ENABLED 0

#define DBG_printf(fmt, ...) \
			do { if (DBG_ENABLED) DBG_SERIAL.printf(fmt, __VA_ARGS__); } while (0)

#define DBG_print(x) \
			do { if (DBG_ENABLED) DBG_SERIAL.print(x); } while (0)

#define DBG_println(x) \
			do { if (DBG_ENABLED) DBG_SERIAL.println(x); } while (0)

#endif // _DEBUG_LOG_H_
