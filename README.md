KreyosFirmware
==============

Build Instruction:
1. The build tool is IAR MSP-430 5.50 
2. You need a workable unix tool in path, like make. Suggest to install msys/mingw32
3. run make -f Makefile.msp430, a watch.txt will be generated @objs.msp430.
4. use tool/convertbin to convert txt file to a bin file that is able to upload through phone application


Flashing instructions (by hawpaw)
1. The folder "Firmware" contain the tools to perform the watch flashing
2. The "Documents" folder contain the file:
	InstructionalGuideforFlashingFirmwareviaUSBCradle.pdf 
	with some explanations for this purpose.