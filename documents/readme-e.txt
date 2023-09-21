　　　　　　　　　　　　　　　　　　　　　　　　　　2023.9.30
　BASIC Execution Environment Open Platform for Raspberry Pi Pico
　　　　　　　　MachiKania type P
　　　　　　　　　　　　　　　　　　　　　by KenKen & Katsumi

MachiKania is an open platform for microcontrollers with a BASIC compiler.
MachiKania type P is equipped with a Raspberry Pi Pico and a small LCD module to realize a portable BASIC execution environment.
External devices such as general-purpose I/O, SPI, and I2C can be easily controlled from BASIC.

The on-board BASIC compiler is KM-BASIC, which is a 32-bit integer BASIC.
It also supports single-precision floating-point arithmetic.
Structured programming and object-oriented support are also available.
BASIC programs can be created on a PC or other device and transferred via MMC or SD card, or using a USB cable and dedicated software.
It is also possible to connect a USB keyboard and directly edit and execute the program.

For operating circuits and other details, please refer to the following Web site
http://www.ze.em-net.ne.jp/~kenken/machikania/typep.html


The included executable file (uf2 file) can be freely used for non-commercial use only, free of charge.
It is also possible to publish your work to the public. In this case, please mention that you are using the MachiKania series on your website, in the instructions, or in any other place.
Please note that we are not responsible for any damage or loss caused by the use of the MachiKania series.

<machikania-p.zip file contents

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

　- shematic.png
　　MachiKania type P schematic

　- wifi.txt
　- wifi-e.txt
　　Reference manual to use WiFi connection with Raspberry Pi Pico W

[pico_ili9341] directory
　- phyllosoma.uf2
　　MachiKania type P BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and writes
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

[pico_w_ili9341] directory
　- phyllosoma.uf2
　　MachiKania type P BASIC system (version supporting PC connect function)
　　Connects the PC and Raspberry Pi Pico via USB and writes
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.
　　WiFi connection is available.

　- phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB cable and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard
　　WiFi connection is available.

[pico_ili9341/embed] directory
[pico_w_ili9341/embed] directory
　Stores tools that allow you to run BASIC programs without an SD card, e.g. for embedded applications

[pcconnect] directory
　Contains tools for transferring files in a PC to MachiKania via USB cable

readme.txt
　This file

readmeLib.txt
　Library documentation in the LIB directory

<Copy all files and directories below here to the root directory of the SD card

MACHIKAP.INI
　Initialization file for MachiKania type P

[LIB] directory
　Class library that can be easily used from BASIC programs Copy the entire LIB directory to the root of the SD card and use it

[samples] directory
　BASIC sample programs. Copy these files to the root directory of the SD card (or create any subdirectory and copy)

　3DWAVE.BAS
　　3D graphic of ripples

　BLOCK.BAS
　　Block breaking game created for the first MachiKania

　INVADE.BAS
　　Invader game created for MachiKania type Z

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

　STARTREK.BAS
　　Star Trek game program

　SOUND.BAS
　　Sample program using the SOUND instruction

　TIME-INT.BAS
　　Sample program for timer interrupt

　WFRAME.BAS
　　Wireframe graphics program

　WEATHER.BAS
　　Sample program to get information from web pages on the net using WGET and JSON classes
　　Display today's date and weather forecast
　
　HDAEMON.BAS
　　Sample program to build a web server construction using the HTTPD class
　　Connect from PC or smartphone to turn on/off LEDs on Pico W

　COSMOS.BMP (used in PHOTO.BAS)


------------------------------------------------------------------------
MachiKania type P BASIC System Revision History

Phyllosoma 1.00/KM-1500 (2022.8.27)
　First public version

Phyllosoma 1.10/KM-1501 (2022.10.1)
　Added PC connect function
　Added NOT#() function
　Fixed a bug that prevented compiling when another class is used in a class.
　Fixed a bug in static function call of a class.
　Allowed assignment of strings and arrays to object fields.
　Fixed a bug that WAVE files stop playing after a certain period of time.
　Allowed WAIT, DELAYMS, and DELAYUS statements to be used during interruptions
　Fixed a bug that MUSIC statement stopped with an error in some environments.
　Improved the display of the file selection screen.
　Four classes, WS2812B, STRDIM, STRD2, and MA, have been added to the library.

Phyllosoma 1.20/KM-1502 (2023.1.28)
　USB keyboard and editor support
　Added INKEY(), READKEY(), and INPUT$() functions and INKEY interrupt function.
　Fixed a bug that occurred in some environments when compiling class files.
　Added ALIGN4 statement, DATAADDRESS() function, and FUNCADDRESS() function.
　Fixed a slight timing discrepancy in timer interrupt.
　Added memory allocation related functions to SYSTEM().
　Added a function related to memory allocation to SYSTEM().
　Supported OPTION CLASSCODE.
　Card can be inserted and removed during BASIC program execution.
　HEX files created in C language can be loaded on the file selection screen.
　Four classes, CRDINI, CLDHEX, REGEXP, and STRING, are added to the library.

Phyllosoma 1.30/KM-1503 (2023.9.30)
　Added support for WiFi connection using Raspberry Pi Pico W. Following statements/functions are added: DNS$(), IFCONFIG$(), NTP, TCPACCEPT(), TCPCLIENT, TCPCLOSE, TCPRECEIVE, TCPSEND, TCPSERVER, TCPSTATUS, TLSCLIENT, WIFIERR(), WIFIERR$()
　Fixed a bug in GCOLOR() function.
　Fixed a key input bug with some USB keyboards.
　FGETC() function returns -1 when EOF.
　Supported XIAO RP2040, RP2040-Zero, and Tiny-2040 for embedding.
　SPI ports can now be specified in the INI file.
　Fixed a bug when the third argument of SERIAL statement is omitted.
　Exceptions are now trapped and displayed on the screen.
　Fixed a bug that CORETIMER starts to fail about 2000 seconds after power-on.
　Support ILI9488.
　Support RTC(Real Time Clock). Follosing statement/functions are added: GETTIME$(), SETTIME. STRFTIME$()
　Supports date/time setting when saving files.
　Supports RTC setting by NTP server.
　Supports turning on/off the LEDs attached to the board by calling SYSTEM 201.
　Fixed a memory allocation bug.
　Fixed a garbage collection bug when using file-related commands such as FREMOVE/SETDIR.
　Improved stability of the WAVE player.
　Fixed a static method call bug in a class.
　Three functions, FFINGD$(), FINFO(), and FINFO$(), are added. Enabled to create file list.
　Fixed a compile-time bug when the REM statement contains double quotation.
