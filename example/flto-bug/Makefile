	include /opt/clownmdsdk/cartridge.mk

CXXFLAGS += -O3 -Wall -Wextra -Wpedantic -std=c++23 -fvisibility=hidden -flto -DNDEBUG
LDFLAGS += -save-temps=obj

all: bin/template-cartridge.bin

bin/template-cartridge.bin: main.cpp
	mkdir -p $(@D)
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)
