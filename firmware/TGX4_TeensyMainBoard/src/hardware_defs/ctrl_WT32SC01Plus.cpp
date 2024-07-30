#include "ctrl_WT32SC01Plus.h"
#include "debug_log.h"
// #include "hw_proto.h"
#include "console.h"
#include "presetSystem.h"

static void i2cS_onReceive(int numBytes);
static void i2cS_onRequest();

#if defined (USE_CONTROL_WT32SC01PLUS)
	Control_WT32SC01PLUS gui;
#endif

Control_WT32SC01PLUS* selfPtr = NULL;

FLASHMEM void Control_WT32SC01PLUS::init(TwoWire* i2cBus, uint8_t addr, uint8_t INTgpio)
{
	selfPtr = this;
	bus = i2cBus;
	i2cS_addr = addr;
	guiINT_pin = INTgpio;

	pinMode(guiINT_pin, OUTPUT);
	digitalWrite(guiINT_pin, LOW);
	bus->begin(i2cS_addr);
	bus->onReceive(i2cS_onReceive);
	bus->onRequest(i2cS_onRequest);
	delay(200);
	gui_state = GUI_STATE_IDLE;
	presetSystem.triggerFullGuiUpdate();
}

void Control_WT32SC01PLUS::process(void)
{
	float32_t tmpf;
	uint32_t t_now = millis();
	static uint32_t t_last = 0;
	char* u8ptr;
	switch(gui_state)
	{
		case GUI_STATE_IDLE: 
			if (t_now - t_last > 100)
			{
				flagChangeMask = presetSystem.getFlagsChangeMask();
				paramChangeMask = presetSystem.getParamsChangeMask();
				if ((flagChangeMask || paramChangeMask))
				{
					gui_state = GUI_STATE_WAIT_4_CMD;
					signalChange();
					presetSystem.resetChangeMask();
				}
				t_last = t_now;
			}
			break;
		case GUI_STATE_WAIT_4_CMD: break;
		case GUI_STATE_CMD_RECEIVED:
			cmd = (gui_cmd_t)i2cS_data[0];
			switch (cmd)
			{
				case GUI_CMD_NOP: break;
				case GUI_CMD_REQ_STATE:
					sendChangeMasks();
					gui_state = GUI_STATE_IDLE;
					break;
				case GUI_CMD_SET_FLAG: // ----------------------------------------------------------------
					DBG_println("cmd set flag");
					// Cmd (1 byte) + flag number (1 byte) + flag value (1 byte)
					if(bytesReceived != 3) 
					{
						gui_state = GUI_STATE_ERROR_NO_DATA;
						break;
					}
					if (i2cS_data[1] < PRESET_FLAG_LAST) // [1] holds the flag index [2] flag value
					{
						presetSystem.updateFlag((preset_paramFlag_t)i2cS_data[1], i2cS_data[2], false); // no need to signal back to the ESP
						//DBG_printf("Flag %d updated with value %d\r\n", i2cS_data[1], i2cS_data[2]);
					}
					i2cS_data[0] = i2cS_addr;
					i2cS_dataRequestLength = 1;					
					break;
				case GUI_CMD_SET_FLAGS: // ----------------------------------------------------------------
					DBG_println("cmd set flags");
					// Cmd (1 byte) + all flags
					if(bytesReceived != 1 + sizeof(preset_flags_t)) 
					{
						gui_state = GUI_STATE_ERROR_NO_DATA;
						break;
					}
					updateFlags(&i2cS_data[1], false);
					i2cS_data[0] = i2cS_addr;
					i2cS_dataRequestLength = 1;
					gui_state = GUI_STATE_IDLE;
					break;					
				case GUI_CMD_SET_PARAM:// ----------------------------------------------------------------
					DBG_println("cmd set param");
					// cmd + param number + param value = 3 bytes
					if(bytesReceived != 3) 
					{
						gui_state = GUI_STATE_ERROR_NO_DATA;
						break;
					}
					if (i2cS_data[1] < PRESET_PARAM_LAST)
					{
						tmpf = (float32_t)i2cS_data[2] / guiSliderMax;
						presetSystem.updateFXparam((preset_param_t)i2cS_data[1], tmpf, false);
					}
					i2cS_data[0] = i2cS_addr;
					i2cS_dataRequestLength = 1;
					gui_state = GUI_STATE_IDLE;
					break;
				case GUI_CMD_SET_PARAMS:	// ----------------------------------------------------------------
					DBG_println("cmd set all params");

					i2cS_data[0] = i2cS_addr;
					i2cS_dataRequestLength = 1;
					gui_state = GUI_STATE_IDLE;
					break;					
				case GUI_CMD_CHANGE_PRESET:
					DBG_println("cmd change preset");
					// cmd + preset num (7bit)
					if(bytesReceived != 2) 
					{
						gui_state = GUI_STATE_ERROR_NO_DATA;
						break;
					}
					if (presetSystem.getSDstatus()) // all 128 presets available
					{
						presetSystem.load(i2cS_data[1] & 0x7F);
					}
					else 				// only 8 in EEPROM
					{
						presetSystem.load(i2cS_data[1] & 0x07);
					}
					DBG_printf("Preset changed to %d\r\n", i2cS_data[1]);
					sendPresetData();
					break;
				case GUI_CMD_SAVE_PRESET: // ----------------------------------------------------------------
					DBG_println("cmd save preset");	
					// ESP send a new name for the preset upon saving
					if(bytesReceived != 1 + member_size(preset_t, name)) 
					{
						gui_state = GUI_STATE_ERROR_NO_DATA;
						break;
					}	
					presetSystem.setName((char*)&i2cS_data[1]);
					presetSystem.save();
					i2cS_data[0] = i2cS_addr;
					i2cS_dataRequestLength = 1;
					gui_state = GUI_STATE_IDLE;
					DBG_printf("Preset %d saved: %s\r\n", 
							presetSystem.getCurrentIdx(), presetSystem.getName());
					break;
				case GUI_CMD_PRESET_RELOAD:
					DBG_println("cmd preset reload");
					presetSystem.load(presetSystem.getCurrentIdx()); // restore the preset settings
					sendPresetData();
					gui_state = GUI_STATE_IDLE;
					break;
				case GUI_CMD_GET_PRESET: // ----------------------------------------------------------------
					// reply: preset data
					DBG_println("cmd get preset");
					sendPresetData();
					gui_state = GUI_STATE_IDLE;
					break;
				case GUI_CMD_GET_PRESET_NAME:
					DBG_println("cmd get preset name");
					// cmd + presetNo, reply = 16byte string
					if(bytesReceived != 2) 
					{
						gui_state = GUI_STATE_ERROR_NO_DATA;
						break;
					}	
					u8ptr = presetSystem.getName(i2cS_data[1]);
					memcpy(i2cS_data, u8ptr, member_size(preset_t, name));
					i2cS_dataRequestLength = member_size(preset_t, name);
					signalChange();
					break;	
				case GUI_CMD_GET_FLAGS:
					DBG_println("cmd get all flags");
					sendFlags();
					gui_state = GUI_STATE_IDLE;				
					break;
				case GUI_CMD_GET_PARAMS:
					DBG_println("cmd get params");
					sendParams();
					gui_state = GUI_STATE_IDLE;					
					break;
				case GUI_CMD_GET_PARAM: // ----------------------------------------------------------------
					DBG_println("cmd get param");
					// cmd + param index, reply = 1 byte (float scaled to uint8_t)
					if(bytesReceived != 2) 
					{
						gui_state = GUI_STATE_ERROR_NO_DATA;
						break;
					}
					if(i2cS_data[1] < PRESET_PARAM_LAST)
					{
						i2cS_data[0] = (uint8_t)(presetSystem.getFXparam((preset_param_t)i2cS_data[1]) * guiSliderMax);
						i2cS_dataRequestLength = 1;
						signalChange();						
						gui_state = GUI_STATE_IDLE;
					}
					else
					{
						gui_state = GUI_STATE_ERROR_WRONG_PARAM;
					}					
					break;
				case GUI_CMD_PRESET_UP: // ----------------------------------------------------------------
					DBG_println("cmd preset up");
					presetSystem.load_next();
					sendPresetData();
					gui_state = GUI_STATE_IDLE;
					break;
				case GUI_CMD_PRESET_DOWN: // ----------------------------------------------------------------
					DBG_println("cmd preset down");
					presetSystem.load_previous();
					sendPresetData();
					gui_state = GUI_STATE_IDLE;		
					break;
			}
			cmd = GUI_CMD_NOP;
			break;
		case GUI_STATE_ERROR_NO_DATA:
			DBG_println("Gui command error! None or too few data bytes received.");
			gui_last_error = gui_state;
			gui_state = GUI_STATE_IDLE;
			break;
		case GUI_STATE_ERROR_WRONG_PARAM:
			DBG_println("Gui command error! Wrong param. ");
			gui_last_error = gui_state;
			gui_state = GUI_STATE_IDLE;
			break;	
		default: break;
	} 
}
void Control_WT32SC01PLUS::sendChangeMasks()
{
	uint8_t* dataDst = &i2cS_data[0];

	memcpy(dataDst, (uint8_t *)&flagChangeMask, sizeof(flagChangeMask));
	dataDst += sizeof(flagChangeMask);
	memcpy(dataDst, (uint8_t *)&paramChangeMask, sizeof(paramChangeMask));
	i2cS_dataRequestLength = sizeof(flagChangeMask) + sizeof(paramChangeMask);
	signalChange();
	flagChangeMask = 0;
	paramChangeMask = 0;
}

void Control_WT32SC01PLUS::sendPresetData()
{
	// Teensy needs to send all the preset values back
	// prepare the data packet
	uint8_t* dataDst = &i2cS_data[0];
	uint8_t len = member_size(preset_t, name);
	memcpy(dataDst, (uint8_t*)presetSystem.getName(), len);
	dataDst += len;
	memcpy(dataDst, presetSystem.getFlagsPtr(), sizeof(preset_flags_t));
	dataDst += sizeof(preset_flags_t);
	// all fx parameters are normalized to 0.0f ... 1.0f range
	for (uint8_t i=0; i<PRESET_PARAM_LAST; i++)
	{
		*dataDst++ = (uint8_t)(presetSystem.getFXparam((preset_param_t)i) * guiSliderMax);
	}
	i2cS_dataRequestLength = presetSystem.length_paramsU8;
	signalChange();
	//DBG_printf("Preset %d data sent\r\n", presetSystem.getFlag(PRESET_FLAG_IDX));
}

void Control_WT32SC01PLUS::sendFlags()
{
	// prepare the data packet
	uint8_t* dataDst = &i2cS_data[0];
	memcpy(dataDst, presetSystem.getFlagsPtr(), sizeof(preset_flags_t));;
	i2cS_dataRequestLength = sizeof(preset_flags_t);
	signalChange();
}

void Control_WT32SC01PLUS::updateFlags(uint8_t* flagsSrc, bool signal)
{
	preset_flags_t flagsTmp;
	memcpy((uint8_t *)&flagsTmp, flagsSrc, sizeof(preset_flags_t));
	uint8_t flagOld, flagNew;
	for (uint8_t i=0; i<PRESET_FLAG_LAST; i++)
	{
		flagOld = presetSystem.getFlag((preset_paramFlag_t)i);
		flagNew = presetSystem.getFlag((preset_paramFlag_t)i, &flagsTmp);
		// updating flags trigger the callbacks, update only the flags with changed value
		if (flagNew != flagOld) 
		{
			presetSystem.updateFlag((preset_paramFlag_t)i, flagNew, signal);
			//DBG_printf("updated flag: %d with value %d\r\n",i, flagNew);
		}
	}
}

void Control_WT32SC01PLUS::sendParams()
{
	for (uint8_t i=0; i<PRESET_PARAM_LAST; i++)
	{
		i2cS_data[i] = (uint8_t)(presetSystem.getFXparam((preset_param_t)i) * guiSliderMax);
	}
	i2cS_dataRequestLength = PRESET_PARAM_LAST;
	signalChange();	
}



void Control_WT32SC01PLUS::i2cS_onReceive_handler(int numBytes)
{
	
	uint8_t *dataPtr = &i2cS_data[0];
	if (numBytes >= i2cS_bf_size || gui_state == GUI_STATE_CMD_RECEIVED)
	{
		bytesReceived = 0;
		bus->flush();
		gui_state = GUI_STATE_IDLE;	
	}
	else 
	{
		bytesReceived = numBytes;
		while(bus->available())
		{
			*dataPtr++ = bus->read();
		}
		gui_state = GUI_STATE_CMD_RECEIVED;
	}
}

void Control_WT32SC01PLUS::i2cS_onRequest_handler()
{
	for (uint8_t i = 0; i< i2cS_dataRequestLength; i++)
	{
		bus->write(*(&i2cS_data[0]+i));
	}
	gui_state = GUI_STATE_IDLE;
}

void i2cS_onReceive(int numBytes)
{
	selfPtr->i2cS_onReceive_handler(numBytes);
}

void i2cS_onRequest()
{
	selfPtr->i2cS_onRequest_handler();
}
