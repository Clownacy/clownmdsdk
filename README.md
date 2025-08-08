# ClownMDSDK

ClownMDSDK is a toolchain for developing Sega Mega Drive/Sega Genesis homebrew
in freestanding C++. The toolchain includes GNU Binutils, GCC, and some
essential libraries and scripts. Convenience functions are provided for
performing low-level tasks such as writing to the VDP's registers.

Unlike SGDK, this is explicitly NOT a framework nor game engine: this project's
sole concern is enabling the creation of bare-metal Mega Drive software using
C++; handling controller inputs, playing sounds, and displaying graphics are all
tasks which are left to the user.


## Building

Building requires a Unix-like environment. Linux users should be able to build
the toolchain natively, while Windows users should install and use MSYS2 or
WSL.

The process of building and installing the toolchain is done in four stages,
denoted by the four subdirectories. See each of the subdirectories' README
files for details. After all stages are complete, the toolchain will be
completely installed and ready for use.


## Using

The toolchain will be installed at `/opt/clownmdsdk`. The simplest way to use
the toolchain is with a Makefile: by adding the line
` include /opt/clownmdsdk/cartridge.mk` to the start, the `CC`, `CXX`, `CPP`,
`AS`, `CFLAGS`, `CXXFLAGS`, `LDFLAGS` variables will all be configured to use
the toolchain, and the rest of the Makefile can be written as it would be for
any other platform.


## Examples

Blank template projects and examples which showcase using ClownMDSDK can be
found in the `examples` directory.


## Licence

Everything in this repository is released under the 0BSD licence.
See `LICENCE.txt` for details.
