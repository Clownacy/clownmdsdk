	include /opt/clownmdsdk/generic.mk

CFLAGS += -O3 -flto -DNDEBUG -Wall -Wextra -Wpedantic -fvisibility=hidden

all: ../bin/lib/libc.a ../bin/init-cartridge.o ../bin/init-ip.o ../bin/init-sp.o

../bin/lib/libc.a: stdlib.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

../bin/%.o: %.s
	mkdir -p $(@D)
	$(AS) -c $< -o $@
