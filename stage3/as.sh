export PREFIX="/opt/clownmdsdk"

mkdir -p build
cmake -B build asl-releases -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
cmake --build build --config Release
cmake --install build --config Release --strip --prefix $PREFIX
