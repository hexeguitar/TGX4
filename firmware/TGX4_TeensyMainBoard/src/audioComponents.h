#ifndef _AUDIOCOMPONENTS_H_
#define _AUDIOCOMPONENTS_H_

#include "OpenAudio_ArduinoLibrary.h"
#include "hexefx_audiolib_F32.h"
#include "RTNeural_F32.h"
// output highpass filter settings
#define MASTER_LOWCUT_FMIN	(20.0f)
#define MASTER_LOWCUT_FMAX	(300.0f)


extern	AudioSwitchSelectorStereo			inputSwitch;
extern	AudioEffectCompressorStereo_F32 	compressor;
extern  AudioEffectWahMono_F32				wah;
extern	AudioEffectGuitarBooster_F32		booster;
extern	AudioEffectRTNeural_F32				amp;
extern	AudioFilterEqualizer3bandStereo_F32	toneStack;
extern	AudioEffectNoiseGateStereo_F32		gate;
extern	AudioEffectDelayStereo_F32			echo;
extern 	AudioEffectReverbSc_F32				reverbSC;
extern	AudioEffectSpringReverb_F32			reverbSP;
extern	AudioEffectPlateReverb_F32			reverbPL;
extern	AudioEffectXfaderStereo_F32			xfader_main;
extern	AudioEffectXfaderStereo_F32			xfader_FX;
extern  AudioFilterBiquadStereo_F32			masterLowCut;
extern	AudioFilterIRCabsim_F32				cabsim;
extern	AudioEffectGainStereo_F32			masterVol;

#if defined(USE_I2S1)
extern	AudioInputI2S_ext_F32				i2s_in;
extern 	AudioOutputI2S_ext_F32     			i2s_out; 
#elif defined(USE_I2S2)
extern 	AudioInputI2S2_F32					i2s_in;
extern 	AudioOutputI2S2_F32     			i2s_out;
#endif

#endif // _AUDIOCOMPONENTS_H_
