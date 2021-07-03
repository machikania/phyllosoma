# kmbasic4arm
KM-BASIC for ARM

## KM-BASIC for ARM
KM-BASIC for ARM is a BASIC compiler for ARMv6-M, especially for Raspberry Pi Pico.

## how to compile
Copy kmbasic4arm directory in pico-example directory of sdk. Edit CMakeLists.txt and add "add_subdirectory(kmbasic4arm)" at the end of file. Then cmake and make.

## how to use
Copy "kmbasic.uf2" to the RPI-RP2 drive of Raspberry Pi Pico. Immediately connect to COMx (com number depends on environment) by serial console at 115200 baud rate.

## License
Most of codes (written in C) are provided with LGPL 2.1 license, but some codes are provided with the other licenses. See the comment of each file.
