/*
 * ResponsiveAnalogRead.h
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

#ifndef _TGX4_POT_H
#define _TGX4_POT_H

#include <Arduino.h>

#define ADC_RES_BITS (10u)

typedef enum
{
    POT_TAPER_LIN,
    POT_TAPER_LOG,
    POT_TAPER_ANTILOG,
    POT_TAPER_SWITCH
} tgx4Pot_taper_t;

class tgx4Pot
{
public:
    // pin - the pin to read
    // sleepEnable - enabling sleep will cause values to take less time to stop changing and potentially stop changing more abruptly,
    //   where as disabling sleep will cause values to ease into their correct position smoothly
    // snapMultiplier - a value from 0 to 1 that controls the amount of easing
    //   increase this to lessen the amount of easing (such as 0.1) and make the responsive values more responsive
    //   but doing so may cause more noise to seep through if sleep is not enabled

    tgx4Pot(){}; // default constructor must be followed by call to begin function
    tgx4Pot(uint8_t inPin, const char *potLabel, bool sleepEnable, float snapMultiplier = 0.01f, tgx4Pot_taper_t pot_taper = POT_TAPER_LIN, uint8_t aver = 0, uint8_t pos = 0, const uint16_t *thresTable = nullptr)
    {
        begin(inPin, potLabel, sleepEnable, snapMultiplier, pot_taper, aver, pos, thresTable);
    };

    void begin(uint8_t AinPin, const char *potLabel, bool sleepEnable, float snapMultiplier = 0.01f, tgx4Pot_taper_t pot_taper = POT_TAPER_LIN, uint8_t aver = 0, uint8_t pos = 0, const uint16_t *thresTable = nullptr); // use with default constructor to initialize

    inline int getValue() { return responsiveValue; } // get the responsive value from last update
    inline float getValue_f() { 
          return (taper==POT_TAPER_SWITCH ? (float)responsiveValue : ((float)responsiveValue / (float)analogResolution));
    }
    inline int getRawValue() { return rawValue; }                        // get the raw analogRead() value from last update
    inline bool hasChanged() { return flags.enabled ? flags.responsiveValueHasChanged : 0; } // returns true if the responsive value has changed during the last update
    inline bool isSleeping() { return flags.sleeping; }                  // returns true if the algorithm is currently in sleeping mode
    void update();                                                       // updates the value by performing an analogRead() and calculating a responsive value based off it
    void update(int rawValueRead);                                       // updates the value accepting a value and calculating a responsive value based off it

    void setSnapMultiplier(float newMultiplier);
	inline void enable(bool state) { flags.enabled = state ? 1 : 0; }
    inline void enableSleep() { flags.sleepEnable = true; }
    inline void disableSleep() { flags.sleepEnable = false; }
    inline void enableEdgeSnap() { flags.edgeSnapEnable = true; }
    // edge snap ensures that values at the edges of the spectrum (0 and 1023) can be easily reached when sleep is enabled
    inline void disableEdgeSnap() { flags.edgeSnapEnable = false; }
    inline void setActivityThreshold(float newThreshold) { activityThreshold = newThreshold; }
    // the amount of movement that must take place to register as activity and start moving the output value. Defaults to 4.0

    void setPotChange_callback(void (*fptr)(float data, const char *potName))
    {
        potChange_callback = fptr;
    }

    const char *label;

private:
    struct flags
    {
		unsigned enabled: 						1;
        unsigned responsiveValueHasChanged: 	1;
        unsigned sleepEnable : 					1;
        unsigned edgeSnapEnable : 				1;
        unsigned sleeping : 					1;
    } flags;

    uint8_t pin;
    static const uint8_t AnalogResolutionBits = ADC_RES_BITS;
    static const uint32_t analogResolution = 1 << ADC_RES_BITS;
    float snapMultiplier;

    float activityThreshold = 4.0f;

    float smoothValue;
    unsigned long lastActivityMS;
    float errorEMA = 0.0f;

    uint8_t switchPositions = 0;    // number of positions if used as switch input
    const uint16_t *switchThresTable = nullptr;

    uint8_t averaging = 32;
    uint8_t readCounter = 0;
    int32_t sum;
    int32_t rawValue;
    int32_t responsiveValue;
    int32_t prevResponsiveValue;

    tgx4Pot_taper_t taper;

    void (*potChange_callback)(float potValue, const char *potName);

    int32_t getResponsiveValue(int32_t newValue);
    int32_t applyTaper(int32_t inValue);
    float snapCurve(float x);
};

#endif // _POTLIB_H
