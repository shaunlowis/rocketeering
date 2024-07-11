# rocketeering
Documentation, progress and updates added [here](https://shaunlowis.github.io/rocketeering/)

NOTE: This guide, code, information and its authors assume no liability for outcomes or results of its use or distribution. All code and information is provided as-is for education purposes.

# Setup

Download appropriate version from [here](https://sourceforge.net/projects/sdcc/)


## Ubuntu

If on ubuntu, you should already have gcc and some form of python.

```
cd path-to-download

tar xjf sdcc-<some-version>.tar.bz

cd sdcc-<some-version>

# this will contain another file with the same name, move to that:
cd sdcc-<some-version>

# then run:
sudo ./configure

# you will probably have some missing dependencies, which I just installed with:
sudo apt install bison
sudo apt install flex
sudo apt install gputils

# repeat above until all requirements are met
# this will take quite a long time
make

# slightly faster
sudo make install

# if all went well:
sdcc -v

# which outputs something like:
SDCC : mcs51/z80/z180/r2k/r2ka/r3ka/sm83/tlcs90/ez80_z80/z80n/r800/ds390/pic16/pic14/TININative/ds400/hc08/s08/stm8/pdk13/pdk14/pdk15/mos6502/mos65c02 TD- 4.4.0 #14620 (Linux)
published under GNU General Public License (GPL)
```

Note; semi-useful guide on SDCC, see [here](https://martyn.welchs.me.uk/posts/installing-and-using-sdcc-on-linux/).

Now we need to install the STM8 flash setup. I assume you have an ST-Link debugger, with a SWIM capable interface.
```
# wherever you want the repo to be
git clone https://github.com/vdudouyt/stm8flash.git

cd stm8flash

# dependencies:
sudo apt-get install pkg-config libusb-1.0-0-dev

make

sudo make install
```

### Download standard library
Source files for the STM8S Standard Peripheral Library are available from ST,
but it is more recommended to get the patched files from [here](https://github.com/roshbaby/stm8s-sdcc). Some further adjustments are required, depending on the
specific STM8 you have.

### Wiring the STLINK
We are using the STLINK-V2. The STM8 micro has to be debugged over SWIM.

Details for this are in section 7.3.3 [here](https://www.st.com/resource/en/user_manual/um2448-stlinkv3set-debuggerprogrammer-for-stm8-and-stm32-stmicroelectronics.pdf)

For debugging, connect SWO to the SWIM breakout header on the schematic and NRST TO NRST.

You may need to remove writing protection:

```
echo "00 00 ff 00 ff 00 ff 00 ff 00 ff" | xxd -r -p > factory_defaults.bin
stm8flash -c stlinkv2 -p stm8s007c8 -s opt -w factory_defaults.bin
```

## Flashing your first test program onto the STM8

In the `/flight_software` directory, simply do a `make flash` and if all goes well,
the compiled `main.c` file should be flashed onto your microcontroller.

![](img/fc_blink.gif)

Next up: make a makefile to first compile the stm8s_gpio.h and stm8s_gpio.c and 

## VS Code

Extensions:

- C/C++ for Visual Studio Code
- Hex Editor
- vscode-devdocs

## Possibly a much better setup:
https://github.com/bschwand/STM8-SPL-SDCC