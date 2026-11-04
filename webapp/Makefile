# Installeur de l'application web ZAD (icone dans "Contenu multimedia"). Base sur svtplay (GPL-3.0+).
PS5_HOST ?= ps5
PS5_PORT ?= 9021
ifdef PS5_PAYLOAD_SDK
    include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
else
    $(error PS5_PAYLOAD_SDK est indefini)
endif
ELF := zad-install.elf
CFLAGS := -Wall -Werror -g -DTITLE_ID="\"ZADW00001\""
LDADD := -lSceIpmi -lSceAppInstUtil
all: $(ELF)
$(ELF): main.c sce_sys/param.json sce_sys/icon0.png sce_sys/pic1.png
	$(CC) $(CFLAGS) $(LDADD) -o $@ main.c
clean:
	rm -f $(ELF)
install: $(ELF)
	$(PS5_DEPLOY) -h $(PS5_HOST) -p $(PS5_PORT) $^
