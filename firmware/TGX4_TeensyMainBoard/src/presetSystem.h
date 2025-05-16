#ifndef _PRESETSYSTEM_H_
#define _PRESETSYSTEM_H_
#include <Arduino.h>
#include <arm_math.h>

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
	unsigned bypass:		1; // master bypass
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
	unsigned newPresetNo:	7;	// temporary preset number used for browsing
}preset_flags_t;

typedef struct
{
	char name[PRESET_NAME_MAXCHARS+1];
	preset_flags_t flags;
	float32_t fxParams[PRESET_PARAM_LAST];
}preset_t;


typedef void (*flagChange_callback)(uint8_t value);
typedef void (*paramChange_callback)(float32_t value);

class PresetSystem
{
public:
	PresetSystem(){};
	~PresetSystem(){};
	void init();
	void writeDefaults(void);

	char* getName() { return currentPreset.name; }
	void setName(char* name, bool signal=false) 
	{
		memcpy(&currentPreset.name, name, member_size(preset_t, name)); 
		if (signal) triggerFullGuiUpdate();
	}
	char* getName(uint8_t presetNo);
	void updateFlag(preset_paramFlag_t flag, uint8_t value, bool signal=true);
	uint8_t getFlag(preset_paramFlag_t flag, preset_flags_t* flagsPrt = NULL);
	uint8_t* getFlagsPtr() { return (uint8_t *)&currentPreset.flags; }
	void setFlagCallback(preset_paramFlag_t flag, flagChange_callback cb)
	{
		if (flag >= PRESET_FLAG_LAST) return;
		flag_cb[(uint8_t)flag] = cb;
	}
	void setParamCallback(preset_param_t param, paramChange_callback cb)
	{
		if (param >= PRESET_PARAM_LAST) return;
		param_cb[(uint8_t)param] = cb;
	}
	inline float32_t getFXparam(preset_param_t paramName)
	{
		return (currentPreset.fxParams[(uint8_t)paramName]);
	}
	inline void updateFXparam(preset_param_t paramName, float32_t newValue, bool signal=true)
	{
		if (paramName >= PRESET_PARAM_LAST) return;
		currentPreset.fxParams[paramName] = newValue;
		if (signal) paramChangeMask |= 1ull<<((uint8_t)paramName);
		if(param_cb[(uint8_t)paramName]) 
		{
			(*param_cb[(uint8_t)paramName])(newValue); // trigger the callback if defined
		}
	}

	// returns the value of the flag that a footswitch is assigned to
	uint8_t getFootswAssignFlagValue(uint8_t assignValue);

	inline uint32_t getFlagsChangeMask() { return flagChangeMask;}
	inline uint64_t getParamsChangeMask() { return paramChangeMask;	}

	inline void resetChangeMask() { flagChangeMask = 0;	paramChangeMask = 0; }
	// this will trigger a full preset update for the GUI
	inline void triggerFullGuiUpdate() 
	{
		flagChangeMask = 0xFFFFFFFF;
		paramChangeMask = 0xFFFFFFFFFFFFFFFF;
	}

	void load(int16_t presetNo);
	void preLoad_next();
	void load_next();
	void load_previous();
	void preLoad_previous();
	void resetPreload() // reset the browsed preset# back to the current one
	{
		currentPreset.flags.newPresetNo = currentPreset.flags.idx;
	}
	// load the preset browsed via the up/down buttons 
	void activate()
	{
		load(currentPreset.flags.newPresetNo);
		triggerFullGuiUpdate();
	}

	void save()
	{
		saveParamsFlag = true; // trigger delayed eeprom save
		t_saveParam = millis();
	}
	uint8_t getCurrentIdx() { return currentPresetIdx; }
	void process();
	uint8_t getMidiChannel() { return midiChannel; }
	void setMidiChannel(uint8_t val);
	
	bool getSDstatus() {return SDcard_ready;}
	// length of the preset with params scaled to uint8_t (used with gui)
	const uint8_t length_paramsU8 = member_size(preset_t, name) +\
									sizeof(preset_flags_t) +\
									PRESET_PARAM_LAST;
	void loadCurrent();
	void printFlags();

private:
	void writeToEeprom(uint8_t presetIdx, preset_t *presetPtr);
	uint8_t load_EEP(int16_t presetNo);
	uint8_t load_SD(int16_t presetNo);
	void write(uint8_t presetNo, preset_t *presetPtr);

	preset_t currentPreset;
	const uint32_t save_timeout = 2000u;
	uint8_t currentPresetIdx = 0;
	uint32_t t_now, t_saveParam, t_saveLastPreset;
	bool SDcard_ready = false;
	bool saveParamsFlag = false;
	bool saveLastPresetFlag = false;
	uint16_t marker_eepAddr = 0;
	const uint8_t eep_marker = 0x5A;
	const uint16_t midiChannel_eepAddr = marker_eepAddr + sizeof(eep_marker);
	uint8_t midiChannel = 1;
	const uint16_t presetLast_eepAddr = midiChannel_eepAddr + sizeof(midiChannel);
	const uint16_t preset_eepAddr_base = presetLast_eepAddr + sizeof(presetLast_eepAddr);
	const uint16_t preset_eepSizeBytes = sizeof(currentPreset.name) + sizeof(currentPreset.flags) + sizeof(currentPreset.fxParams)/sizeof(float32_t);
	// each bit is one param (idx=preset_param_t)  used to report 
	// the local changes (via pot, switch etc) back to the GUI so it can update the display accordingly
	uint64_t paramChangeMask = 0;
	uint32_t flagChangeMask = 0;

	// Array of callbacks triggered upon a flag change
	flagChange_callback flag_cb[PRESET_FLAG_LAST];
	// Array of callbacks triggered upon a parameter change
	paramChange_callback param_cb[PRESET_PARAM_LAST];
	uint8_t tmpBf[member_size(preset_t, name)];

	uint32_t presetPreloadTimestamp = 0;
	const uint32_t presetPreloadTimeout = 2000;
};

extern PresetSystem presetSystem;

#endif // _PRESETSYSTEM_H_
