	include /opt/clownmdsdk/generic.mk

# TODO: Use '-flto' here once GCC fixes whatever ridiculous bug of theirs is causing `memset` to resolve to address 0.
CFLAGS += -O3 -DNDEBUG -Wall -Wextra -Wpedantic -fvisibility=hidden

all: ../bin/lib/libc.a ../bin/init-cartridge.o ../bin/init-ip.o ../bin/init-sp.o

../bin/lib/libc.a: string.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

../bin/%.o: %.s
	mkdir -p $(@D)
	$(AS) -c $< -o $@
