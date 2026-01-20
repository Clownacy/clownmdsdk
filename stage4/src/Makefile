	include /opt/clownmdsdk/generic.mk

# TODO: Use '-flto' here once GCC fixes whatever ridiculous bug of theirs is causing `memset` to resolve to address 0.
CFLAGS += -O3 -DNDEBUG -Wall -Wextra -Wpedantic -fvisibility=hidden

all: ../bin/install/lib/libc.a ../bin/install/init-cartridge.o ../bin/install/init-ip.o ../bin/install/init-sp.o

../bin/install/lib/libc.a: ../bin/stdlib.o ../bin/string.o
	mkdir -p $(@D)
	$(AR) rcs $@ $^

../bin/%.o: %.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

../bin/install/%.o: %.s
	mkdir -p $(@D)
	$(AS) -c $< -o $@

../bin/install/init-cartridge.o: ../bin/init-z80.bin

../bin/init-z80.bin: init-z80.asm
	mkdir -p $(@D)
	asl -q $< -o ../bin/init-z80.p
	p2bin -q ../bin/init-z80.p $@
