#ifndef _UTILS_H_
#define _UTILS_H_
#include <Arduino.h>
#include <Wire.h>

void printSysInfo(char * buf);
void i2c_scan(TwoWire *i2cBus, uint8_t sdaPin, uint8_t sclPin);
void printBin(uint64_t v, uint8_t num_places);

#endif // _UTILS_H_
