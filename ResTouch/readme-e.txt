　　　　　　　　　　　　　　　　　　　　　　　　　　2025.12.27
　BASIC Execution Environment Open Platform for Raspberry Pi Pico
　　　　　　　　MachiKania type P
　　　　　　　　　　　　　　　　　　　　　by KenKen & Katsumi


MachiKania is an open platform for microcontrollers with a BASIC compiler.
MachiKania type P is equipped with a Raspberry Pi Pico (Pico 2, Pico W) and 
a small LCD module to realize a portable BASIC execution environment.
External devices such as general-purpose I/O, SPI, and I2C can be easily 
controlled from BASIC.

This document describes MachiKania for the Waveshare Pico-ResTouch-LCD-3.5 
(hereafter referred to as ResTouch).


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

When using the ResTouch-compatible version of MachiKania, please note the 
following points.

<Supported Pi Pico boards>

It supports four models: Raspberry Pi Pico, Raspberry Pi Pico 2, Raspberry Pi 
Pico W, and Raspberry Pi Pico 2 W. Please install the appropriate phyllosoma_kb.uf2 
file for your board from the distributed archive. For WiFi-compatible versions, 
connecting to the internet via WiFi is also possible.

To install the .uf2 file, follow these steps:
  1. Turn off the power to the ResTouch (unplug the USB micro B cable).
  2. Flip the ResTouch over, then while pressing and holding the“BOOTSEL” 
     button on the Raspberry Pi Pico board, connect it to your PC using a USB 
     micro B cable.
  3. A drive named RPI-RP2 or RP2350 will appear on your PC. Drag and drop the 
     phyllosoma_kb.uf2 file into that drive.


<MACHIKAP.INI>

To ensure that MachiKania works properly on ResTouch, make sure to place the 
included MACHIKAP.INI file in the root directory of the MMC/SD card.


<Keyboard>

For programs that do not use the touch panel, the keyboard is the only input 
device. Please connect a USB keyboard using a USB-OTG cable. Be sure to use 
a Micro-B USB-OTG cable with power input.

For general use, please install phyllosoma_kb.uf2, which supports USB keyboards. 
The phyllosoma.uf2 file is intended for special use cases such as operating 
solely with the touch panel or communicating with a PC via serial connection, 
without using a USB keyboard.


<Button switches>

MachiKania is operated using six buttons: Up, Down, Left, Right, Fire, and 
Start. Since ResTouch does not have these physical buttons, they are emulated 
via the keyboard. By default, the arrow keys correspond to directional controls, 
the "F" key to Fire, and the "S" key to Start. If you wish to change these 
key mappings, please edit the MACHIKAP.INI file. Settings like "EMULATEBUTTONUP=" 
control these mappings. The value on the right side represents the virtual key 
code in decimal format. To find the virtual key code for a specific key, run the 
BASIC program below and press the desired key.

  DO:PRINT INKEY():LOOP


<Stopping a BASIC program mid-execution>

To stop a BASIC program while it is running, press the Pause key or the Ctrl + Alt + Del keys 
simultaneously. This will interrupt the program. However, please note that in certain 
loops (such as those without any PRINT commands) the program may not stop as expected.


<Touch Panel>

When using the touch panel, please use the TSC2046 class. During class initialization, 
specify ports 9 and 10 in the INIT() method as shown below:

  TSC2046::INIT(9,10)


<LCD Backlight>

The LCD backlight brightness can be controlled via I/O bit 8 or PWM1. For example:

  PWM 200

This will dim the display (specify a value between 0–1000). To restore the original brightness:

  PWM 1000

You can also turn the backlight on (OUT 8,1) or off (OUT 8,0) using "OUT" statement.


<Receiving Data from the LCD>

Although ResTouch can send data to the LCD, it cannot receive data from it. 
As a result, certain graphic commands—such as the GCOLOR function—are not 
supported. For example, the sample program INVADE.BAS, which uses this feature, 
does not operate correctly.


<Audio Output>

ResTouch does not have a built-in audio output device. Therefore, in order to run 
programs that output sound, you need to connect an external audio device (such as 
a piezo buzzer or headphone jack) to ResTouch. Audio from MachiKania is output 
through GP27 on the Raspberry Pi Pico, so please use this pin along with the GND pin.


<machikania-rt.zip file contents>

[pico] directory
　Stores binaries used with Raspberry Pi Pico
　- phyllosoma.uf2
　　MachiKania type P BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and transfer the uf2 file
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

[pico_w] directory
　Stores binaries used with Raspberry Pi Pico W
　- phyllosoma.uf2
　　MachiKania type P BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and transfer the uf2 file
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.
　　WiFi connection is available.

　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard
　　WiFi connection is available.

[pico2] directory
　Stores binaries used with Raspberry Pi Pico 2
　- phyllosoma.uf2
　　MachiKania type P BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and transfer the uf2 file
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

[pico2_w] directory
　Stores binaries used with Raspberry Pi Pico 2 W
　- phyllosoma.uf2
　　MachiKania type P BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and transfer the uf2 file
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.
　　WiFi connection is available.

　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard
　　WiFi connection is available.

[pcconnect] directory
　Contains tools for transferring files in a PC to MachiKania via USB cable

readme-e.txt
　This file

readmeLib-e.txt
　Library documentation in the LIB directory

<Copy all files and directories below here to the root directory of the MMC/SD card>

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
　　How to connect and use a keyboard

　- pcconnect.txt
　- pcconnect-e.txt
　　How to transfer files from your PC to MachiKania

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
　Added support for Waveshare ResTouch LCD 3.5

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
