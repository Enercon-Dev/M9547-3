..\..\..\Scripts\srec_cat.exe M9547.bin -binary -CRC16_Little_Endian -maximum-addr M9547.bin -binary -POLY ccitt -XMODEM -o M9547_crc.bin -binary
..\..\..\Scripts\srec_cat.exe M9547.hex -intel -offset -0x08000000 ..\..\..\BootLoader\CUBEIDE\Debug\BootLoader.hex -intel -offset -0x08000000 -o M9547_boot.bin -binary



