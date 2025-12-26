　　　　　　　　　　　　　　　　　　　　　　　　　　2025.12.27
　BASIC Execution Environment Open Platform for Raspberry Pi Pico
　　　　　　　　MachiKania type P
　　　　　　　　　　　　　　　　　　　　　by KenKen & Katsumi


MachiKania is an open platform for microcontrollers with a BASIC compiler.
MachiKania type P is equipped with a Raspberry Pi Pico (Pico 2, Pico W) and 
a small LCD module to realize a portable BASIC execution environment.
External devices such as general-purpose I/O, SPI, and I2C can be easily 
controlled from BASIC.


This document describes MachiKania for the ClockworkPi PicoCalc (hereafter
 referred to as PicoCalc).

The on-board BASIC compiler is KM-BASIC, which is a 32-bit integer BASIC.
It also supports single-precision floating-point arithmetic.
Structured and object-oriented programmings are also available.
BASIC programs can be created on a PC or other device and transferred via 
MMC or SD card, or using a USB cable and dedicated software.
It is also possible to connect a USB keyboard and directly edit and execute 
the program.


The included executable file (uf2 file) can be freely used for non-commercial 
use only, free of charge.
It is also possible to publish your work to the public. In this case, please 
mention that you are using the MachiKania series on your website, in the 
instructions, or in any other place.
Please note that we are not responsible for any damage or loss caused by the 
use of the MachiKania series.

When using the PicoCalc-compatible version of MachiKania, please note the 
following points.

<Supported Pi Pico boards>

It supports four models: Raspberry Pi Pico, Raspberry Pi Pico 2, Raspberry Pi 
Pico W, and Raspberry Pi Pico 2 W. Please install the appropriate phyllosoma_kb.uf2 
file for your board from the distributed archive. For WiFi-compatible versions, 
connecting to the internet via WiFi is also possible.

To install the .uf2 file, follow these steps:
  1. Turn off the power to the PicoCalc.
  2. Flip the PicoCalc over, then while pressing and holding the “BOOTSEL” 
     button on the Raspberry Pi Pico board, connect it to your PC using a USB 
     micro B cable.
  3. A drive named RPI-RP2 or RP2350 will appear on your PC. Drag and drop the 
     phyllosoma_kb.uf2 file into that drive.
  4. Disconnect the USB micro B plug and turn the PicoCalc power back on.

<MACHIKAP.INI>

To ensure that MachiKania works properly on PicoCalc, make sure to place the 
included MACHIKAP.INI file in the root directory of the MMC/SD card.


<Keyboard>

The built-in keyboard of the PicoCalc is supported. Please note that external 
USB keyboards are not recognized.

The differences in operation between a USB keyboard and the PicoCalc keyboard 
are as follows:

  - Since "Shift + Right" and "Shift + Left" cannot be used, use "Alt + Right" 
    and "Alt + Left" instead.
  - Since there are no "Page Up" and "Page Down" buttons, use "Alt + Up" and 
    "Alt + Down" instead.
  - If you want to use "Home" and "End" without the "Shift" key, press 
    "Alt + Tab" and "Alt + Del" respectively.


<Button switches>

MachiKania is operated using six buttons: Up, Down, Left, Right, Fire, and 
Start. Since PicoCalc does not have these physical buttons, they are emulated 
via the keyboard. By default, the arrow keys correspond to directional controls, 
the Space key to Fire, and the Enter key to Start. If you wish to change these 
key mappings, please edit the MACHIKAP.INI file. Settings like "EMULATEBUTTONUP=" 
control these mappings. The value on the right side represents the virtual key 
code in decimal format. To find the virtual key code for a specific key, run the 
BASIC program below and press the desired key.

  DO:PRINT INKEY():LOOP


<Using I2C>

The keyboard interface uses the I2C function (GP6, GP7; I2C1). If you wish to 
use I2C for other purposes, please use I2C0 instead (such as GP0/GP1). This can 
be configured using the "I2CSDA=" and "I2CSCL=" settings in the MACHIKAP.INI file. 
Be careful not to use ports associated with I2C1 (like GP2/GP3) in this setting, 
as the keyboard may no longer be recognized.


<Stopping a BASIC program mid-execution>

To stop a BASIC program while it is running, press the Ctrl + Alt + Del keys 
simultaneously. This will interrupt the program. However, please note that in certain 
loops (such as those without any PRINT commands) the program may not stop as expected.


<machikania-pc.zip file contents>

[pico] directory
　Stores binaries used with Raspberry Pi Pico
　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and keyboard

[pico_w] directory
　Stores binaries used with Raspberry Pi Pico W
　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and keyboard
　　WiFi connection is available.

[pico2] directory
　Stores binaries used with Raspberry Pi Pico 2
　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and keyboard

[pico2_w] directory
　Stores binaries used with Raspberry Pi Pico 2 W
　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and keyboard
　　WiFi connection is available.

readme-e.txt
　This file

readmeLib-e.txt
　Library documentation in the LIB directory

<Copy all files and directories below here to the root directory of the MMC/SD card

MACHIKAP.INI
　Initialization file for MachiKania type P

[LIB] directory
　Class library that can be easily used from BASIC programs 
　Copy the entire LIB directory to the root of the MMC/SD card and use it

[docs] directory
　Stores documents
　- help.txt
　- help-e.txt
　　Reference manual for KM-BASIC

　- class.txt
　- class-e.txt
　　Reference Manual for Object-Oriented Programming in KM-BASIC

　- cpuclock.txt
　- cpuclock-e.txt
　　How to adjust the CPU clock frequency

　- keyboard.txt
　- keyboard-e.txt
　　How to use the keyboard

　- shematic.png
　　MachiKania type P schematic

　- wifi.txt
　- wifi-e.txt
　　Reference manual to use WiFi connection with Raspberry Pi Pico W

[samples] directory
　BASIC sample programs. Copy these files to the root directory of the SD card 
　(or create any subdirectory and copy)

　3DWAVE.BAS
　　3D graphic of ripples

　BLOCK.BAS
　　Block breaking game created for the first MachiKania

　COSMOS.BMP (used in PHOTO.BAS)

　FILEMAN.BAS
　　File manager for MachiKania type P/PU

　HDAEMON.BAS
　　Sample program to build a web server construction using the HTTPD class
　　Connect from PC or smartphone to turn on/off LEDs on Pico W

　INVADE.BAS
　　Invader game created for MachiKania type Z/M/P/PU

　LCHIKA.BAS
　　Sample program to blink LEDs

　MANDELBR.BAS
　　Program to output Mandelbrot in graphic

　MAZE3D.BAS
　　Three-dimensional maze escape game

　MOZART.BAS
　　Playing WAV file sample program

　MOZART.WAV
　　Used by MOZART.BAS

　MUSIC.BAS
　　Sample program using the MUSIC instruction

　NIHONGO.BAS
　　Program to display Japanese language using CKNJ16 class in the library

　PCG.BAS
　　Sample program using PCG

　PEGSOL-G.BAS
　　Peg solitaire game program

　PHOTO.BAS
　　Program to display BMP files on LCD

　RAYTRACE.BAS
　　Ray tracing program

　SOUND.BAS
　　Sample program using the SOUND instruction

　STARTREK.BAS
　　Star Trek game program

　TIME-INT.BAS
　　Sample program for timer interrupt

　WEATHER.BAS
　　Sample program to get information from web pages on the net using WGET and JSON classes
　　Display today's date and weather forecast
　
　WFRAME.BAS
　　Wireframe graphics program


------------------------------------------------------------------------
MachiKania type P BASIC System Revision History

Phyllosoma 1.60/KM-1510 (2025.8.17)
　Added support for ClockworkPi PicoCalc

Pyllosoma 1.61/KM-1511 (2025.12.27)
　Enabled display of statement or function help when using the editor
　Added the RND#() function
　Improved TLS handshake for network connections; files larger than 16 Kb can now be retrieved via HTTPS
　Supported use of PWM4–PWM9, with ports configurable via the INI file
　Fixed behavior of OUT8L/IN8L, OUT8H/IN8H, and OUT16/IN16 statements in PicoCalc, ResTouch, and type PU mini
　Fixed a bug where compilation failed in certain environments when one class used another within class files
　Added FFTLIB class to the class library for performing fast Fourier transform and inverse transform
　Updated WGET in the class library to handle 301 Moved responses
　Corrected class.txt
　Added help-k.txt
