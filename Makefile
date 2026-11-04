# ZAD PS5 homebrew - make pc | make ps5 | make clean
SRC := src/main.c
CFLAGS_COMMUN := -Wall -Wextra -O2

.PHONY: pc ps5 clean test-ps5

pc: build/zad-pc

build/zad-pc: $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS_COMMUN) $^ -o $@ $(shell sdl2-config --cflags --libs)

ps5: build/zad.elf

ifdef PS5_PAYLOAD_SDK
include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
endif

build/zad.elf: $(SRC)
ifndef PS5_PAYLOAD_SDK
	$(error PS5_PAYLOAD_SDK est indefini : voir README.md, etape 1)
endif
	@mkdir -p build
	$(CC) $(CFLAGS_COMMUN) -DPLATFORM_PS5 $^ -o $@ \
	  $(shell $(PS5_PAYLOAD_SDK)/bin/prospero-pkg-config --cflags --libs sdl2)

# envoi vers la console : make test-ps5 PS5_HOST=192.168.x.x
PS5_HOST ?= ps5
PS5_PORT ?= 9021
test-ps5: build/zad.elf
	nc -w 3 $(PS5_HOST) $(PS5_PORT) < build/zad.elf

clean:
	rm -rf build
