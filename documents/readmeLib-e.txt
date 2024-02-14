<About the MachiKania library>

The LIB directory contains several sub-directories, each of which contains class files. This will allow you to use the USECLASS statement. This will allow you to use these classes by declaring them with the USECLASS statement.

The following is a (possibly incomplete) list of classes included in this library. Please refer to the respective documentation (e.g., help.txt) for information on how to use each class.

BIGNUM
	Class for performing long multiple arithmetic operations up to 1000 decimal places.

BUTTON
	Class to make it easier to use the UP, DOWN, LEFT, RIGHT, START, and FIRE buttons.

C24LC
	Class for reading and writing EEPROM 24LC512.

CIOEX
	Class for controlling I/O expander, MCP23017.

CKNJ8
	Class for Japanese display. Uses Misaki font (8x8).

CKNJ12
	Japanese display class. Use Shinonome font (12x12).

CKNJ16
	Japanese display class. Use Shinonome font (16x16).

CLDHEX
	Class for capturing HEX file contents into memory.

CRDINI
	Class for examining MachiKania INI files (MACHIKAP.INI, MACHIKAM.INI, etc.).

CSWTIF
	Class for displaying TIFF images.

GEN3O
	Class for displaying Genno Kaku Gothic, a large font of 29 to 118 pixels, included in machikap-p2-xxx.zip.

HTTPD
	Class to construct http server

IR_RX
	Class to use IR receiver

IR_TX
	Class to use IR transmitter

JSON
	Class to analyze JSON string

MA
	Class for memory allocation, used in STRDIM.

QRCODE
	Class for displaying QRCODE on the screen.

REGEXP
	Class for using regular expressions with MachiKania type P.

STRD2
	Class for using arrays containing strings (used when defining arrays containing many short strings).

STRDIM
	Class for using arrays containing strings (used when defining arrays containing long or variable length strings).

STRING
	Class for handling string objects, similar to Java's String class.

TSC2046
	Class for retrieving information from LCD touch panel (TSC2046).

WGET
	Class for retrieving information from a specified URL as a client when connecting to Wifi.

WS2812B
	Class for using serial connection LED WS2812B.

------------------------------------------------------------------------
MachiKania type P Class Library Revision History

Phyllosoma 1.00/KM-1500 (2022.8.27)
　First public version
　Including BIGNUM, C24LC, CIOEX, CKNJ8, CKNJ12, CKNJ16, CSWTIF

Phyllosoma 1.10/KM-1501 (2022.10.1)
　MA, STRDIM,STRD2, and WS2812B are added.
　CSWTIF ver 0.3

Phyllosoma 1.20/KM-1502 (January 28, 2023)
　CLDHEX, CRDINI, REGEXP, and STRING are added.

Phyllosoma 1.30/KM-1503 (2023.9.30)
　HTTPD, IR_RX, IR_TX, JSON, and WGET are added
　CKNJ8 ver 0.4
　CKNJ12 ver 0.4
　CKNJ16 ver 0.4

Phyllosoma 1.31/KM-1504 (2023.10.28)
　No change

Phyllosoma 1.40/KM-1505 (2024.2.17)
　BUTTON, QRCODE, TS2046 added
　CKNJ8 ver 0.5
　CKNJ12 ver 0.5
　CKNJ16 ver 0.5
　HTTPD ver 0.3.2
