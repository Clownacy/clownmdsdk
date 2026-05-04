	include /opt/clownmdsdk/generic.mk

CFLAGS += -D__CLOWNMDSDK_CARTRIDGE__=1
CXXFLAGS += -D__CLOWNMDSDK_CARTRIDGE__=1
LDFLAGS += -lstubs-cartridge -T $(CLOWNMDSDK_LOCATION)/cartridge.ld
