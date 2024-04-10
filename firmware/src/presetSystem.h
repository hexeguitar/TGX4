#ifndef _PRESETSYSTEM_H_
#define _PRESETSYSTEM_H_
#include <Arduino.h>
#include <arm_math.h>

#define PRESET_NO_DEFAULT				(0)
#define MIDI_CHANNEL_DEFAULT			(1)
#define PRESET_MAX_NO					(8u) // max number of presets

#define PRESET_FLAG_INPUT_SELECT_SHIFT	(0u)
#define PRESET_FLAG_INPUT_SELECT_MASK	(0x03)
#define PRESET_FLAG_INPUT_SELECT(x)		(((x)<<PRESET_FLAG_INPUT_SELECT_SHIFT)&PRESET_FLAG_INPUT_SELECT_MASK)

#define PRESET_FLAG_COMPEN_SHIFT		(2u)
#define PRESET_FLAG_COMPEN_MASK			(0x04u)
#define PRESET_FLAG_COMPEN(x)			(((x)<<PRESET_FLAG_COMPEN_SHIFT)&PRESET_FLAG_COMPEN_MASK)

#define PRESET_FLAG_BOOSTEN_SHIFT		(3u)
#define PRESET_FLAG_BOOSTEN_MASK		(0x08u)
#define PRESET_FLAG_BOOSTEN(x)			(((x)<<PRESET_FLAG_BOOSTEN_SHIFT)&PRESET_FLAG_BOOSTEN_MASK)

#define PRESET_FLAG_AMPNO_SHIFT			(4u)
#define PRESET_FLAG_AMPNO_MASK			(0xF0u)
#define PRESET_FLAG_AMPNO(x)			(((x)<<PRESET_FLAG_AMPNO_SHIFT)&PRESET_FLAG_AMPNO_MASK)

#define PRESET_FLAG_DOUBEN_SHIFT		(8u)
#define PRESET_FLAG_DOUBEN_MASK			(0x0100u)
#define PRESET_FLAG_DOUBEN(x)			(((x)<<PRESET_FLAG_DOUBEN_SHIFT)&PRESET_FLAG_DOUBEN_MASK)

#define PRESET_FLAG_CABIRNO_SHIFT		(9u)
#define PRESET_FLAG_CABIRNO_MASK		(0x1E00u)
#define PRESET_FLAG_CABIRNO(x)			(((x)<<PRESET_FLAG_CABIRNO_SHIFT)&PRESET_FLAG_CABIRNO_MASK)

#define PRESET_FLAG_DELAYEN_SHIFT		(13u)
#define PRESET_FLAG_DELAYEN_MASK		(0x2000u)
#define PRESET_FLAG_DELAYEN(x)			(((x)<<PRESET_FLAG_DELAYEN_SHIFT)&PRESET_FLAG_DELAYEN_MASK)

#define PRESET_FLAG_REVBTYPE_SHIFT		(14u)
#define PRESET_FLAG_REVBTYPE_MASK		(0xC000u)
#define PRESET_FLAG_REVBTYPE(x)			(((x)<<PRESET_FLAG_REVBTYPE_SHIFT)&PRESET_FLAG_REVBTYPE_MASK)

#define PRESET_FLAG_REVBEN_SHIFT		(16u)
#define PRESET_FLAG_REVBEN_MASK			(0x00010000u)
#define PRESET_FLAG_REVBEN(x)			(((x)<<PRESET_FLAG_REVBEN_SHIFT)&PRESET_FLAG_REVBEN_MASK)

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
	PRESET_FLAG_INSELECT 	= 0,
	PRESET_FLAG_COMP_EN,
	PRESET_FLAG_BOOST_EN,
	PRESET_FLAG_OCTAVE_EN,
	PRESET_FLAG_AMPMODEL_NO,
	PRESET_FLAG_DOUBLER_EN,
	PRESET_FLAG_CABIR_NO,
	PRESET_FLAG_DELAY_EN,
	PRESET_FLAG_REVBTYPE,
	PRESET_FLAG_REVB_EN,
	PRESET_FLAG_REVB_FREEZE_EN,
	PRESET_FLAG_DELAY_FREEZE_EN
}preset_paramFlag_t;

typedef struct 
{
	unsigned inSelect:		2; 	// bit 0 1
	unsigned compressorEn:	1;	// bit 2
	unsigned boostEn:		1;	// bit 3
	unsigned octaveEn:		1;	// bit 4
	unsigned ampModelNo:	4;	// bit 5 6 7 8
	unsigned doublerEn:		1;	// bit 9
	unsigned cabIrNo:		4;	// bit 10 11 12 13
	unsigned delayEn:		1;	// bit 14
	unsigned revebType:		2;	// bit 15 16
	unsigned reverbEn:		1;	// bit 17
	unsigned reverbFreezeEn:1; 	// bit 18
	unsigned delayFreezeEn:	1;	// bit 19
	unsigned padding:		12; // 20-31
}preset_flags_bits_t;

typedef union
{
	preset_flags_bits_t bits;
	uint32_t raw;
}preset_flags_t;

typedef struct
{
	preset_flags_t flags;
	float32_t fxParams[PRESET_PARAM_LAST];
}preset_t;

extern preset_t currentPreset;

void preset_init();
void preset_writeDefaults(void);

inline void preset_updateFXparam(preset_param_t paramName, float32_t newValue);
inline void preset_updateFXparam(preset_param_t paramName, float32_t newValue)
{
	currentPreset.fxParams[paramName] = newValue;
}

inline float32_t preset_getFXparam(preset_param_t paramName);
inline float32_t preset_getFXparam(preset_param_t paramName)
{
	return (currentPreset.fxParams[paramName]);
}

void preset_updateFlag(preset_paramFlag_t flag, uint8_t value);
uint8_t preset_getFlag(preset_paramFlag_t flag);	// return individual flags
uint32_t preset_getFlag();							// return all flags as 32bit word

void preset_load(uint32_t presetNo);
void preset_save();
uint8_t preset_getCurrentIdx();

void presetSystem_process();
uint8_t midiChannel_get();
void midiChannel_set(uint8_t val);

#endif // _PRESETSYSTEM_H_
