CLOWNMDSDK_LOCATION := /opt/clownmdsdk
CLOWNMDSDK_PREFIX := $(CLOWNMDSDK_LOCATION)/bin/m68k-elf-

AS := $(CLOWNMDSDK_PREFIX)as
CC := $(CLOWNMDSDK_PREFIX)gcc
CXX := $(CLOWNMDSDK_PREFIX)g++
CPP := $(CLOWNMDSDK_PREFIX)cpp
LD := $(CLOWNMDSDK_PREFIX)ld

CLOWNMDSDK_CANDCXXFLAGS := -mshort -D__MEGA_DRIVE__ -ffreestanding -nostdlib -fno-ident -isystem $(CLOWNMDSDK_LOCATION)/include
CFLAGS := $(CLOWNMDSDK_CANDCXXFLAGS)
CXXFLAGS := $(CLOWNMDSDK_CANDCXXFLAGS) -fno-exceptions -fno-rtti
LDFLAGS := -lgcc
