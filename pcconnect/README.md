# phyllosoma
MachiKania Phyllosoma

## Connection between PC and MachiKania Phyllosoma
The PC (Windows and Linux are confirmed currently) can be connected to MachiKania by USB carble for transferring files from PC to MachiKania.

## connect.c
The connect.c is the source code of appication running in PC for the connection between PC and MachiKania.

## How to compile connect.c
Use gcc. In windows, CygWin is required.

## How to use
1.Place "connect.ini" file in the same directory where a.out or a.exe exists  
2.Create "machikap" directory in the same directory.  
3.Place the files (for example, MACHIKAP.BAS) to be transfered to MachiKania in the machikap directory.  
4.Edit the files in machikap directory for your purpose.  
5.Run a.out or a.exe.  
6.Reset MachiKania to transfer the files.  

## Settings
Edit "connect.ini" file for configuration. It is necessary to specify the device to connect (for example, /dev/ttyS2, /dev/ttyACM0).

When connection between PC and MachiKania is unstable, prolong the waiting time of MachiKania in the beginning by editing MACHIKAP.INI in MMC/SD card (STARTWAIT=xxxx).