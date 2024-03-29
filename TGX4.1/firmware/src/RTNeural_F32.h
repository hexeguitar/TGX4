/**
 * @file RTNeural_F32.cpp
 * @author Piotr Zapart www.hexefx.com
 * @brief Based on Seed pedal design by
 * 		 Keith Bloemer (GuitarML) https://github.com/GuitarML/Seed
 * 		Ported to Teensy4.1 with the following additions/changes:
 * 			- adjusted model volume to match unity gain with bypass signal
 * 			- dual mono input and output
 * 			- added bypass system
 * 
 * 		Required libraries:
 * 				https://github.com/chipaudette/OpenAudio_ArduinoLibrary.git
 * 				https://github.com/hexeguitar/hexefx_audiolib_F32.git
 * @version 0.1
 * @date 2024-01-31
 */
#ifndef _RTNEURAL_F32_H_
#define _RTNEURAL_F32_H_

#include <Arduino.h>
#include "AudioStream_F32.h"
#include "arm_math.h"
// Wiring abs causes problems within RTNeural
#undef abs

#include "RTNeural/RTNeural.h"
#include "RTNeural_models.h"

class AudioEffectRTNeural_F32 : public AudioStream_F32
{
public:
	AudioEffectRTNeural_F32();
	~AudioEffectRTNeural_F32(){};
	virtual void update(void);
	void changeModel(uint8_t modelNo);
	
	void gain(float32_t g)
	{
		g = constrain(g, 0.0f, 1.0f);
		__disable_irq();
		inputGain = g;
		__enable_irq();
	}
	uint8_t getModel() {return bp ? 0 : modelIndex + 1;}


private:
	audio_block_f32_t *inputQueueArray_f32[2];
	RTNeural::ModelT<float, 1, 1,
		RTNeural::GRULayerT<float, 1, 9>,
		RTNeural::DenseT<float, 9, 1>> model;

	uint8_t modelIndex;
	float nnLevelAdjust;
	bool bp = true; //bypass
	float32_t inputGain = 1.0f;
	bool initialized = false;

	inline void transmitSilence()
	{
		audio_block_f32_t* block = AudioStream_F32::allocate_f32();
		if (!block) return;
		memset(&block->data[0], 0, block->length*sizeof(float32_t));
		AudioStream_F32::transmit(block, 0);	// transmit silence	
		AudioStream_F32::transmit(block, 1);
		AudioStream_F32::release(block);
	}
	inline void setModel();

};

#endif // _RTNEURAL_F32_H_
