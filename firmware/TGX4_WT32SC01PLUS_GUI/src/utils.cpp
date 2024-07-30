#include "utils.h"
#include <Wire.h>
#include "cli.h"

void printSysInfo(char *buf)
{
	if (!buf)
		return;
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	snprintf(buf, 256, "SYSTEM INFO:#\n\n"
					   "%d cores Wifi %s%s\n"
					   "Silicon revision: %d\n"
					   "%dMB %s flash\n"
					   "Total heap: %d\n"
					   "Free heap: %d\n"
					   "Total PSRAM:# %d\n"
					   "Free PSRAM:# %d\n",

			 chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
			 (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
			 chip_info.revision,
			 spi_flash_get_chip_size() / (1024 * 1024),
			 (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embeded" : "external",
			 ESP.getHeapSize(),
			 ESP.getFreeHeap(),
			 ESP.getPsramSize(),
			 ESP.getFreePsram());
}

float map_rng_f(float val, float in_min, float in_max, float out_min, float out_max)
{
	if (val <= in_min)
		return out_min;
	if (val >= in_max)
		return out_max;
	float result = (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	return result;
}

void i2c_scan(TwoWire *i2cBus, uint8_t sdaPin, uint8_t sclPin)
{
	uint8_t i, err;
	TwoWire *_i2c = i2cBus;
	if (!i2cBus)
	{ 
		cli_set_color(CLI_COLOR_RED);
		DBG_SERIAL.println("Error! Please provide valid I2C bus!"); 
		cli_reset_color();
		return;	
	}
	_i2c->begin(sdaPin, sclPin, 100000u);
	DBG_SERIAL.print("   ");
	for (i = 0; i < 16; i++) { DBG_SERIAL.printf("%3x", i); }
	for (i = 0; i <= 127; i++)
	{
		if (i % 16 == 0)
		{
			DBG_SERIAL.printf("\n%02x:", i & 0xF0);
		}
		_i2c->beginTransmission(i);
		err = _i2c->endTransmission();
		
		if (err==0) 	        
		{ 
			cli_set_color(CLI_COLOR_CYAN);
			DBG_SERIAL.printf(" %02x", i);
			cli_reset_color(); 
		}
		else 					
		{ 
			DBG_SERIAL.printf(" XX"); 
		}
		delayMicroseconds(200);		   
	}
	DBG_SERIAL.println("");
}

void printBin(uint64_t v, uint8_t num_places)
{
    uint64_t mask=0, n;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1) | 0x0001ULL;
    }
    v = v & mask;  // truncate v to specified number of places

    while(num_places)
    {

        if (v & (0x0001ULL << (num_places-1))) DBG_SERIAL.write('1');
        else                                DBG_SERIAL.write('0');
        --num_places;
        if(((num_places%4) == 0) && (num_places != 0))
        {
             DBG_SERIAL.write('_');
        }
    }
    DBG_SERIAL.print("\r\n");
}