	include /opt/clownmdsdk/ip.mk

CXXFLAGS += -O3 -Wall -Wextra -Wpedantic -std=c++23 -fvisibility=hidden -flto -DNDEBUG
LDFLAGS += -save-temps=obj

all: bin/ip.bin

bin/ip.o: ip.cpp
	mkdir -p bin
	$(CXX) -c $< -o $@ $(CXXFLAGS)

bin/ip.bin: bin/ip.o
	mkdir -p bin
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)
