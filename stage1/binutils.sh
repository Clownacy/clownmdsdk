export PREFIX="/opt/clownmdsdk"
export TARGET=m68k-elf
export PATH="$PREFIX/bin:$PATH"

mkdir -p build-binutils
cd build-binutils
../binutils-2.43/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make LDFLAGS=--static
make install-strip
cd ..
