#ifndef _ANALYZE_CLIPINDICATOR_F32_H_
#define _ANALYZE_CLIPINDICATOR_F32_H_

#include <Arduino.h>
#include "AudioStream_F32.h"

class AudioAnalyzeClipIndicator : public AudioStream_F32
{
public:
	AudioAnalyzeClipIndicator(int8_t pos, int8_t neg = -1) : AudioStream_F32(1, inputQueueArray_f32), pinPos(pos), pinNeg(neg) {}
	// Alternate specification of block size.  Sample rate does not apply for analyze_rms
	AudioAnalyzeClipIndicator(const AudioSettings_F32 &settings, int8_t pos, int8_t neg = -1) : AudioStream_F32(1, inputQueueArray_f32),
																								pinPos(pos), pinNeg(neg)
	{
		block_size = settings.audio_block_samples;
	}
	/**
	 * @brief define gpios used to blink leds.
	 *
	 * @param pinClipPos - pin for the postive/abs clip led
	 * @param pinClipNeg - pin for the negative clip led, use -1 to set the abs mode
	 * @param blinktime_ms - blink time in ms
	 */
	void FLASHMEM begin(bool active_low = true, uint16_t blinktime_ms = 100)
	{
		out_activeLow = active_low;
		blinkT_ms = blinktime_ms;
		pinMode(pinPos, OUTPUT);
		digitalWriteFast(pinPos, LOW ^ out_activeLow);
		if (pinNeg > 0)
		{
			abs_mode = false;
			pinMode(pinNeg, OUTPUT);
			digitalWriteFast(pinNeg, LOW ^ out_activeLow);
		}
	}
	void set_clipLevel(float32_t clipLvl)
	{
		clipLevel = clipLvl;
	}

	void update()
	{
		audio_block_f32_t *blockIn;
		const float32_t *p, *end;
		float32_t d;
		uint32_t tNow;
		blockIn = AudioStream_F32::receiveReadOnly_f32();
		if (!blockIn) return;
		
		p = blockIn->data;
		end = p + block_size;
		do 
		{
			d = *p++;
			if (abs_mode) d = fabsf(d);
			if (d >= clipLevel) 
			{
				digitalWriteFast(pinPos, HIGH ^  out_activeLow);
				clipFlag |= 1;
				blinkerPos = millis();
			}
			if (d < -clipLevel) // will never be true if abs is enabled
			{
				digitalWriteFast(pinNeg, HIGH ^  out_activeLow);
				clipFlag |= 2;
				blinkerNeg = millis();				
			}
			tNow = millis();
			if ((tNow - blinkerPos > blinkT_ms) && (clipFlag & 1))
			{
				digitalWriteFast(pinPos, LOW ^  out_activeLow);
				clipFlag &= ~1;
			}
			if ((tNow - blinkerNeg > blinkT_ms) && (clipFlag & 2))
			{
				digitalWriteFast(pinNeg, LOW ^  out_activeLow);
				clipFlag &= ~2;
			}			
		} while (p < end);		
	}

private:
	audio_block_f32_t *inputQueueArray_f32[1];
	uint16_t block_size = AUDIO_BLOCK_SAMPLES;
	bool abs_mode = true;
	bool out_activeLow = true;
	uint8_t clipFlag;
	uint16_t blinkT_ms;
	uint32_t blinkerPos;
	uint32_t blinkerNeg;
	const int8_t pinPos;
	const int8_t pinNeg;
	float32_t clipLevel = 0.9999f;
};

#endif // _ANALYZE_CLIPINDICATOR_F32_H_
