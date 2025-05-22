export PREFIX="/opt/clownmdsdk"


mkdir build
cmake -B build asl-releases -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON -DCMAKE_INSTALL_PREFIX=$PREFIX
cmake --build build --config Release
cmake --build build --target install
