#ifndef _ANALYZE_VUMETERSTEREO_F32_H_
#define _ANALYZE_VUMETERSTEREO_F32_H_

#include "Arduino.h"
#include "AudioStream_F32.h"

class AudioAnalyzeVUmeterStereo_F32 : public AudioStream_F32
{
public:
	AudioAnalyzeVUmeterStereo_F32(void) : AudioStream_F32(2, inputQueueArray_f32) {}
	// Alternate specification of block size.  Sample rate does not apply for analyze_rms
	AudioAnalyzeVUmeterStereo_F32(const AudioSettings_F32 &settings) : AudioStream_F32(2, inputQueueArray_f32)
	{
		block_size = settings.audio_block_samples;
	}

private:
	audio_block_f32_t *inputQueueArray_f32[2];
	uint16_t block_size = AUDIO_BLOCK_SAMPLES;
	uint8_t pin_clipPos;
	uint8_t pin_clipNeg;
	float32_t RMSsmooth;
	double RMS_accum = 0.0;
    uint32_t RMS_count = 0;
};

#endif // _ANALYZE_VUMETER_F32_H_
