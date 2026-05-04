	include /opt/clownmdsdk/generic.mk

CFLAGS += -D__CLOWNMDSDK_IP__=1
CXXFLAGS += -D__CLOWNMDSDK_IP__=1
LDFLAGS += -T $(CLOWNMDSDK_LOCATION)/ip.ld
