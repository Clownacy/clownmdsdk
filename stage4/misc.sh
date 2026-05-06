export PREFIX="/opt/clownmdsdk"

# Copy the headers and scripts to the install location first,
# as they are needed for building the libraries.
cp -r headers-and-scripts/* $PREFIX

# Build the libraries.
make -C src -B

# Copy the libraries to the install location.
cp -r bin/install/* $PREFIX/m68k-elf

# Install host build of ClownLZSS, for its compression utility.
mkdir -p bin/clownlzss-host
cmake -B bin/clownlzss-host ../../clownlzss \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_POLICY_DEFAULT_CMP0069=NEW \
    -DCMAKE_INSTALL_PREFIX=$PREFIX
cmake --build bin/clownlzss-host --config Release --parallel ${1:-$(nproc)}
cmake --install bin/clownlzss-host --config Release --prefix $PREFIX --strip

# Install target build of ClownLZSS, for its decompression libraries.
mkdir -p bin/clownlzss-target
cmake -B bin/clownlzss-target ../../clownlzss \
    --toolchain=$PREFIX/toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_POLICY_DEFAULT_CMP0069=NEW \
    -DCMAKE_INSTALL_PREFIX=$PREFIX/m68k-elf \
    -DCLOWNLZSS_TOOL=OFF \
    -DCLOWNLZSS_COMPRESSORS=OFF
cmake --build bin/clownlzss-target --config Release --parallel ${1:-$(nproc)}
cmake --install bin/clownlzss-target --config Release --prefix $PREFIX/m68k-elf --strip
