export PREFIX="/opt/clownmdsdk"

mkdir -p build
cmake -B build asl-releases -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON -DCMAKE_INSTALL_PREFIX=$PREFIX
cmake --build build --config Release --parallel ${1:-$(nproc)}
cmake --install build --config Release --strip
