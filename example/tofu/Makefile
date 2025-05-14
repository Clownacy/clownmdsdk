	include /opt/clownmdsdk/cartridge.mk

CXXFLAGS += -O3 -Wall -Wextra -Wpedantic -std=c++26 -fvisibility=hidden -flto
LDFLAGS += -save-temps=obj -Xlinker -Map=bin/output.map 

ifneq ($(DEBUG), 1)
  CXXFLAGS += -DNDEBUG
endif

OBJDIR := bin

SOURCES := \
	main \
	controller \
	doubly-linked-list \
	level \
	objects \
	objects/base \
	objects/bullet \
	objects/player \
	pool \
	singly-linked-list \
	sprite

OBJECTS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(SOURCES)))
DEPENDENCIES = $(addprefix $(OBJDIR)/, $(addsuffix .d, $(SOURCES)))

all: $(OBJDIR)/rom.bin

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $^ -o $@ -c $(CXXFLAGS) -MMD -MP -MF $(OBJDIR)/$*.d

$(OBJDIR)/rom.bin: $(OBJECTS)
	mkdir -p $(@D)
	$(CXX) $^ -o $@ $(CXXFLAGS) $(LDFLAGS)

include $(wildcard $(DEPENDENCIES))
