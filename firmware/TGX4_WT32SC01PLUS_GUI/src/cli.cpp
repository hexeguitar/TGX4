#include "cli.h"
#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"
#include <Wire.h>
#include "utils.h"
#include "pinCfg.h"
#include "TGX4.h"
#include "debug_log.h"

void writeChar(EmbeddedCli *embeddedCli, char c);

EmbeddedCliConfig *config = embeddedCliDefaultConfig();

EmbeddedCli *cli = embeddedCliNew(config);

void cb_i2cScan(EmbeddedCli *cli, char *args, void *context);
void cb_getPreset(EmbeddedCli *cli, char *args, void *context);
void cb_info(EmbeddedCli *cli, char *args, void *context);
void cb_rst(EmbeddedCli *cli, char *args, void *context);
void cb_status(EmbeddedCli *cli, char *args, void *context);

void cli_init()
{
	cli->writeChar = writeChar;
	embeddedCliAddBinding(cli, {
			"i2c",
			"Scan I2C bus",
			false,
			nullptr,
			cb_i2cScan
	});	

	embeddedCliAddBinding(cli, {
			"get",
			"Read preset data",
			false,
			nullptr,
			cb_getPreset
	});	
	embeddedCliAddBinding(cli, {
			"info",
			"chip info",
			false,
			nullptr,
			cb_info
	});	
	embeddedCliAddBinding(cli, {
			"rst",
			"reset mcu",
			false,
			nullptr,
			cb_rst
	});	
	embeddedCliAddBinding(cli, {
			"status",
			"print status",
			false,
			nullptr,
			cb_status
	});	

}
void writeChar(EmbeddedCli *embeddedCli, char c)
{
	DBG_SERIAL.write(c);
}

void cli_process()
{
	while(DBG_SERIAL.available())	embeddedCliReceiveChar(cli, DBG_SERIAL.read());
	embeddedCliProcess(cli);	
}

void cb_rst(EmbeddedCli *cli, char *args, void *context)
{
	esp_restart();
}

void cb_i2cScan(EmbeddedCli *cli, char *args, void *context)
{
	TwoWire *i2cBus;
    if (embeddedCliGetTokenCount(args) == 0)
	{
		cli_set_color(CLI_COLOR_GREEN);
		i2cBus = &Wire;
		DBG_SERIAL.println("Scanning bus: Wire");
		cli_reset_color();		
	}
    else
	{
		const char* arg = embeddedCliGetToken(args, 1);
		uint32_t wireNo = strtoul(arg, NULL, 10);
		if (wireNo >= 0 && wireNo < 2)
		{
			cli_set_color(CLI_COLOR_GREEN);
			switch(wireNo)
			{
				case 0: i2cBus = &Wire; break;
				case 1: i2cBus = &Wire1; break;
				default: i2cBus = &Wire; break;
			}
			DBG_SERIAL.printf("Scanning bus: Wire%d\r\n", wireNo);
			cli_reset_color();
		}
		else
		{
			cli_set_color(CLI_COLOR_RED);
			DBG_SERIAL.println("Error! Wrong I2C bus number!");
			cli_reset_color();
			return;			
		}
	}
	i2c_scan(i2cBus, I2C_SDA_GPIO, I2C_SCL_GPIO);
}


void cb_getPreset(EmbeddedCli *cli, char *args, void *context)
{
	teensy.write(GUI_CMD_GET_PRESET, NULL, 0);
}



void cb_info(EmbeddedCli *cli, char *args, void *context)
{

	esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
	DBG_SERIAL.printf(		"SYSTEM INFO:#\n\n"
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
						spi_flash_get_chip_size()/(1024*1024),
						(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embeded" : "external",
						ESP.getHeapSize(),
						ESP.getFreeHeap(),
						ESP.getPsramSize(),
						ESP.getFreePsram());


}

void cb_status(EmbeddedCli *cli, char *args, void *context)
{
	teensy_state_t s = teensy.getState();
	DBG_SERIAL.printf("TGX4 state = %d\r\n", (uint8_t)s);
}