　　　　　　　　　　　　　　　　　　　　　　　　　　2025.11.23
　BASIC Execution Environment Open Platform for Raspberry Pi Pico
　　　　　　　　MachiKania type P
　　　　　　　　　　　　　　　　　　　　　by KenKen & Katsumi


MachiKania is an open platform for microcontrollers with a BASIC compiler.
MachiKania type P is equipped with a Raspberry Pi Pico (Pico 2, Pico W) and 
a small LCD module to realize a portable BASIC execution environment.
External devices such as general-purpose I/O, SPI, and I2C can be easily 
controlled from BASIC.


This document describes MachiKania for the Waveshare RP2350-LCD-1.47 (hereafter
 referred to as RP2350-LCD-1.47).

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

When using the RP2350-LCD-1.47-compatible version of MachiKania, please note the 
following points.


<MACHIKAP.INI>

To ensure that MachiKania works properly on RP2350-LCD-1.47, make sure to place the 
included MACHIKAP.INI file in the root directory of the MMC/SD card.


<Keyboard>

For programs that do not use the gpio input(s), the keyboard is the only input 
device. Please connect a USB keyboard using a USB-OTG cable. Be sure to use 
a Type-C USB-OTG cable with power input. Additionally, power can also be supplied 
from the 5V pin and GND pin (pins 10 and 11) of the RP2350-LCD-1.47.

For general use, please install phyllosoma_kb.uf2, which supports USB keyboards. 
The phyllosoma.uf2 file is intended for special use cases such as operating 
solely without the input or communicating with a PC via serial connection, 
without using a USB keyboard.


<Button switches>

MachiKania is operated using six buttons: Up, Down, Left, Right, Fire, and 
Start. Since RP2350-LCD-1.47 does not have these physical buttons, they are emulated 
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


<LCD>

Please note that the RP2350-LCD-1.47's LCD has rounded corners, so the display is missing
at the four corners.


<machikania-rl.zip file contents>

[rp2350_lcd_1_47] directory
　Stores binaries used with Raspberry Pi Pico 2
　- phyllosoma.uf2
　　MachiKania type P BASIC system (version supporting PC connect function)
　　Connects the PC and RP2350-LCD-1.47 via USB and transfer the uf2 file
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection version)
　　Connects a PC and RP2350-LCD-1.47 via USB cable and transfer the uf2 file
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

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

　WFRAME.BAS
　　Wireframe graphics program


------------------------------------------------------------------------
MachiKania type P BASIC System Revision History

Pyllosoma 1.61/KM-1511 (2025.12.27)
　Added support for Waveshare RP2350-LCD-1.47
