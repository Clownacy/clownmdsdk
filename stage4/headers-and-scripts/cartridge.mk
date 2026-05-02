	include /opt/clownmdsdk/generic.mk

LDFLAGS += -lstubs-cartridge -T $(CLOWNMDSDK_LOCATION)/cartridge.ld
