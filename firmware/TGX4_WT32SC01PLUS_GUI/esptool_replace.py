'''
06.2024 - Issues with uploading firmware to the ESP32-S3
via USB UART/JTAG interface. After uploading the stub into RAM
the process fails with message

A fatal error occurred: No serial data received.
Failed uploading: uploading error: exit status 2

Temporary solution until the esptool gets updated in the ESP32
core for PlatformIO:
This script installs the newest esptool into the platformio
environment and uses it as upload tool, fixing the problem.

Usage: save this scripts as 

esptool_repolace.py 

in the root dir of the project (where the platformio.ini is)
In the platformio.ini file add:

extra_scripts = esptool_replace.py

P.Z. 2024

'''
Import("env")
import os
import inspect 

try:
    import esptool
except ImportError:
    env.Execute("$PYTHONEXE -m pip install esptool")
# get the path to the installed esptool
dir = inspect.getfile(esptool)
# replace the uploader variable with the installed esptool
env.Replace(UPLOADER=dir)

	