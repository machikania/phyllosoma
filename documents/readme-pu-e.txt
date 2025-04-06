　　　　　　　　　　　　　　　　　　　　　　　　　　2025.4.19
　BASIC Execution Environment Open Platform for Raspberry Pi Pico
　　　　　　　　MachiKania type PU
　　　　　　　　　　　　　　　　　　　　　by KenKen & Katsumi

MachiKania is an open platform for microcontrollers with a BASIC compiler.
MachiKania type PU is equipped with a Raspberry Pi Pico (Pico 2, Pico W) and an 
NTSC video output circuitry to provide a portable BASIC execution environment.
External devices such as general-purpose I/O, SPI, and I2C can be easily 
controlled from BASIC.



The on-board BASIC compiler is KM-BASIC, which is a 32-bit integer BASIC.
It also supports single-precision floating-point arithmetic.
Structured and object-oriented programmings are also available.
BASIC programs can be created on a PC or other device and transferred via 
MMC or SD card, or using a USB cable and dedicated software.
It is also possible to connect a USB keyboard and directly edit and execute 
the program.


For operating circuits and other details, please refer to the following Web site
http://www.ze.em-net.ne.jp/~kenken/machikania/typepu.html (in Japanese)


The included executable file (uf2 file) can be freely used for non-commercial 
use only, free of charge.
It is also possible to publish your work to the public. In this case, 
please mention that you are using the MachiKania series on your website, 
in the instructions, or in any other place.
Please note that we are not responsible for any damage or loss caused 
by the use of the MachiKania series.

<machikania-pu.zip file contents>

[documents] directory
　Stores documents
　- help.txt
　- help-e.txt
　　Reference manual for KM-BASIC

　- class.txt
　- class-e.txt
　　Reference Manual for Object-Oriented Programming in KM-BASIC

　- embed.txt
　- embed-e.txt
　　How to Create File Embedded Executables

　- pcconnect.txt
　- pcconnect-e.txt
　　How to transfer files from your PC to MachiKania

　- shematic_ntsc.png
　- shematic_xiao.png
　　MachiKania type PU schematics

　- wifi.txt
　- wifi-e.txt
　　Reference manual to use WiFi connection with Raspberry Pi Pico W

[pico_ntsc] directory
　Stores binaries when using Raspberry Pi Pico
　- puerulus.uf2
　　MachiKania type PU BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and writes
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　- puerulus_kb.uf2
　　MachiKania type PU BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

[pico_w_ntsc] directory
　Stores binaries when using Raspberry Pi Pico W
　- puerulus.uf2
　　MachiKania type PU BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and writes
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.
　　WiFi connection is available.

　- puerulus_kb.uf2
　　MachiKania type PU BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard
　　WiFi connection is available.

[pico2_ntsc] directory
　Stores binaries when using Raspberry Pi Pico 2
　- puerulus.uf2
　　MachiKania type PU BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and writes
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　- puerulus_kb.uf2
　　MachiKania type PU BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

[pico2_w_ntsc] directory
　Stores binaries when using Raspberry Pi Pico 2 W
　- puerulus.uf2
　　MachiKania type PU BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and writes
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.
　　WiFi connection is available.

　- puerulus_kb.uf2
　　MachiKania type PU BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard
　　WiFi connection is available.

[xiao_ntsc] directory
　Stores binaries when using XIAO-RP2040, TINY2040, or RP2040-Zero
　- puerulus.uf2
　　MachiKania type PU BASIC system (version supporting PC connect function)
　　Connects the PC and micro-contoller board via USB and writes
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　- puerulus_kb.uf2
　　MachiKania type PU BASIC system main unit (USB keyboard connection version)
　　Connects a PC and micro-contoller board via USB cable and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

[xiao_rp2350_ntsc] directory
　Stores binaries when using XIAO-RP2350, TINY2350, or RP2350-Zero
　- puerulus.uf2
　　MachiKania type PU BASIC system (version supporting PC connect function)
　　Connects the PC and micro-contoller board via USB and writes
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　- puerulus_kb.uf2
　　MachiKania type PU BASIC system main unit (USB keyboard connection version)
　　Connects a PC and micro-contoller board via USB cable and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

[pico_ntsc/embed] directory
[pico_w_ntsc/embed] directory
[pico2_ntsc/embed] directory
[pico2_w_ntsc/embed] directory
[xiao_ntsc/embed] directory
[xiao_rp2350_ntsc/embed] directory
　Stores tools that allow you to run BASIC programs without an SD card, 
　e.g. for embedded applications

[pcconnect] directory
　Contains tools for transferring files in a PC to MachiKania via USB 
　cable

readme.txt
　This file

readmeLib.txt
　Library documentation in the LIB directory

<Copy all files and directories below here to the root directory of the SD card>

MACHIKAP.INI
　Initialization file for MachiKania type PU (shared with type P)

[LIB] directory
　Class library that can be easily used from BASIC programs. Copy the entire 
　LIB directory to the root of the SD card and use it

[samples] directory
　BASIC sample programs. Copy these files to the root directory of the 
　SD card (or create any subdirectory and copy)

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
　　Program to output Mandelbrot in text

　MAZE3D.BAS
　　Three-dimensional maze escape game

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
MachiKania type PU BASIC System Revision History

Puerulus 1.50/KM-1507 (2024.12.28)
　First public version

Puerulus 1.51/KM-1508 (2025.2.22)
　Fixed a bug that caused line numbers at the bottom of the screen to be displayed incorrectly when using the editor.
　Changed pin assignments for SPI, I2C, and UART to be freely configurable in MACHIKAP.INI.
　Fixed a problem with REGEXP in the class library that caused a memory error when used continuously.

Phyllosoma 1.52/KM-1509 (2025.4.19)
　Enabled stopping program execution not only with the "Break" key but also with the "Ctrl + Alt + Del" key.
　Made it possible to retrieve the keyboard state using SYSTEM(40), SYSTEM(41), SYSTEM(42), and SYSTEM(43).
　Added functionality to modify the CPU's clock speed and voltage using SYSTEM 50 and SYSTEM 51.
　By holding the SART button during startup, it's now possible to skip the automatic execution of MACHIKAP.BAS upon boot.
　By specifying the second argument in the USEGRAPHIC statement, it is now possible to draw and display using two graphic RAM areas.
　By setting WIDTH80 in MACHIKAP.INI, the system can be configured to display 80 characters horizontally at startup.
　The sample program RAYTRACE.BAS has been modified so that it can be displayed with Type PU.
　Added CPUCLOCK to the class library, making it possible to check available CPU clock frequencies.
　Raspberry Pi Pico 2 W is supported.
　Improved transfer speed when using SPI.
　Update MAZE3D.BAS and STARTREK.BAS sample programs.
　Added POLYGON.BAS sample program.
