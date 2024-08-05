# Defining a new hardware platform  

Hardware platform for the TGX4 project consists of minimum one to two components:  #
1. Main board driver, defined in firmware as **hw** object. This part includes the Teensy and the audio system (codec or separate adc +dac).
2. Optional controller driver, defined in firmware as **gui** object. This can be a separate board with pots and switched or as in case of the dedicated hardware, the ESP32S3 board + touch display.  

### Hardware platform class  

Minimum requirements for a custom hardware driver are:  
1. Codec chip driver configured with the following settings:  
	- Sampling frequency: 44.1kHz
	- Master clock = 256 * Fs
	- 32bit left justified word (I2S format)
2. Optional GPIO configuration if the board uses analog switching for bypass, dry and wet signals.  

Take a look at the `hw_template.h` file in the `hardware_defs` folder.  
Contrary to the classic Teensy AudioLib auto generated code, a codec class (ie. *AudioControl_SGTL5000*) is a private member of the hardware platform class.  

A hardware platform class has to implement the following methods:  
- `bool init(void)` -  initializing the codec and if required other peripherals, GPIOS used for switches, pots and other control elements.  
- `bool dry_set(hw_state_t newState)` - used to externally switch the DRY signal. Often it can be a GPIO controlling an analog CMOS/FET switch or an already built in feature of the codec chip. If not used, should return `false`.  
- `bool dry_get()` - returns the dry signal switch state .
- `bool wet_set(hw_state_t newState)` - as above, only for the WET signal. Make it return `true` if not used, as in such cases the output is a WET signal only.  
- `bool wet_get()` - returns the wet signal switch state.  
- `bool state_set(hw_state_t newState)` - this is a master bypass switch. Normally it should switch off the WET signal and switch on the DRY signal if bypass is engaged. It can also be set to toggle the WET signal only if the device is used for time based effects only with the DRY signal always on.  
- `bool state_get()` returns the master bypass state.  
- `bool process(void)` - this method will be used for hardware tasks like reading switches, pots etc.  
- `bool phase_invert = false;` this variable sets the output phase. In some situations, like having an output MFB anti aliasing filters, which invert phase, it is convenient to flip it before the DAC. Set to true to enable output phase inverse.  

Create your own hardware class header+source file and put it in the `hardware_defs` folder.   
At the end of the header file add the preprocessor condition
```c++
#elif defined(USE_YOUR_CUSTOM_HARDWARE_PLATFORM)
	extern hw_YourCustomHardwarePlatform hw;	
#endif
```
The source file, in minimal version can be used to instantiate the class only:  
`yourCustomHardwarePlatform.cpp`:  
```c++
#include "yourCustomHardwarePlatform.h"
#elif defined(USE_YOUR_CUSTOM_HARDWARE_PLATFORM)
	hw_YourCustomHardwarePlatform hw;	
#endif
```
Next, include the new header file in the `hardware_defs.h` file.   
`USE_YOUR_CUSTOM_HARDWARE_PLATFORM` is a flag defined in the `platformio.ini` file and it controls which HW platform will the project be compiled for. 

```
[env:t40_es8388]
board = teensy40
build_flags = 
		${env.build_flags}
		-DUSE_CTRL_DUMMY
		-DUSE_I2S1
		-DUSE_ES8388	

[env:t41_es8388]
board = teensy41
build_flags = 
		${env.build_flags}
		-DUSE_CTRL_DUMMY
		-DUSE_I2S1
		-DUSE_ES8388	

; add new custom environment  
[env:your_custom_hw_platform]
board = teensy41	;or teensy4.0
build_flags = 
		${env.build_flags}
		-DUSE_YOUR_CUSTOM_HARDWARE_PLATFORM
		-DUSE_CTRL_DUMMY
		-DUSE_I2S1		; if using standard I2S
		;-DUSE_I2S2		; if using I2S2
		-DUSE_SGTL5000	; define which codec is used, optional

```  

### Controller/GUI class 
Similar to the HW platform class, this one creates a control interface.  
Required methods:  
- `void init(void)` -  initializing the controller and all it's used hardware.  
- `void process(void)` - processing the inputs, outputs, updating the parameters.  
- `void signalChage()` - used to inform the controller about a change or to confirm receiving a command.  

The header file should include the extern declaration of the class as **gui** object.
```c++
#if defined (USE_CTRL_CUSTOM)
	extern ctrl_custom gui;
#endif
```  
and the source file should instantiate it: 
`ctrl_custom.cpp`:  
```c++
#include "ctrl_custom.h"
#if defined (USE_CTRL_CUSTOM)
	ctrl_custom gui;
#endif
``` 
#### If no controller is required it has to be declared as DUMMY in the `platformio.ini` file:  
`-DUSE_CTRL_DUMMY`

----
Once all the files are in place change the default build environment to the new defined:  
```
[platformio]
default_envs = your_custom_hw_platform
```
Build and upload the project.  

## Tip for Linux users  
Working with two boards programmed via Serial port can be frustrating if they constantly change the serial port number. Luckily, the versatile udev on Linux comes with a helping hand and allows to create USB aliases. This way a device with a specified VID/PID can be accessed via a custom name instead of the direct /dev/ttyxx device.  
For this project, i have created a custom udev rules file  
`99-MCU_alias.rules`  
with the content:  
```bash
SUBSYSTEM=="tty", ATTRS{idVendor}=="16c0", ATTRS{idProduct}=="0489", SYMLINK+="ttyACM_Teensy4"
SUBSYSTEM=="tty", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", SYMLINK+="ttyACM_ESP32"
```  
and copied it to the /etc/udev/rules.d/ directory:  
`sudo cp  99-MCU_alias.rules /etc/udev/rules.d/`

Running `sudo udevadm control --reload` and re plugging the board makes them appear as `ttyACM_Teensy4` and `ttyACM_ESP32` no matter which `ttyACMx` port they are on.  

----  
Copyright (c) 2024 by Piotr Zapart  
www.hexefx.com  