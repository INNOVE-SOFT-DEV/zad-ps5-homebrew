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

DEPS := deps
SDL_INC := -I$(DEPS)/SDL/build/include -I$(DEPS)/SDL/build/include/SDL2 \
           -I$(DEPS)/SDL/build/include-config-release/SDL2 -I$(DEPS)/SDL/build/include-config-release
PS5_LIBS := $(DEPS)/SDL/build/libSDL2.a $(DEPS)/libsamplerate/src/.libs/libsamplerate.a \
            -lSceSystemService -lSceUserService -lScePad -lSceAudioOut -lSceVideoOut \
            -lSceKeyboard -lSceImeDialog -lSceNotification

build/zad.elf: $(SRC)
ifndef PS5_PAYLOAD_SDK
	$(error PS5_PAYLOAD_SDK est indefini : voir README.md, etape 1)
endif
	@test -f $(DEPS)/SDL/build/libSDL2.a || { echo "Lancez d'abord scripts/setup-ps5-sdl.sh"; exit 1; }
	@mkdir -p build
	$(CC) $(CFLAGS_COMMUN) -DPLATFORM_PS5 $(SDL_INC) $^ -o $@ $(PS5_LIBS)

# envoi vers la console : make test-ps5 PS5_HOST=192.168.x.x
PS5_HOST ?= ps5
PS5_PORT ?= 9021
test-ps5: build/zad.elf
	nc -w 3 $(PS5_HOST) $(PS5_PORT) < build/zad.elf

clean:
	rm -rf build
