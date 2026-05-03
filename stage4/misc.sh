export PREFIX="/opt/clownmdsdk"

# Copy the headers and scripts to the install location first,
# as they are needed for building the libraries.
cp -r headers-and-scripts/* $PREFIX

# Build the libraries.
make -C src -B

# Copy the libraries to the install location.
cp -r bin/install/* $PREFIX/m68k-elf

# Install host build of ClownLZSS, for its compression utility.
rm -rf bin/clownlzss-host
mkdir -p bin/clownlzss-host
cmake -B bin/clownlzss-host ../../clownlzss \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$PREFIX \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_POLICY_DEFAULT_CMP0069=NEW
cmake --build bin/clownlzss-host --config Release --parallel ${1:-$(nproc)}
cmake --build bin/clownlzss-host --target install

# Install target build of ClownLZSS, for its decompression libraries.
rm -rf bin/clownlzss-target
mkdir -p bin/clownlzss-target
cmake -B bin/clownlzss-target ../../clownlzss \
    --toolchain=$PREFIX/generic.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$PREFIX/m68k-elf \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON \
    -DCMAKE_POLICY_DEFAULT_CMP0069=NEW \
    -DCLOWNLZSS_TOOL=OFF \
    -DCLOWNLZSS_COMPRESSORS=OFF
cmake --build bin/clownlzss-target --config Release --parallel ${1:-$(nproc)}
cmake --build bin/clownlzss-target --target install
