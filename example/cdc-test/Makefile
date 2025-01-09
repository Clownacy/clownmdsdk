	include /opt/clownmdsdk/generic.mk

CXXFLAGS += -O3 -Wall -Wextra -Wpedantic -std=c++23 -fvisibility=hidden -flto -DNDEBUG
LDFLAGS += -save-temps=obj

all: bin/cdc-test-cd.iso bin/cdc-test-cartridge.bin

bin/ip.bin: ip.cpp
	mkdir -p $(@D)
	$(CXX) $< -o $@ $(CXXFLAGS) -DCD $(LDFLAGS) -T $(CLOWNMDSDK_LOCATION)/ip.ld

bin/sp.bin: sp.cpp
	mkdir -p $(@D)
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS) -T $(CLOWNMDSDK_LOCATION)/sp.ld

bin/cdc-test-cd.o: disc.s bin/ip.bin bin/sp.bin
	mkdir -p $(@D)
	$(AS) $< -o $@

bin/cdc-test-cd.iso: bin/cdc-test-cd.o
	mkdir -p $(@D)
	$(LD) -shared $< -o $@ --oformat=binary

bin/sp.o: bin/sp.bin
	$(INCBIN) $< $@

bin/font.o: font.unc
	$(INCBIN) $< $@

bin/cdc-test-cartridge.bin: ip.cpp bin/sp.o bin/font.o
	mkdir -p $(@D)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS) -T $(CLOWNMDSDK_LOCATION)/cartridge.ld
