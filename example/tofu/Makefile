	include /opt/clownmdsdk/cartridge.mk

CXXFLAGS += -O3 -Wall -Wextra -Wpedantic -std=c++26 -fvisibility=hidden -flto
LDFLAGS += -save-temps=obj -Xlinker -Map=bin/output.map 

ifneq ($(DEBUG), 1)
  CXXFLAGS += -DNDEBUG
endif

all: bin/rom.bin

bin/rom.bin: main.cpp controller.cpp doubly-linked-list.cpp level.cpp objects.cpp objects/base.cpp objects/bullet.cpp objects/player.cpp pool.cpp singly-linked-list.cpp sprite.cpp
	mkdir -p $(@D)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS)
