# Stage 1

The first stage is compiling and installing GNU Binutils. This provides tools
such as an assembler and a linker, and is needed for compiling GCC. These will
target the Sega Mega Drive's CPU; the Motorola 68000.

To begin, install Patch, GNU Make, GNU Diffutils, Texinfo, Flex, Bison, and GCC
(or Clang). These will be required to compile GNU Binutils and GCC. For MSYS2,
the package names are...
- `patch`
- `make`
- `mingw-w64-[environment goes here]-diffutils`
- `mingw-w64-[environment goes here]-texinfo`
- `flex`
- `bison`
- `mingw-w64-[environment goes here]-gcc` (for GCC)
- `mingw-w64-[environment goes here]-clang` (for Clang)

Linux distributions will have different package names.

Next, download GNU Binutils 2.43. It can be found at the following URL:
https://ftp.gnu.org/gnu/binutils/binutils-2.43.tar.xz

After that, extract the `binutils-2.43` directory to here, and then run the
`binutils.sh` script. Upon its completion, GNU Binutils will be compiled and
installed to `/opt/clownmdsdk` and Stage 2 can begin.
