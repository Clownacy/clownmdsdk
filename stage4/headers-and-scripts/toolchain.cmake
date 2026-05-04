set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR m68k)

set(MEGADRIVE 1)
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
set(CMAKE_C_AND_CXX_FLAGS_INIT "-mshort -D__MEGA_DRIVE__ -ffreestanding -nodefaultlibs -fno-ident -fvisibility=hidden -isystem ${CLOWNMDSDK_LOCATION}/include -L ${CLOWNMDSDK_LOCATION}/lib")
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_AND_CXX_FLAGS_INIT}")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_AND_CXX_FLAGS_INIT} -fno-exceptions -fno-rtti -fno-use-cxa-atexit")
set(CMAKE_C_STANDARD_LIBRARIES "-lgcc -lc")
set(CMAKE_CXX_STANDARD_LIBRARIES "-lgcc -lc")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-save-temps=obj -Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/output.map")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_EXECUTABLE_SUFFIX_C ".bin")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".bin")

# Enable link-time optimisations for Release builds, because the
# Mega Drive sure does need it...
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

function(add_cartridge_executable name)
	add_executable(${ARGV})
	target_link_libraries(${name} PUBLIC stubs-cartridge)
	target_link_options(${name} PUBLIC "-T${CLOWNMDSDK_LOCATION}/cartridge.ld")
endfunction()

function(add_ip_executable name)
	add_executable(${ARGV})
	target_link_options(${name} PUBLIC "-T${CLOWNMDSDK_LOCATION}/ip.ld")
endfunction()

function(add_sp_executable name)
	add_executable(${ARGV})
	target_link_options(${name} PUBLIC "-T${CLOWNMDSDK_LOCATION}/sp.ld")
endfunction()