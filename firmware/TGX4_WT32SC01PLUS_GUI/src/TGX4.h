#ifndef _TGX4_H_
#define _TGX4_H_

#include <Arduino.h>
#include <Wire.h>

#define member_size(type, member) (sizeof( ((type *)0)->member ))
#define PRESET_NO_DEFAULT				(0)
#define MIDI_CHANNEL_DEFAULT			(1)
#define PRESET_MAX_NO_EEP				(8u) // max number of presets stored in EEPROM
#define PRESET_MAX_NO					(128u) // max number of presets

#define PRESET_NAME_MAXCHARS			(15)

enum IRcabs_t
{
	IR_GUITAR1 = 0,
	IR_GUITAR2,
	IR_GUITAR3,
	IR_GUITAR4,
	IR_GUITAR5,
	IR_GUITAR6,
	IR_GUITAR7,
	IR_BASS1,
	IR_BASS2,
	IR_BASS3,
	IR_OFF
};

enum ampModels_t
{
	AMP_OFF = 0,
	AMP_CLEAN1,
	AMP_CLEAN2,
	AMP_CLEAN3,
	AMP_CLEAN4,
	AMP_CRUNCH1,
	AMP_CRUNCH2,
	AMP_LEAD1,
	AMP_LEAD2
};

typedef enum
{
	REVERB_PLATE  = 0,
	REVERB_SC	  = 1,
	REVERB_SPRING = 2
}active_reverb_e;

typedef enum
{
	PRESET_PARAM_COMP_PREGAIN = 0,
	PRESET_PARAM_COMP_THRES,
	PRESET_PARAM_COMP_RATIO,
	PRESET_PARAM_COMP_ATTACK,
	PRESET_PARAM_COMP_RELEASE,
	PRESET_PARAM_COMP_POSTGAIN,
	PRESET_PARAM_WAH_FREQ,
	PRESET_PARAM_WAH_MIX,
	PRESET_PARAM_BOOST_DRIVE,
	PRESET_PARAM_BOOST_BOTTOM,
	PRESET_PARAM_BOOST_PRESENCE,
	PRESET_PARAM_BOOST_MIX,
	PRESET_PARAM_BOOST_LEVEL,
	PRESET_PARAM_AMP_GATE,
	PRESET_PARAM_AMP_GAIN,
	PRESET_PARAM_AMP_BASS,
	PRESET_PARAM_AMP_MID,
	PRESET_PARAM_AMP_TREBLE,
	PRESET_PARAM_AMP_FXMIX,
	PRESET_PARAM_DELAY_TIME,
	PRESET_PARAM_DELAY_INERTIA,
	PRESET_PARAM_DELAY_REPEATS,
	PRESET_PARAM_DELAY_MIX,
	PRESET_PARAM_DELAY_HICUT,
	PRESET_PARAM_DELAY_LOWCUT,
	PRESET_PARAM_DELAY_TREBLE,
	PRESET_PARAM_DELAY_BASS,
	PRESET_PARAM_DELAY_MODRATE,
	PRESET_PARAM_DELAY_MODDEPTH,
	PRESET_PARAM_REVERBPL_SIZE,
	PRESET_PARAM_REVERBPL_DIFF,
	PRESET_PARAM_REVERBPL_MIX,
	PRESET_PARAM_REVERBPL_BASSCUT,
	PRESET_PARAM_REVERBPL_TREBLECUT,
	PRESET_PARAM_REVERBPL_TREBLE,
	PRESET_PARAM_REVERBPL_BASS,
	PRESET_PARAM_REVERBPL_BLEEDIN,
	PRESET_PARAM_REVERBPL_PITCH,
	PRESET_PARAM_REVERBPL_PITCHMIX,
	PRESET_PARAM_REVERBPL_SHIMMER,
	PRESET_PARAM_REVERBPL_PITCHSHIMM,
	PRESET_PARAM_REVERBSC_TIME,
	PRESET_PARAM_REVERBSC_MIX,
	PRESET_PARAM_REVERBSC_LOWPASSF,
	PRESET_PARAM_REVERBSP_TIME,
	PRESET_PARAM_REVERBSP_MIX,
	PRESET_PARAM_REVERBSP_BASSCUT,
	PRESET_PARAM_REVERBSP_TREBLECUT,
	PRESET_PARAM_MASTER_VOLUME,
	PRESET_PARAM_MASTER_PAN,
	PRESET_PARAM_DELAY_REVERB_MIX,
	PRESET_PARAM_MASTER_LOWCUT,

	PRESET_PARAM_LAST // MUST BE LAST!!!
}preset_param_t;


typedef enum
{
	PRESET_FLAG_BYPASS = 0,
	PRESET_FLAG_INSELECT,
	PRESET_FLAG_COMP_EN,
	PRESET_FLAG_WAH_EN,
	PRESET_FLAG_WAH_MODEL,
	PRESET_FLAG_BOOST_EN,
	PRESET_FLAG_OCTAVE_EN,
	PRESET_FLAG_AMPMODEL_NO,
	PRESET_FLAG_DOUBLER_EN,
	PRESET_FLAG_CABIR_NO,
	PRESET_FLAG_DELAY_EN,
	PRESET_FLAG_REVBTYPE,
	PRESET_FLAG_REVB_EN,
	PRESET_FLAG_REVB_FREEZE_EN,
	PRESET_FLAG_DELAY_FREEZE_EN,
	PRESET_FLAG_POT1_ASSIGN,
	PRESET_FLAG_POT2_ASSIGN,
	PRESET_FLAG_POT3_ASSIGN,
	PRESET_FLAG_EXP_ASSIGN,
	PRESET_FLAG_FT1_ASSIGN,
	PRESET_FLAG_FT2_ASSIGN,	
	PRESET_FLAG_DRYSIG,
	PRESET_FLAG_IDX,
	PRESET_FLAG_LAST
}preset_paramFlag_t;

typedef enum
{
	PRESET_FTASSIGN_OFF = 0,
	PRESET_FTASSIGN_COMP,
	PRESET_FTASSIGN_WAH,
	PRESET_FTASSIGN_BOOST,
	PRESET_FTASSIGN_OCT,
	PRESET_FTASSIGN_DELAY,
	PRESET_FTASSIGN_DELAY_FREEZE,
	PRESET_FTASSIGN_DELAY_TAP,
	PRESET_FTASSIGN_REVERB,
	PRESET_FTASSIGN_REVERB_FREEZE,
	PRESET_FTASSIGN_PRESET_UP,
	PRESET_FTASSIGN_PRESET_DOWN,
	PRESET_FTASSIGN_LAST
}preset_footswitchAssign_t;


typedef struct 
{
	unsigned bypass:		1;  // master bypass
	unsigned inSelect:		2;
	unsigned compressorEn:	1;
	unsigned wahEn:			1;
	unsigned wahModel:		3;
	unsigned boostEn:		1;
	unsigned octaveEn:		1;
	unsigned ampModelNo:	4;
	unsigned doublerEn:		1;
	unsigned cabIrNo:		4;
	unsigned delayEn:		1;
	unsigned revebType:		2;
	unsigned reverbEn:		1;
	unsigned reverbFreezeEn:1;
	unsigned delayFreezeEn:	1;
	unsigned pot1Assign:	6;
	unsigned pot2Assign:	6;
	unsigned pot3Assign:	6;
	unsigned expAssign:		6;
	unsigned ft1Assign:		4;
	unsigned ft2Assign:		4;
	unsigned drySigEn:		1;
	unsigned idx:			7;  // index in the 128 preset table
	unsigned hw_info:		5;	// Hardware info flags		- these two are not required for each preset
								// 							- only using the flags to pass the info to the ESP32
								// [4]:ExpSns [3]SDready [2]PSram [10]:Teensy model, 
	unsigned newPresetNo:	7;								
}preset_flags_t;


typedef struct
{
	char name[PRESET_NAME_MAXCHARS+1];
	preset_flags_t flags;
	uint8_t fxParams[PRESET_PARAM_LAST];
}preset_t;

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

typedef enum
{
	TEENSY_STATE_BOOT,
	TEENSY_STATE_IDLE,
	TEENSY_STATE_CMD_SENT
}teensy_state_t;

void gui_int_cb(void);

class TGX4_ctrl
{
public:
	TGX4_ctrl(uint8_t I2Caddr, uint8_t sda, uint8_t scl, uint8_t intGpio)
	{
		_addr = I2Caddr;
		_sda = sda;
		_scl = scl;
		_intPin = intGpio;
	}
	~TGX4_ctrl(){};
	void init();

	uint8_t write(gui_cmd_t command, uint8_t* dataSrc, uint16_t size);
	bool read(uint8_t* dst, uint8_t size);
	char* getPresetName() { return currentPreset.name; } 
	uint8_t getFlag(preset_paramFlag_t flag);
	void setFlag(preset_paramFlag_t flag, uint8_t value, bool signal=true);
	uint8_t getParam(preset_param_t param);
	void setParam(preset_param_t param, uint8_t value, bool signal=true);
	void triggerUpdate() { updateRequestFlag = true; }
	void process();
	void presetUp();
	void presetDown();

	const char* hw_getMCU() { return (currentPreset.flags.hw_info & 0x01 ? "T4.1" : "T4.0"); }
	const char* hw_getPSRAM()	{ return (currentPreset.flags.hw_info & (1<<2) ? "PSRAM" :""); }
	const char* hw_getSDready() { return (currentPreset.flags.hw_info & (1<<3)) ? "SDCARD": ""; }
	const char* hw_getVersion() { return  versionString;}
	teensy_state_t getState() {return state;}
	void printFlags();

private:
	const char* versionString ="2.0";
	uint8_t _sda;
	uint8_t _scl;
	uint8_t _addr;
	uint8_t _intPin;
	bool updateRequestFlag = false;
	uint8_t dataBf[128];
	teensy_state_t state = TEENSY_STATE_BOOT;
	gui_cmd_t cmd = GUI_CMD_NOP;
	uint32_t timer = 0;

	uint32_t flagsChangeMask_rx;
	uint64_t paramChangeMask_rx;
	uint32_t flagsChangeMask_tx;
	uint64_t paramChangeMask_tx;
	preset_t currentPreset;
	
	void processChangeReport();
	void processFlags();
	void processParams();
	void populatePreset(); // save data received via i2c to local preset copy 

	typedef enum
	{
		I2C_ERR_TIMEOUT,
		I2C_ERR_NO_DATA_RECEIVED
	}i2c_error_t;
	void handle_i2cError(i2c_error_t err);

};

extern TGX4_ctrl teensy;

#endif // _TEENSY_CTRL_H_
