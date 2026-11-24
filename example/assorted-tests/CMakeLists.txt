cmake_minimum_required(VERSION 3.0...4.0)

project(assorted-tests LANGUAGES CXX)

# CD Build

add_executable(ip IP "ip.cpp" "cdc-test.cpp" "graphics-test.cpp" "hv-test.cpp" "main-menu.cpp" "utility.cpp")
set_target_properties(ip PROPERTIES CXX_STANDARD 26)

add_executable(sp SP "sp.cpp")
set_target_properties(sp PROPERTIES CXX_STANDARD 23)

add_iso_stub(assorted-tests-cd ip sp)

# Cartridge Build

add_custom_command(
	OUTPUT "font.kos"
	COMMAND "${CLOWNMDSDK_LOCATION}/bin/clownlzss" -k -i ${CMAKE_CURRENT_SOURCE_DIR}/../common/font.unc -o font.kos
)

add_custom_command(
	OUTPUT "sp.kos"
	COMMAND "${CLOWNMDSDK_LOCATION}/bin/clownlzss" -k -i sp.bin -o sp.kos
	DEPENDS sp
)

add_executable(assorted-tests-cartridge CARTRIDGE "ip.cpp" "cdc-test.cpp" "graphics-test.cpp" "hv-test.cpp" "main-menu.cpp" "utility.cpp" "font.kos" "sp.kos")
set_target_properties(assorted-tests-cartridge PROPERTIES CXX_STANDARD 26)
