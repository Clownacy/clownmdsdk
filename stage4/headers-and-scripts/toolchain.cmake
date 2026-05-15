set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR m68k)

set(MEGA_DRIVE 1)
set(CLOWNMDSDK 1)

set(CLOWNMDSDK_LOCATION "${CMAKE_CURRENT_LIST_DIR}")
set(CMAKE_SYSROOT "${CLOWNMDSDK_LOCATION}")
set(CMAKE_STAGING_PREFIX "${CLOWNMDSDK_LOCATION}/stage")

set(CMAKE_C_COMPILER "${CLOWNMDSDK_LOCATION}/bin/m68k-elf-gcc${CMAKE_HOST_EXECUTABLE_SUFFIX}")
set(CMAKE_CXX_COMPILER "${CLOWNMDSDK_LOCATION}/bin/m68k-elf-g++${CMAKE_HOST_EXECUTABLE_SUFFIX}")

# GCC by default uses `-fuse-cxa-atexit`, which requires that the C
# standard library support `__cxa_atexit`. Since ClownMDSDK does not
# support `__cxa_atexit`, this setting causes errors. To mitigate this,
# use `-fno-use-cxa-atexit` to disable this requirement. Static
# destructors are stripped-out by the linker anyway (since Mega Drive
# games never exit), so it doesn't matter what they do so long as they
# don't produce errors.
# Visibility is set to hidden to assist compiler optimisations, since
# visible symbols cannot be inlined.
add_compile_definitions(__MEGA_DRIVE__=1 __CLOWNMDSDK__=1)
add_compile_options(-mshort -ffreestanding -nodefaultlibs -fno-ident -fvisibility=hidden)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions> $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)

macro(clownmdsdk_do_language_flags language)
	set(CMAKE_${language}_STANDARD_INCLUDE_DIRECTORIES "${CLOWNMDSDK_LOCATION}/include")
	set(CMAKE_${language}_STANDARD_LINK_DIRECTORIES "${CLOWNMDSDK_LOCATION}/lib")
	set(CMAKE_EXECUTABLE_SUFFIX_${language} ".bin")
endmacro()

clownmdsdk_do_language_flags(C)
clownmdsdk_do_language_flags(CXX)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Enable link-time optimisations for Release builds, because the
# Mega Drive sure does need it...
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

function(clownmdsdk_add_executable name)
	add_executable(${ARGV})
	# So we can study the generated assembly.
	target_link_options(${name} PUBLIC -save-temps=obj "LINKER:-Map=${CMAKE_CURRENT_BINARY_DIR}/${name}.map")
endfunction()

function(add_cartridge_executable name)
	clownmdsdk_add_executable(${ARGV})
	target_compile_definitions(${name} PRIVATE __CLOWNMDSDK_CARTRIDGE__=1)
	target_link_options(${name} PUBLIC "-T${CLOWNMDSDK_LOCATION}/cartridge.ld")
	target_link_libraries(${name} PUBLIC stubs-cartridge)
endfunction()

function(add_ip_executable name)
	clownmdsdk_add_executable(${ARGV})
	target_compile_definitions(${name} PRIVATE __CLOWNMDSDK_IP__=1)
	target_link_options(${name} PUBLIC "-T${CLOWNMDSDK_LOCATION}/ip.ld")
endfunction()

function(add_sp_executable name)
	clownmdsdk_add_executable(${ARGV})
	target_compile_definitions(${name} PRIVATE __CLOWNMDSDK_SP__=1)
	target_link_options(${name} PUBLIC "-T${CLOWNMDSDK_LOCATION}/sp.ld")
endfunction()

function(add_iso_stub name ip sp)
	add_custom_command(
		OUTPUT "${name}.s"
		COMMAND "${CLOWNMDSDK_LOCATION}/bin/m68k-elf-cpp" "${CLOWNMDSDK_LOCATION}/iso-stub.s" -o "${name}.s" -DIP_FILENAME=\\"${ip}.bin\\" -DSP_FILENAME=\\"${sp}.bin\\"
	)

	add_custom_command(
		OUTPUT "${name}.o"
		COMMAND "${CLOWNMDSDK_LOCATION}/bin/m68k-elf-as" "${name}.s" -o "${name}.o"
		DEPENDS ${name}.s ${ip} ${sp}
	)

	add_custom_command(
		OUTPUT "${name}.iso"
		COMMAND "${CLOWNMDSDK_LOCATION}/bin/m68k-elf-ld" -shared "${name}.o" -o "${name}.iso" --oformat=binary
		DEPENDS ${name}.o
	)

	add_library(${name} INTERFACE ${name}.iso)
endfunction()
