# Coding notes/findings

## Printf Floats
Cannot printf floats by default, says `<NO FLOAT>`. See Compiler support routines of the [SDCC manual](docs/sdccman.pdf) for details. To enable add `-DUSE_FLOATS=1` to command line flags.
https://community.platformio.org/t/how-does-platformio-link-duse-floats-1/28458/14

## Alternate function remapping (Option byte programming)
To access the alternate pin functions you need to program the option byte accordingly. E.g. we use I2C on pins 16 & 17 which is their alternate function.

Option byte is in EEPROM and should be programmed a minimal number of times. You can use STVP as per [this guide](https://documentation.help/STM8-FlashTool-STVP/Prog0122.htm).
