cmake_minimum_required(VERSION 3.0...4.0)

project(template-cd LANGUAGES CXX)

add_executable(ip "ip.cpp")
set_target_properties(ip PROPERTIES CXX_STANDARD 23)
target_link_options(ip PRIVATE "-T${CLOWNMDSDK_LOCATION}/ip.ld")

add_executable(sp "sp.cpp")
set_target_properties(sp PROPERTIES CXX_STANDARD 23)
target_link_options(sp PRIVATE "-T${CLOWNMDSDK_LOCATION}/sp.ld")

add_custom_command(
	OUTPUT "template-cd.o"
	COMMAND ${CLOWNMDSDK_LOCATION}/bin/m68k-elf-as "${CMAKE_CURRENT_SOURCE_DIR}/disc.s" -o "template-cd.o"
	DEPENDS ip sp
)

add_custom_command(
	OUTPUT "template-cd.iso"
	COMMAND ${CLOWNMDSDK_LOCATION}/bin/m68k-elf-ld -shared "template-cd.o" -o "template-cd.iso" --oformat=binary
	DEPENDS template-cd.o
)

add_library(template-cd INTERFACE template-cd.iso)
