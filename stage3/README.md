# Stage 3

This stage covers building and installing Sega Mega Drive-specific headers,
scripts, and libraries.


## Files

### Scripts
- `bare.mk` - A Makefile script that defines essential compiler/linker flags
  and makes the Makefile that invoked it use the toolchain. By including this
  file, a Makefile can be made to produce bare-metal code that runs on the
  Sega Mega Drive. This is used for compiling the libraries.
- `rom.mk` - A Makefile script that provides additional linker flags for
  producing executables. By including this file, a Makefile can be made to
  produce Sega Mega Drive executables (A.K.A. "ROMs").
- `rom.ld` - A linker script that defines the Sega Mega Drive's basic memory
  layout and links libraries for performing bootstrapping.

### Headers
- `assert.h`, `cassert` - Implementation of the `assert` macro.
- `stdlib.h`, `cstdlib` - Declarations for `abs`, `memcpy`, and `memset`. 
- `clownmdsdk.h` - Helper functions for accessing the Sega Mega Drive's
  hardware.

### Libraries
- `init.s` - Bootstrapper that initialises the hardware and runs the global
  constructors.


## Building

To build and install these, first run `src/z80/build.lua` (or `build.bat` if
using Windows), then run `misc.sh`.

With that done, clownmdsdk is completely installed and ready for use. The
toolchain can be tested by compiling the example program in the `example`
directory.
