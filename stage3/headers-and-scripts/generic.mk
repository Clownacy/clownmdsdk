CLOWNMDSDK_LOCATION := /opt/clownmdsdk
CLOWNMDSDK_PREFIX := $(CLOWNMDSDK_LOCATION)/bin/m68k-elf-

AS := $(CLOWNMDSDK_PREFIX)as
CC := $(CLOWNMDSDK_PREFIX)gcc
CXX := $(CLOWNMDSDK_PREFIX)g++
CPP := $(CLOWNMDSDK_PREFIX)cpp
LD := $(CLOWNMDSDK_PREFIX)ld
INCBIN := $(CLOWNMDSDK_PREFIX)objcopy -I binary -O elf32-m68k --rename-section .data=.rodata,alloc,load,readonly,data,contents

CLOWNMDSDK_CANDCXXFLAGS := -mshort -D__MEGA_DRIVE__ -ffreestanding -nodefaultlibs -fno-ident -fno-use-cxa-atexit -isystem $(CLOWNMDSDK_LOCATION)/include -L $(CLOWNMDSDK_LOCATION)/lib
CFLAGS := $(CLOWNMDSDK_CANDCXXFLAGS)
CXXFLAGS := $(CLOWNMDSDK_CANDCXXFLAGS) -fno-exceptions -fno-rtti
LDFLAGS := -lgcc -lc
