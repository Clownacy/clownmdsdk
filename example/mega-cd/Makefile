	include /opt/clownmdsdk/bare.mk

all: bin/disc.iso

ip/bin/ip.bin:
	$(MAKE) -C ip

sp/bin/sp.bin:
	$(MAKE) -C sp

bin/disc.o: disc.s ip/bin/ip.bin sp/bin/sp.bin
	mkdir -p $(@D)
	$(AS) $< -o $@

bin/disc.iso: bin/disc.o
	mkdir -p $(@D)
	$(LD) -shared $< -o $@ --oformat=binary
