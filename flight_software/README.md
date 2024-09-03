# Debugger
Debug is available via [openocd](https://openocd.org/). Various windows install methods, I used [Chocolatey](https://community.chocolatey.org/packages/openocd). This installs it in `C:\ProgramData\chocolatey\lib`.

Openocd uses .cfg files to specify how to use boards (e.g. stm nucleo), targets (e.g. stm8s or even specific chips) and interfaces (e.g. stlink). These .cfg files are human readable and installed in the scripts folder of openocd, 
`C:\ProgramData\chocolatey\lib\openocd\tools\install\share\openocd\scripts`. See the [readme](https://openocd.org/doc-release/README) for more details.


`openocd -f interface/stlink-dap.cfg -f target/stm8s.cfg`

https://packages.msys2.org/package/mingw-w64-x86_64-gdb-multiarch?repo=mingw64
Needs mysys2 bin dir on path

# Coding notes/findings

## Printf Floats
Cannot printf floats by default, says `<NO FLOAT>`. See Compiler support routines of the [SDCC manual](docs/sdccman.pdf) for details. To enable add `-DUSE_FLOATS=1` to command line flags.
https://community.platformio.org/t/how-does-platformio-link-duse-floats-1/28458/14

## Alternate function remapping (Option byte programming)
To access the alternate pin functions you need to program the option byte accordingly. E.g. we use I2C on pins 16 & 17 which is their alternate function.

Option byte is in EEPROM and should be programmed a minimal number of times. You can use STVP as per [this guide](https://documentation.help/STM8-FlashTool-STVP/Prog0122.htm).

## Int type width only 2 bytes
On 32 bit systems ints are typically 4 bytes (STM32) and also in 64-bit AIX and Linux applications. In SDCC it is only 2 bytes (same as AVR actually) as per section 1.1 of the [SDCC manual](docs/sdccman.pdf). Therefore code such as `uint32_t comparison = 1<<(length - 1);` does not work as expected for length>16. This is because the 1 defaults to an int, and trying to leftshift it more than 16 bits is outside its width. You must specify the 1 as a 32 bit int, e.g. `uint32_t comparison = (uint32_t)1<<(length - 1);`

Lesson is to make code portable between different devices you shouldn't rely on the type widths being the same. And when taking code from forums/gpt, try specifying the compiler. Looking/Asking for C code is not enough.

## Interrupts
You need to match your interrupt handler function to the interrupt vector, so that your ISR actually gets called when the interrupt happens. This is achieved using the IRQ no assigned to the interrupt in the Interrupt Vector Mapping table of thhe STM8S007C8 Datasheet. 

E.g. UART3 RX full is number 20 with vector address 0x008058

## SD Card init/working
SD cards work based on [commands](https://chlazza.nfshost.com/sdcardinfo.html). Must send in certain order to init and expect correct responses. Demo [here](http://www.rjhcoding.com/avrc-sd-interface-3.php) but like [this guy](https://stackoverflow.com/questions/8080718/sdhc-microsd-card-and-spi-initialization) I had to also remove the CMD55 and set HCS bit in CMD1. Might be wrong.