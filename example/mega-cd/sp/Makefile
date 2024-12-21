	include /opt/clownmdsdk/sp.mk

CXXFLAGS += -O3 -Wall -Wextra -Wpedantic -std=c++23 -fvisibility=hidden -flto -DNDEBUG
LDFLAGS += -save-temps=obj

all: bin/sp.bin

bin/sp.o: sp.cpp
	mkdir -p bin
	$(CXX) -c $< -o $@ $(CXXFLAGS)

bin/sp.bin: bin/sp.o
	mkdir -p bin
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $< -o $@
