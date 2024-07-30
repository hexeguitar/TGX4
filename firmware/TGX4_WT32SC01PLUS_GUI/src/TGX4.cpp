#include "TGX4.h"
#include "pinCfg.h"
#include "gui.h"
#include "utils.h"
#include "debug_log.h"

#define TEENSY_REPORT_SIZE (4+8)	// flagsMask u32 + paramMask u64

TGX4_ctrl teensy = TGX4_ctrl(TEENSY_ADDR, I2C_SDA_GPIO, I2C_SCL_GPIO, GUI_INT_GPIO);

/**
 * @brief Initialize TGX4 Teensy controller
 */
void TGX4_ctrl::init()
{
	Wire.setBufferSize(256);
	Wire.begin(_sda, _scl, 400000U);
	pinMode(_intPin, INPUT_PULLDOWN);
	attachInterrupt(_intPin, gui_int_cb, FALLING);
	state = TEENSY_STATE_BOOT;
	delay(10); // give teensy time to boot up
}

/**
 * @brief write command + optional data
 * 
 * @param command 
 * @param dataSrc 
 * @param size 
 * @return uint8_t 
 */
uint8_t TGX4_ctrl::write(gui_cmd_t command, uint8_t* dataSrc, uint16_t size)
{
	uint8_t result;
	cmd = command;
	DBG_printf("Write cmd: %d\r\n", (uint8_t)cmd);
	Wire.beginTransmission(_addr);
	Wire.write((uint8_t)command);
	if (size == 0 || !dataSrc) 
	{
		result = Wire.endTransmission();
		if (result == 0)
		{
			state = TEENSY_STATE_CMD_SENT;
			timer = millis();
		}
		else state = TEENSY_STATE_IDLE;
		return result;
	}	
	Wire.write(dataSrc, size);
	result = Wire.endTransmission();
	if (result == 0)
	{
		state = TEENSY_STATE_CMD_SENT;
		timer = millis();	
	}
	else state = TEENSY_STATE_IDLE;
	return result;
}

/**
 * @brief Read n bytes and store them in the provided buffer
 * 
 * @param dst destination buffer
 * @param size how many bytes to request
 * @return bool  succes (true) or fail (false)
 */
bool TGX4_ctrl::read(uint8_t* dst, uint8_t size)
{
	uint8_t received = 0;
	received = Wire.requestFrom(_addr, size);
	if (received != size) 
	{
		DBG_printf("Teensy read error, requested %d, received %d bytes \r\n", size, received);
		Wire.flush();
		cmd = GUI_CMD_NOP;
		return false;
	}
	while(received)
	{
		*dst++ = Wire.read();
		received--;
	}
	return true;
}

void gui_int_cb(void)
{
	teensy.triggerUpdate();
}

void TGX4_ctrl::process()
{
	uint32_t tNow;
	static uint32_t tLast;
	uint8_t paramNo;
	uint8_t paramVal;
	uint16_t i;
	char msgBf[64];

	switch(state)
	{
		case TEENSY_STATE_BOOT:
			if (updateRequestFlag) 
			{
				updateRequestFlag = false;
				write(GUI_CMD_GET_PRESET, NULL, 0);
			} else
			{	// if Teensy did not signal the readiness, try requesting preset data after 1sec delay
				tNow = millis();
				if (tNow > 1000) write(GUI_CMD_GET_PRESET, NULL, 0);
			}
			break;
		case TEENSY_STATE_IDLE: 
			tNow = millis();
			if (tNow - tLast > 100)
			{
				if (flagsChangeMask_tx)
				{
					write(GUI_CMD_SET_FLAGS, (uint8_t*)&currentPreset.flags, sizeof(currentPreset.flags));
					flagsChangeMask_tx = 0;
					state = TEENSY_STATE_IDLE;	
				}
				if (paramChangeMask_tx)
				{
					DBG_printf("param mask = %016llX\r\n", paramChangeMask_tx);
					for (i=0; i<PRESET_PARAM_LAST; i++)
					{
						if (paramChangeMask_tx & (1ull<<i))
						{
							dataBf[0] = i;
							dataBf[1] = currentPreset.fxParams[i];
							write(GUI_CMD_SET_PARAM, dataBf, 2);
						}
					}
					paramChangeMask_tx = 0;
				}
				tLast = tNow;
			}
			if (updateRequestFlag) // request came from Teensy - ask for report
			{
				updateRequestFlag = false;
				write(GUI_CMD_REQ_STATE, NULL, 0);
			}
			break;
		case TEENSY_STATE_CMD_SENT:
			if (updateRequestFlag)
			{
				updateRequestFlag = false;
				switch(cmd)
				{
					// if the Teensy signals there has been a change made locally (via pot etc)
					// ESP asks for change mask of flags and params
					case GUI_CMD_REQ_STATE:
						if (read(dataBf, TEENSY_REPORT_SIZE))
						{
							memcpy((uint8_t*)&flagsChangeMask_rx, (uint8_t*)&dataBf[0], sizeof(flagsChangeMask_rx));
							memcpy((uint8_t*)&paramChangeMask_rx, (uint8_t*)&dataBf[sizeof(flagsChangeMask_rx)], sizeof(paramChangeMask_rx));
							processChangeReport();
						}
						break;
					// Teensy returns the full preset dataBf (name, flags, params)
					case GUI_CMD_PRESET_UP:
					case GUI_CMD_PRESET_DOWN:
					case GUI_CMD_PRESET_RELOAD:
					case GUI_CMD_GET_PRESET:
					case GUI_CMD_CHANGE_PRESET:
						DBG_println("full preset data req");
						read(dataBf, sizeof(preset_t));
						populatePreset();
						gui_populatePreset(&currentPreset);
						cmd = GUI_CMD_NOP;
						state = TEENSY_STATE_IDLE;						
						break;						
					case GUI_CMD_NOP:
						break;
					case GUI_CMD_SET_FLAG:
					case GUI_CMD_SET_FLAGS:
					case GUI_CMD_SET_PARAM:
					case GUI_CMD_SET_PARAMS:
					case GUI_CMD_SAVE_PRESET:
						read(dataBf, 1);
						if (dataBf[0] != _addr) DBG_printf("Cmd %d No ACK!\r\n", cmd);
						cmd = GUI_CMD_NOP;
						state = TEENSY_STATE_IDLE;
						break;
					case GUI_CMD_GET_FLAGS:
						DBG_println("Requesting flags");
						read(dataBf, sizeof(preset_flags_t));
						memcpy((uint8_t*)&currentPreset.flags, (uint8_t*)&dataBf[0], sizeof(preset_flags_t));
						processFlags();
						// resetting the cmd+state is handled in the processFlags
						// (might need to send more commands)
						break;
					case GUI_CMD_GET_PARAM:
						DBG_println("Requesting param");
						read(dataBf, 2); // 2 bytes, param number + value
						setParam((preset_param_t)dataBf[0], dataBf[1], false);
						gui_updateParam((preset_param_t)i, getParam((preset_param_t)i));
						state = TEENSY_STATE_IDLE;
						cmd = GUI_CMD_NOP;
						break;
					case GUI_CMD_GET_PARAMS:
						DBG_println("Requesting params");
						read(dataBf, PRESET_PARAM_LAST);
						for (i=0; i<PRESET_PARAM_LAST; i++)
						{
							setParam((preset_param_t)i, dataBf[i], false);
						}						
						processParams();
						cmd = GUI_CMD_NOP;
						state = TEENSY_STATE_IDLE;						
						break;
					case GUI_CMD_GET_PRESET_NAME:
						read(dataBf, member_size(preset_t, name));
						DBG_printf("PRELOAD PRESET %d\r\n%s\r\n", currentPreset.flags.newPresetNo+1, dataBf);
						snprintf(msgBf, sizeof(msgBf), "LOAD PRESET %d\n%s\nhold to activate", currentPreset.flags.newPresetNo+1, dataBf);
						gui_show_popup(msgBf, 2000);
						state = TEENSY_STATE_IDLE;
						cmd = GUI_CMD_NOP;
						break;
				}
			}
			else
			{
				tNow = millis();
				if (tNow - timer > 100) // timeout 100ms to receive the data
				{
					updateRequestFlag = false;
					state = TEENSY_STATE_IDLE;
					Wire.flush();
					DBG_println("I2C Timeout, going idle");
				}
			}
		break;
		
	}
}

void TGX4_ctrl::handle_i2cError(i2c_error_t err)
{
	uint32_t tNow;
	switch (err)
	{
		case I2C_ERR_TIMEOUT:
			tNow = millis();
			if (tNow - timer > 100) // timeout 100ms to receive the data
			{
				state = TEENSY_STATE_IDLE;
				Wire.flush();
				updateRequestFlag = false;
				DBG_println("Cmd sent, reply Timeout, going idle");
			}
			break;
		case I2C_ERR_NO_DATA_RECEIVED:
			
			break;
	
		default:
			break;
	}

}

/**
 * @brief flags and param change masks are available, process them
 * 			to update parameters within the GUI
 * 
 */
void TGX4_ctrl::processChangeReport()
{
	uint8_t i = 0;
	uint8_t updateType = (flagsChangeMask_rx!=0) | ((paramChangeMask_rx!=0)<<1);

	switch (updateType)
	{
		case 0:	
			cmd = GUI_CMD_NOP; 
			state = TEENSY_STATE_IDLE;
			break;
		case 1:	// flags only
			write(GUI_CMD_GET_FLAGS, NULL, 0);
			break;
		case 2:	// params only
			write(GUI_CMD_GET_PARAMS, NULL, 0);
			break;
		case 3: // flags + params need update
			DBG_println("Flags+params update");
			write(GUI_CMD_GET_PRESET, NULL, 0);	
			break;
	}
}

void TGX4_ctrl::processFlags()
{
	uint16_t i;
	for (i=0; i<PRESET_FLAG_LAST; i++)
	{
		if (flagsChangeMask_rx & (1<<i))
		{
			gui_updateFlag((preset_paramFlag_t)i, getFlag((preset_paramFlag_t)i));
		}
	}
	// handle extra flags (not preset related)
	dataBf[0] = currentPreset.flags.newPresetNo;
	if (dataBf[0] != currentPreset.flags.idx)
	{
		DBG_printf("Request name for preset %d\r\n", dataBf[0]);
		write(GUI_CMD_GET_PRESET_NAME, &dataBf[0], 1);
	}
	else 
	{
		cmd = GUI_CMD_NOP;
		state = TEENSY_STATE_IDLE;
	}
}

void TGX4_ctrl::processParams()
{
	uint16_t i;
	for (i=0; i<PRESET_PARAM_LAST; i++)
	{
		if (paramChangeMask_rx & (1ull<<i))
		{
			gui_updateParam((preset_param_t)i, getParam((preset_param_t)i));
		}
	}	
}


uint8_t TGX4_ctrl::getFlag(preset_paramFlag_t flag)
{
	uint8_t result = 0;
	switch(flag)
	{
		case PRESET_FLAG_BYPASS:
			result = currentPreset.flags.bypass;			break;		
		case PRESET_FLAG_INSELECT:
			result = currentPreset.flags.inSelect; 			break;
		case PRESET_FLAG_COMP_EN:
			result = currentPreset.flags.compressorEn;		break;
		case PRESET_FLAG_WAH_EN:
			result = currentPreset.flags.wahEn;				break;
		case PRESET_FLAG_WAH_MODEL:
			result = currentPreset.flags.wahModel;			break;				
		case PRESET_FLAG_BOOST_EN:
			result = currentPreset.flags.boostEn;			break;
		case PRESET_FLAG_OCTAVE_EN:
			result = currentPreset.flags.octaveEn;			break;
		case PRESET_FLAG_AMPMODEL_NO:
			result = currentPreset.flags.ampModelNo;		break;
		case PRESET_FLAG_DOUBLER_EN:
			result = currentPreset.flags.doublerEn;			break;
		case PRESET_FLAG_CABIR_NO:
			result = currentPreset.flags.cabIrNo;			break;
		case PRESET_FLAG_DELAY_EN:
			result = currentPreset.flags.delayEn;			break;
		case PRESET_FLAG_REVBTYPE:
			result = currentPreset.flags.revebType;			break;
		case PRESET_FLAG_REVB_EN:
			result = currentPreset.flags.reverbEn;			break;
		case PRESET_FLAG_POT1_ASSIGN:
			result = currentPreset.flags.pot1Assign;		break;
		case PRESET_FLAG_POT2_ASSIGN:
			result = currentPreset.flags.pot2Assign;		break;
		case PRESET_FLAG_POT3_ASSIGN:
			result = currentPreset.flags.pot3Assign;		break;
		case PRESET_FLAG_EXP_ASSIGN:
			result = currentPreset.flags.expAssign;			break;
		case PRESET_FLAG_FT1_ASSIGN:
			result = currentPreset.flags.ft1Assign;			break;
		case PRESET_FLAG_FT2_ASSIGN:
			result = currentPreset.flags.ft2Assign;			break;
		case PRESET_FLAG_DRYSIG:
			result = currentPreset.flags.drySigEn;			break;
		case PRESET_FLAG_IDX:
			result = currentPreset.flags.idx;				break;
		case PRESET_FLAG_REVB_FREEZE_EN:
			result = currentPreset.flags.reverbFreezeEn;	break;
		case PRESET_FLAG_DELAY_FREEZE_EN:
			result = currentPreset.flags.delayFreezeEn;		break;	
		default: break;	
	}
	return result;
}

void TGX4_ctrl::setFlag(preset_paramFlag_t flag, uint8_t value, bool signal)
{
	if (signal) flagsChangeMask_tx |= 1<<((uint8_t)flag);
	switch(flag)
	{
		case PRESET_FLAG_BYPASS:
			currentPreset.flags.bypass = value & 0x01;			break;		
		case PRESET_FLAG_INSELECT:
			currentPreset.flags.inSelect = value & 0x03;		break;
		case PRESET_FLAG_COMP_EN:
			currentPreset.flags.compressorEn = value & 0x01;	break;
		case PRESET_FLAG_WAH_EN:
			currentPreset.flags.wahEn = value & 0x01;			break;
		case PRESET_FLAG_WAH_MODEL:
			currentPreset.flags.wahModel = value & 0x07;		break;				
		case PRESET_FLAG_BOOST_EN:
			currentPreset.flags.boostEn = value & 0x01;			break;
		case PRESET_FLAG_OCTAVE_EN:
		currentPreset.flags.octaveEn = value & 0x01;			break;
		case PRESET_FLAG_AMPMODEL_NO:
			currentPreset.flags.ampModelNo = value & 0x0F;		break;
		case PRESET_FLAG_DOUBLER_EN:
			currentPreset.flags.doublerEn = value & 0x01;		break;
		case PRESET_FLAG_CABIR_NO:
			currentPreset.flags.cabIrNo = value & 0x0F;			break;
		case PRESET_FLAG_DELAY_EN:
			currentPreset.flags.delayEn = value & 0x01;			break;
		case PRESET_FLAG_REVBTYPE:
			currentPreset.flags.revebType = value & 0x03;		break;
		case PRESET_FLAG_REVB_EN:
			currentPreset.flags.reverbEn = value & 0x01;		break;
		case PRESET_FLAG_POT1_ASSIGN:
			currentPreset.flags.pot1Assign = value & 0x3F;		break;
		case PRESET_FLAG_POT2_ASSIGN:
			currentPreset.flags.pot2Assign = value & 0x3F;		break;
		case PRESET_FLAG_POT3_ASSIGN:
			currentPreset.flags.pot3Assign = value & 0x3F;		break;
		case PRESET_FLAG_EXP_ASSIGN:
			currentPreset.flags.expAssign = value & 0x3F;		break;
		case PRESET_FLAG_FT1_ASSIGN:
			currentPreset.flags.ft1Assign = value & 0x0F;		break;
		case PRESET_FLAG_FT2_ASSIGN:
			currentPreset.flags.ft2Assign = value & 0x0F;		break;					
		case PRESET_FLAG_DRYSIG:
			currentPreset.flags.drySigEn = value & 0x01;		break;
		case PRESET_FLAG_IDX:
			currentPreset.flags.idx = value & 0x7F;				break;		
		case PRESET_FLAG_DELAY_FREEZE_EN:
			currentPreset.flags.delayFreezeEn = value & 0x01; 	break;
		case PRESET_FLAG_REVB_FREEZE_EN:
			currentPreset.flags.reverbFreezeEn = value & 0x01; 	break;
		default: break;	
	}
}



uint8_t TGX4_ctrl::getParam(preset_param_t param)
{
	return (currentPreset.fxParams[param]);
}

void TGX4_ctrl::setParam(preset_param_t param, uint8_t value, bool signal)
{
	if (param >= PRESET_PARAM_LAST) return;
	currentPreset.fxParams[param] = value;
	if (signal) 
	{
		paramChangeMask_tx |= 1ull<<((uint8_t)param);
	}
}

void TGX4_ctrl::printFlags()
{
	DBG_SERIAL.printf("Bypass: %02x\r\n", currentPreset.flags.bypass);
	DBG_SERIAL.printf("inSelect: %02x\r\n", currentPreset.flags.inSelect);
	DBG_SERIAL.printf("comp: %02x\r\n", currentPreset.flags.compressorEn);
	DBG_SERIAL.printf("wah: %02x\r\n", currentPreset.flags.wahEn);
	DBG_SERIAL.printf("wah model: %02x\r\n", currentPreset.flags.wahModel);
	DBG_SERIAL.printf("boost: %02x\r\n", currentPreset.flags.boostEn);
	DBG_SERIAL.printf("oct: %02x\r\n", currentPreset.flags.octaveEn);
	DBG_SERIAL.printf("amp#: %02x\r\n", currentPreset.flags.ampModelNo);
	DBG_SERIAL.printf("doubler: %02x\r\n", currentPreset.flags.doublerEn);
	DBG_SERIAL.printf("cab#: %02x\r\n", currentPreset.flags.cabIrNo);
	DBG_SERIAL.printf("delay: %02x\r\n", currentPreset.flags.delayEn);
	DBG_SERIAL.printf("delay Freeze: %02x\r\n", currentPreset.flags.delayFreezeEn);
	DBG_SERIAL.printf("reverb: %02x\r\n", currentPreset.flags.reverbEn);
	DBG_SERIAL.printf("rvb type: %02x\r\n", currentPreset.flags.revebType);
	DBG_SERIAL.printf("rvb Freeze: %02x\r\n", currentPreset.flags.reverbFreezeEn);
	DBG_SERIAL.printf("pot1: %02x\r\n", currentPreset.flags.pot1Assign);
	DBG_SERIAL.printf("pot2: %02x\r\n", currentPreset.flags.pot2Assign);
	DBG_SERIAL.printf("pot3: %02x\r\n", currentPreset.flags.pot3Assign);
	DBG_SERIAL.printf("exp : %02x\r\n", currentPreset.flags.expAssign);
	DBG_SERIAL.printf("dry: %02x\r\n", currentPreset.flags.drySigEn);
	DBG_SERIAL.printf("idx: %02x\r\n", currentPreset.flags.idx);
}

void TGX4_ctrl::populatePreset()
{
	memcpy((uint8_t*)&currentPreset.name[0], (uint8_t*)&dataBf[0], member_size(preset_t, name));
	memcpy((uint8_t *)&currentPreset.flags, &dataBf[member_size(preset_t, name)], sizeof(preset_flags_t));
	for (uint8_t i=0; i<PRESET_PARAM_LAST; i++)
	{
		setParam((preset_param_t)i, dataBf[member_size(preset_t, name)+sizeof(preset_flags_t)+i], 0);
	}
	currentPreset.flags.newPresetNo = currentPreset.flags.idx;
	paramChangeMask_tx = 0;
	flagsChangeMask_tx = 0;
}

void TGX4_ctrl::presetUp()
{
	write(GUI_CMD_PRESET_UP, NULL, 0);
}
void TGX4_ctrl::presetDown()
{
	write(GUI_CMD_PRESET_DOWN, NULL, 0);
}