	include /opt/clownmdsdk/bare.mk

LDFLAGS += $(CLOWNMDSDK_LOCATION)/lib/libc.o -T $(CLOWNMDSDK_LOCATION)/rom.ld
