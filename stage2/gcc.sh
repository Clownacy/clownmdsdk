export PREFIX="/opt/clownmdsdk"
export TARGET=m68k-elf
export PATH="$PREFIX/bin:$PATH"

# Fix compatibility with Clang on Windows.
patch -sNd gcc-16.1.0 -p1 < FixMinGWClang.patch

mkdir -p build-gcc
cd build-gcc
../gcc-16.1.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --disable-multilib --with-cpu=68000 --disable-hosted-libstdcxx
make all-gcc
make all-target-libgcc
make all-target-libstdc++-v3
make install-strip-gcc
make install-strip-target-libgcc
make install-strip-target-libstdc++-v3
cd ..

# Work-around a dumb GCC bug.
patch -sNd $PREFIX/$TARGET/include/c++/16.1.0 -p1 < DisableBrokenOptional.patch
