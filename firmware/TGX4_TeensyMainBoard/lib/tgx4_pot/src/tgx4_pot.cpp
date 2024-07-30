/*
 * ResponsiveAnalogRead.cpp
 * Arduino library for eliminating noise in analogRead inputs without decreasing responsiveness
 *
 * Copyright (c) 2016 Damien Clarke
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "tgx4_pot.h"

void tgx4Pot::begin(uint8_t AinPin, const char *potLabel, bool sleepEnable, float snapMultiplier, tgx4Pot_taper_t pot_taper, uint8_t aver, uint8_t pos, const uint16_t *thresTable)
{
    pinMode(pin, INPUT);    // ensure button pin is an input
    digitalWrite(pin, LOW); // ensure pullup is off on button pin
    switchPositions = pos;
    switchThresTable = thresTable;
    potChange_callback = NULL;
    label = potLabel;
    pin = AinPin;
    flags.sleepEnable = sleepEnable;
	flags.enabled = 1;
    taper = pot_taper;
    averaging = aver;
    setSnapMultiplier(snapMultiplier);
}

void tgx4Pot::update(void)
{
	if (!flags.enabled) return;
    if (averaging)
    {
        sum += analogRead(pin);
        if (++readCounter == averaging)
        {
            readCounter = 0;
            this->update(sum / averaging);
            sum = 0;
        }
    }
    else
    {
        update(analogRead(pin));
    }
}

void tgx4Pot::update(int rawValueRead)
{
	if (!flags.enabled) return;
    rawValue = rawValueRead;
    prevResponsiveValue = responsiveValue;
    responsiveValue = getResponsiveValue(rawValue);
    flags.responsiveValueHasChanged = responsiveValue != prevResponsiveValue;
    if (potChange_callback && flags.responsiveValueHasChanged)
    {
        //if (taper == POT_TAPER_SWITCH)
            //Serial.printf("SWposRaw= %d resp = %d\r\n", rawValue, responsiveValue);
        (*potChange_callback)(getValue_f(), label);
    }
}

int32_t tgx4Pot::getResponsiveValue(int32_t newValue)
{
    // if sleep and edge snap are enabled and the new value is very close to an edge, drag it a little closer to the edges
    // This'll make it easier to pull the output values right to the extremes without sleeping,
    // and it'll make movements right near the edge appear larger, making it easier to wake up
    if (flags.sleepEnable && flags.edgeSnapEnable)
    {
        if (newValue < activityThreshold)
        {
            newValue = (newValue * 2) - activityThreshold;
        }
        else if (newValue > analogResolution - activityThreshold)
        {
            newValue = (newValue * 2) - analogResolution + activityThreshold;
        }
    }

    // get difference between new input value and current smooth value
    unsigned int diff = abs(newValue - smoothValue);

    // measure the difference between the new value and current value
    // and use another exponential moving average to work out what
    // the current margin of error is
    errorEMA += ((newValue - smoothValue) - errorEMA) * 0.4f;

    // if sleep has been enabled, sleep when the amount of error is below the activity threshold
    if (flags.sleepEnable)
    {
        // recalculate sleeping status
        flags.sleeping = abs(errorEMA) < activityThreshold;
    }

    // if we're allowed to sleep, and we're sleeping
    // then don't update responsiveValue this loop
    // just output the existing responsiveValue
    if (flags.sleepEnable && flags.sleeping)
    {
        return (applyTaper(smoothValue));
    }

    // use a 'snap curve' function, where we pass in the diff (x) and get back a number from 0-1.
    // We want small values of x to result in an output close to zero, so when the smooth value is close to the input value
    // it'll smooth out noise aggressively by responding slowly to sudden changes.
    // We want a small increase in x to result in a much higher output value, so medium and large movements are snappy and responsive,
    // and aren't made sluggish by unnecessarily filtering out noise. A hyperbola (f(x) = 1/x) curve is used.
    // First x has an offset of 1 applied, so x = 0 now results in a value of 1 from the hyperbola function.
    // High values of x tend toward 0, but we want an output that begins at 0 and tends toward 1, so 1-y flips this up the right way.
    // Finally the result is multiplied by 2 and capped at a maximum of one, which means that at a certain point all larger movements are maximally snappy

    // then multiply the input by SNAP_MULTIPLER so input values fit the snap curve better.
    float snap = snapCurve(diff * snapMultiplier);

    // when sleep is enabled, the emphasis is stopping on a responsiveValue quickly, and it's less about easing into position.
    // If sleep is enabled, add a small amount to snap so it'll tend to snap into a more accurate position before sleeping starts.
    if (flags.sleepEnable)
    {
         snap = 0.5f*snap + 0.5f;
    }

    // calculate the exponential moving average based on the snap
    smoothValue += (newValue - smoothValue) * snap;

    // ensure output is in bounds
    if (smoothValue < 0.0f)
    {
        smoothValue = 0.0f;
    }
    else if (smoothValue > analogResolution - 1)
    {
        smoothValue = analogResolution - 1;
    }
    return (applyTaper(smoothValue));
}

int32_t tgx4Pot::applyTaper(int32_t inValue)
{
    int32_t result = 0;
    uint8_t _swPos = 0;
    switch (taper)
    {
		case POT_TAPER_LIN:
			result = (int32_t)smoothValue;
			break;
		case POT_TAPER_LOG:
			result = (int32_t)((smoothValue * smoothValue) / (analogResolution - 1));
			break;
		case POT_TAPER_ANTILOG:
			result = (int32_t)(2.0f * smoothValue - ((smoothValue * smoothValue) / (analogResolution - 1)));
			break;
		case POT_TAPER_SWITCH:
			if (switchPositions == 0 || !switchThresTable)
			{
				result = 0;
				break;
			}
			_swPos = 0;
			while ((float)switchThresTable[_swPos] < smoothValue && _swPos < switchPositions)
			{
				_swPos++;
			}
			result = (int32_t)_swPos;
			break;
		default:
			break;
    }
    return result;
}

float tgx4Pot::snapCurve(float x)
{
    float y = 1.0f / (x + 1.0f);
    y = (1.0f - y) * 2.0f;
    if (y > 1.0f)
    {
        return 1.0f;
    }
    return y;
}

void tgx4Pot::setSnapMultiplier(float newMultiplier)
{
	newMultiplier = constrain(newMultiplier, 0.0f, 1.0f);
    snapMultiplier = newMultiplier;
}
