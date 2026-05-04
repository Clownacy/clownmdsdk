cmake_minimum_required(VERSION 3.0...4.0)

project(cdc-test LANGUAGES CXX)

# CD Build

add_ip_executable(ip "ip.cpp" "cdc-test.cpp" "graphics-test.cpp" "hv-test.cpp" "main-menu.cpp" "utility.cpp")
set_target_properties(ip PROPERTIES CXX_STANDARD 23)

add_sp_executable(sp "sp.cpp")
set_target_properties(sp PROPERTIES CXX_STANDARD 23)

add_custom_command(
	OUTPUT "cdc-test-cd.o"
	COMMAND "${CLOWNMDSDK_LOCATION}/bin/m68k-elf-as" "${CMAKE_CURRENT_SOURCE_DIR}/disc.s" -o "cdc-test-cd.o"
	DEPENDS ip sp
)

add_custom_command(
	OUTPUT "cdc-test-cd.iso"
	COMMAND "${CLOWNMDSDK_LOCATION}/bin/m68k-elf-ld" -shared "cdc-test-cd.o" -o "cdc-test-cd.iso" --oformat=binary
	DEPENDS cdc-test-cd.o
)

add_library(cdc-test-cd INTERFACE cdc-test-cd.iso)

# Cartridge Build

add_cartridge_executable(cdc-test-cartridge "ip.cpp" "cdc-test.cpp" "graphics-test.cpp" "hv-test.cpp" "main-menu.cpp" "utility.cpp")
set_target_properties(cdc-test-cartridge PROPERTIES CXX_STANDARD 23)
add_dependencies(cdc-test-cartridge sp)
