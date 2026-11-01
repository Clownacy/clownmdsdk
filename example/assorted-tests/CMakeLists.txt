cmake_minimum_required(VERSION 3.0...4.0)

project(assorted-tests LANGUAGES CXX)

# CD Build

add_ip_executable(ip "ip.cpp" "cdc-test.cpp" "graphics-test.cpp" "hv-test.cpp" "main-menu.cpp" "utility.cpp")
set_target_properties(ip PROPERTIES CXX_STANDARD 26)

add_sp_executable(sp "sp.cpp")
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

add_cartridge_executable(assorted-tests-cartridge "ip.cpp" "cdc-test.cpp" "graphics-test.cpp" "hv-test.cpp" "main-menu.cpp" "utility.cpp" "font.kos" "sp.kos")
set_target_properties(assorted-tests-cartridge PROPERTIES CXX_STANDARD 26)
