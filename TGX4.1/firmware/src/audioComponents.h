#ifndef _AUDIOCOMPONENTS_H_
#define _AUDIOCOMPONENTS_H_

#include "OpenAudio_ArduinoLibrary.h"
#include "hexefx_audio_F32.h"
#include "RTNeural_F32.h"

#define MASTER_LOWCUT_FMIN	(20.0f)
#define MASTER_LOWCUT_FMAX	(300.0f)

#if defined(USE_TEENSY_AUDIO_BOARD) || defined(USE_BLACKADDR_TGAPRO)
extern	AudioInputI2S_F32				i2s_in;
// T41GFX uses I2S2
#elif defined(USE_HEXEFX_T41GFX)
extern	AudioInputI2S2_F32				i2s_in;
#endif

extern	AudioSwitchSelectorStereo		inputSwitch;
extern	AudioEffectCompressorStereo_F32 compressor;
extern	AudioEffectGuitarBooster_F32	booster;
extern	AudioEffectRTNeural_F32			amp;
extern	AudioFilterEqualizer3bandStereo_F32	toneStack;
extern	AudioEffectNoiseGateStereo_F32	gate;
extern	AudioEffectDelayStereo_F32		echo;
extern	AudioEffectSpringReverb_F32		reverbSP;
extern	AudioEffectPlateReverb_F32		reverbPL;
extern	AudioEffectReverbSc_F32			reverbSC;
extern	AudioEffectXfaderStereo_F32		xfader_main;
extern	AudioEffectXfaderStereo_F32		xfader_FX;
extern  AudioFilterBiquadStereo_F32		masterLowCut;
extern	AudioFilterIRCabsim_F32			cabsim;
extern	AudioEffectGainStereo_F32		masterVol;

#if defined(USE_TEENSY_AUDIO_BOARD)
	extern	AudioOutputI2S_F32     			i2s_out;
	extern	AudioControlSGTL5000_F32		codec;
#elif defined(USE_BLACKADDR_TGAPRO)
	extern	AudioOutputI2S_F32     			i2s_out;
	extern	AudioControlWM8731_F32			codec;
#elif defined(USE_HEXEFX_T41GFX)
	extern	AudioOutputI2S2_F32     		i2s_out;
	extern	AudioControlWM8731_F32			codec;
#endif

#endif // _AUDIOCOMPONENTS_H_
