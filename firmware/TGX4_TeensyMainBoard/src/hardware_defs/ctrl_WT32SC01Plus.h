/**
 * @file ctrl_WT32SC01Plus.h
 * @author Piotr Zapart www.hexefx.com
 * @brief 	GUI for the TGX4 project, using WT32SC01Plus Display + ESP32S3
 * 			3.5" 480x32 ST7796U, Capacitive touch FT6336U "Smart Panlee"
 * 			Communication with the display is using I2C + one interrrupt line
 * @version 0.1
 * @date 2024-07-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef _CTRL_WT32SC01PLUS_H_
#define _CTRL_WT32SC01PLUS_H_

#include <Arduino.h>
#include <Wire.h>
#include "presetSystem.h"
#include "ctrl_proto.h"

class Control_WT32SC01PLUS : public ctrl_TGX4_prototype
{
public:
	Control_WT32SC01PLUS(){};
	~Control_WT32SC01PLUS(){};
	void init(void)
	{
		init(&Wire1, 0x55, 22);
	}
	void init(TwoWire* i2cBus, uint8_t addr, uint8_t INtgpio);
	void process();
	// I2C slave interrupt handlers
	void i2cS_onReceive_handler(int numBytges);
	void i2cS_onRequest_handler();
	inline void signalChange()
	{
		digitalWrite(guiINT_pin, HIGH);
		delayMicroseconds(3);
		digitalWrite(guiINT_pin, LOW);
	}

private:
	TwoWire* bus;
	uint8_t guiINT_pin;
	uint32_t flagChangeMask = 0;
	uint64_t paramChangeMask = 0;
	uint8_t bytesReceived;

	/*	Max packet length is:
	 * 	- flag change mask = 4 bytes
	 *	- param change mask = 8 bytes
	 * 	- preset name = 16 bytes
	 *	- flags = sizeof(preset_flags_t) = x bytes
	 * 	- params PRESET_PARAM_LAST	
	 * 
	*/
	static const uint8_t i2cS_bf_size = sizeof(flagChangeMask) +\
										sizeof(paramChangeMask) +\
										member_size(preset_t, name) + 1 +\
										sizeof(preset_flags_t) +\
										PRESET_PARAM_LAST;
	uint8_t i2cS_addr;
	uint8_t i2cS_dataRequestLength = 0;
	uint8_t i2cS_data[i2cS_bf_size];
	// max value for the gui slider, all param values are sent a uint8_t
	const float32_t guiSliderMax = 100.0f; 
	
	typedef enum
	{
		GUI_CMD_NOP = 0,
		GUI_CMD_REQ_STATE,
		GUI_CMD_SET_FLAG,
		GUI_CMD_SET_FLAGS,
		GUI_CMD_SET_PARAM,
		GUI_CMD_SET_PARAMS,
		GUI_CMD_GET_PRESET,
		GUI_CMD_GET_FLAGS,
		GUI_CMD_GET_PARAMS,
		GUI_CMD_GET_PARAM,
		GUI_CMD_GET_PRESET_NAME,
		GUI_CMD_CHANGE_PRESET,
		GUI_CMD_SAVE_PRESET,
		GUI_CMD_PRESET_UP,
		GUI_CMD_PRESET_DOWN,
		GUI_CMD_PRESET_RELOAD
	}gui_cmd_t;
	gui_cmd_t cmd = GUI_CMD_NOP;

	typedef enum
	{
		GUI_STATE_IDLE,
		GUI_STATE_WAIT_4_CMD,
		GUI_STATE_CMD_RECEIVED,
		GUI_STATE_ERROR_NO_DATA,
		GUI_STATE_ERROR_WRONG_PARAM
	}gui_state_t;
	gui_state_t gui_state = GUI_STATE_WAIT_4_CMD;
	gui_state_t gui_last_error;
	void sendPresetData();
	void sendFlags();
	void updateFlags(uint8_t* flagsSrc, bool signal=true);
	void sendParams();
	void sendChangeMasks();
};

#if defined (USE_CONTROL_WT32SC01PLUS)
	extern Control_WT32SC01PLUS gui;
#endif

#endif // _CTRL_WT32SC01PLUS_H_
