The included executable file (uf2 file) can be freely used for non-commercial use only
You can freely use the included executable file (uf2 file) for non-commercial use only.
You may also publish your work to the general public. In that case,
Please mention that you are using the MachiKania series on your website or
Please note that any damage or injury resulting from the use of the MachiKania series must be paid for by the user.
We are not responsible for any damage or loss caused by the use of this software.


<machikania-p2.zip file contents>

[LIB] directory
　Contains additinal class library
　Stores the
　-[GEN3O]
　　Class to show Source Han Sans font. In addition to GEN3O.BAS and help.txt,
　　this directory contains files required for execution of the GEN3O class. 

[pico_ili9488] directory
　Contains binaries used when combining Raspberry Pi Pico and ILI9488 LCD
　Stores the
　-phyllosoma.uf2
　　MachiKania type P BASIC system main unit (PC connect function supported version)
　　Connecting a PC and Raspberry Pi Pico via USB and writing
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable.

　-phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection version)
　　Connects a PC and Raspberry Pi Pico via USB and writes
　　Directly edit and execute BASIC programs with the built-in editor and USB keyboard

[pico_w_ili9488] directory
　Stores binaries used when using Raspberry Pi Pico W with ILI9488 LCD
　Stores the
　-phyllosoma.uf2
　　MachiKania type P BASIC system main unit (PC connect function supported version)
　　Connecting a PC and Raspberry Pi Pico via USB and writing
　　PC connect function allows BASIC programs to be transferred from a PC connected via USB cable
　　WiFi connection is available

　-phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection compatible version)
　　Connects a PC and Raspberry Pi Pico via USB and writes
　　Built-in editor and USB keyboard allow direct editing and execution of BASIC programs
　　WiFi connection available

[pico_ili9488/embed] directory
[pico_w_ili9488/embed] directory
　Stores tools to run BASIC programs without MMC/SD card for embedded applications.
　tools to run BASIC programs without an MMC/SD card, such as for embedded applications.

[xiao_embed] directory
　Stores binaries used for embedding in Seeed XIAO RP2040.
　-phyllosoma.uf2
　　MachiKania type P BASIC system (without USB keyboard connection)
　　USB connection between PC and Seeed XIAO RP2040 for writing
　　Embed MACHIKAP.BAS etc. by convert.php

　-phyllosoma_kb.uf2
　　MachiKania type P BASIC system main unit (USB keyboard connection compatible version)
　　USB connection between PC and Seeed XIAO RP2040 for writing
　　Embed MACHIKAP.BAS etc. by convert.php
　　USB keyboard can be used in BASIC program