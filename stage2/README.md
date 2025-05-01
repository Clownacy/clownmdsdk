# Stage 2

This stage is compiling and installing GCC. This provides the compiler itself,
which is needed for compiling the libraries in Stage 3. Like the assembler and
linker, the compiler will target the Motorola 68000.

Download GCC 15.1.0. It can be found at the following URL:
https://ftp.gnu.org/gnu/gcc/gcc-15.1.0/gcc-15.1.0.tar.xz

After that, extract the `gcc-15.1.0` directory to here, and then run the
`gcc.sh` script. Upon its completion, GCC will be compiled and installed to
`/opt/clownmdsdk` and Stage 3 can begin.
