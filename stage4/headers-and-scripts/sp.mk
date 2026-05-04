	include /opt/clownmdsdk/generic.mk

CFLAGS += -D__CLOWNMDSDK_SP__=1
CXXFLAGS += -D__CLOWNMDSDK_SP__=1
LDFLAGS += -T $(CLOWNMDSDK_LOCATION)/sp.ld
