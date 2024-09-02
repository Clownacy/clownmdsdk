	include /opt/clownmdsdk/bare.mk

CFLAGS += -O3 -flto -DNDEBUG -Wall -Wextra -Wpedantic -fvisibility=hidden

all: ../bin/lib/libc.o ../bin/init.o

../bin/lib/libc.o: stdlib.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)

../bin/%.o: %.s
	mkdir -p $(@D)
	$(AS) -c $< -o $@
