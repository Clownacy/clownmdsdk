	include /opt/clownmdsdk/bare.mk

CXXFLAGS += -O3 -Wall -Wextra -Wpedantic -std=c++23 -fvisibility=hidden -flto -DNDEBUG
LDFLAGS += -save-temps=obj

all: bin/disc.iso

bin/ip.bin: ip.cpp
	mkdir -p $(@D)
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS) -T $(CLOWNMDSDK_LOCATION)/ip.ld

bin/sp.bin: sp.cpp
	mkdir -p $(@D)
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS) -T $(CLOWNMDSDK_LOCATION)/sp.ld

bin/disc.o: disc.s bin/ip.bin bin/sp.bin
	mkdir -p $(@D)
	$(AS) $< -o $@

bin/disc.iso: bin/disc.o
	mkdir -p $(@D)
	$(LD) -shared $< -o $@ --oformat=binary
