# Setup

Download appropriate version from [here](https://sourceforge.net/projects/sdcc/)

NOTE: This guide assumes no liability for whatever the sdcc install toolchain does.

## Ubuntu
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
```
Note; got up to installing requirements under Installing SDCC, see [here](https://martyn.welchs.me.uk/posts/installing-and-using-sdcc-on-linux/), 

## VS Code

Extensions:

- C/C++ for Visual Studio Code
- Hex Editor
- vscode-devdocs