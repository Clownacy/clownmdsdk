CLOWNMDSDK_LOCATION := /opt/clownmdsdk
CLOWNMDSDK_BIN := $(CLOWNMDSDK_LOCATION)/bin
CLOWNMDSDK_PREFIX := $(CLOWNMDSDK_BIN)/m68k-elf-

export PATH := $(CLOWNMDSDK_BIN):$(PATH)

AR := $(CLOWNMDSDK_PREFIX)ar
AS := $(CLOWNMDSDK_PREFIX)as
CC := $(CLOWNMDSDK_PREFIX)gcc
CXX := $(CLOWNMDSDK_PREFIX)g++
CPP := $(CLOWNMDSDK_PREFIX)cpp
LD := $(CLOWNMDSDK_PREFIX)ld

# GCC by default uses `-fuse-cxa-atexit`, which requires that the C
# standard library support `__cxa_atexit`. Since ClownMDSDK does not
# support `__cxa_atexit`, this setting causes errors. To mitigate this,
# use `-fno-use-cxa-atexit` to disable this requirement. Static
# destructors are stripped-out by the linker anyway (since Mega Drive
# games never exit), so it doesn't matter what they do so long as they
# don't produce errors.
CLOWNMDSDK_CANDCXXFLAGS := -mshort -D__MEGA_DRIVE__=1 -D__CLOWNMDSDK__=1 -ffreestanding -nodefaultlibs -fno-ident -isystem $(CLOWNMDSDK_LOCATION)/include -L $(CLOWNMDSDK_LOCATION)/lib
CFLAGS := $(CLOWNMDSDK_CANDCXXFLAGS)
CXXFLAGS := $(CLOWNMDSDK_CANDCXXFLAGS) -fno-exceptions -fno-rtti -fno-use-cxa-atexit
LDFLAGS := -lgcc -lc
