export PREFIX="/opt/clownmdsdk"
export TARGET=m68k-elf
export PATH="$PREFIX/bin:$PATH"

# Fix compatibility with Clang on Windows.
patch -sNd gcc-14.2.0 -p1 < FixMinGWClang.patch

mkdir -p build-gcc
cd build-gcc
../gcc-14.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --disable-multilib --with-cpu=68000 --disable-hosted-libstdcxx
make LDFLAGS=--static all-gcc
make LDFLAGS=--static all-target-libgcc
make LDFLAGS=--static all-target-libstdc++-v3
make install-strip-gcc
make install-strip-target-libgcc
make install-strip-target-libstdc++-v3
cd ..
