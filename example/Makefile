	include /opt/clownmdsdk/rom.mk

CXXFLAGS += -O3 -Wall -Wextra -Wpedantic -std=c++23 -fvisibility=hidden -flto -DNDEBUG
LDFLAGS += -save-temps=obj

all: bin/rom.bin

bin/main.o: main.cpp
	mkdir -p bin
	$(CXX) -c $< -o $@ $(CXXFLAGS)

bin/rom.bin: bin/main.o
	mkdir -p bin
	$(CXX) $< -o $@ $(CXXFLAGS) $(LDFLAGS)
