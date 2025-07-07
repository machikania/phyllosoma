　　　　　　　　　　　　　　　　　　　　　　　　　　2025.xx.xx
　BASIC Execution Environment Open Platform for Raspberry Pi Pico
　　　　　　　　MachiKania type P
　　　　　　　　　　　　　　　　　　　　　by KenKen & Katsumi


MachiKania is an open platform for microcontrollers with a BASIC compiler.
MachiKania type P is equipped with a Raspberry Pi Pico (Pico 2, Pico W) and 
a small LCD module to realize a portable BASIC execution environment.
External devices such as general-purpose I/O, SPI, and I2C can be easily 
controlled from BASIC.


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

To stop a BASIC program while it is running, press the Ctrl + Alt + Del keys 
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


<machikania-rt.zip file contents>

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
　Raspberry Pi Pico W is supported.
　WEATHER.BAS (get weather forecast from web), HDEAMON.BAS (construct web server) were added as a sample program.
　Added HTTPD (construct HTTP deamon), IR_RX (IR receiver), IR_TX (IR transmitter), JSON (analyze JSON string) and WGET (web browsing) to the class library.
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

Phyllosoma 1.31/KM-1504 (2023.10.28)
　The BASIC program for embed now supports file reading.
　The number of files that can be embedded for embedding has been increased to 16.
　Fixed a bug that caused wrong values when displaying floating points and handling strings.

Phyllosoma 1.40/KM-1505 (2024.2.17)
　FILEMAN.BAS (file manager) was added as a sample program.
　Added BUTTON (button operation), GEN3O (Genkaku Gothic display), QRCODE (QRCODE display), 
and TSC2046 (touch panel operation) to the class library.
　Enabled to use spi1 with SPI instruction when LCD is not used.
　Fixed a bug in PUTBMP instruction when displaying a long width image.
　The file modification date and time can be displayed in the file list view.
　The display order of file names and modified dates in the file list display is now supported.
　Fixed a display bug on ILI9488 LCD when it is displayed in portrait mode.
　The FRENAME and MKDIR instructions and the FRENAME() and MKDIR() functions have been added.
　Auxiliary code (auxcode) can be added.

Phyllosoma 1.41/KM-1506 (2024.10.06)
　Support Raspberry Pi Pico 2.
　Update WEATHER.BAS sample program.

Phyllosoma 1.50/KM-1507 (2024.12.28)
　Update BLOCK.BAS and PHOTO.BAS sample programs.
　Update WS2812B and QRCODE in class library to support using for Pico 2.
　Fixed a bug in the file selection function of the text editor.
　Changed so that separate class files can be handled for Pico/Pico2 or Type P/PU.
　Fix SYSTEM(4) to return correct CPU execution speed.
　Added SYSTEM(6) to obtain the size of BASIC's object RAM area.
　When using the ILI9488 LCD, the width that could not be specified before can now be specified with the WIDTH statement.
　When a WAIT statement is being executed, execution can be stopped by pressing the PAUSE/BREAK key on the keyboard.

Phyllosoma 1.51/KM-1508 (2025.2.22)
　Changed pin assignments for SPI, I2C, and UART to be freely configurable in MACHIKAP.INI.
　Distribution of XIAO embed version was terminated due to the availability of MACHIKAP.INI to configure pin assignments.
　Fixed a problem with REGEXP in the class library that caused a memory error when used continuously.

Phyllosoma 1.52/KM-1509 (2025.4.12)
　Enabled stopping program execution not only with the "Break" key but also with the "Ctrl + Alt + Del" key.
　Made it possible to retrieve the keyboard state using SYSTEM(40), SYSTEM(41), SYSTEM(42), and SYSTEM(43).
　Added functionality to modify the CPU's clock speed and voltage using SYSTEM 50 and SYSTEM 51.
　By holding the STAR button during startup, it's now possible to skip the automatic execution of MACHIKAP.BAS upon boot.
　Added CPUCLOCK to the class library, making it possible to check available CPU clock frequencies.
　Raspberry Pi Pico 2 W is supported.
　Improved LCD transfer speed when using Raspberry Pi Pico 2.
　Update MAZE3D.BAS and STARTREK.BAS sample programs.
　Added POLYGON.BAS sample program.
