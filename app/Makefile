# Application ZAD pour PS5 : make -C app   (necessite PS5_PAYLOAD_SDK)
# Base sur samples/install_app du PS5 Payload SDK (GPL-3.0).
ifdef PS5_PAYLOAD_SDK
include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
else
$(error PS5_PAYLOAD_SDK est indefini)
endif

TITLE_ID := ZADA00001
ELF2SELF := $(PS5_PAYLOAD_SDK)/samples/install_app/make_fself.py
CFLAGS := -Wall -Werror -g -DTITLE_ID=\"$(TITLE_ID)\"
LDADD := -lSceIpmi -lSceAppInstUtil -lSceUserService -lSceSystemService -lSceNotification
PS4_SDK_VERSION ?= 0x07590001
AUTHID := 0x3800000000000022
AUTHINFO := 00 00 00 00 00 00 00 00 00 00 00 00 00 1C 00 40 00 FF 00 00 00 00 \
            00 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 80 \
            00 40 00 40 00 00 00 00 00 00 00 80 00 00 00 00 00 00 00 08 00 40 \
            FF FF 00 00 00 F0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 \
            00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 \
            00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 \
            00 00 00 00

.INTERMEDIATE: eboot.elf
all: installer.elf uninstaller.elf $(TITLE_ID)/eboot.bin

installer.elf: installer.c
	$(CC) $(CFLAGS) $(LDADD) -o $@ $^
uninstaller.elf: uninstaller.c
	$(CC) $(CFLAGS) $(LDADD) -o $@ $^
eboot.o: eboot.c
	$(CC) -c $(CFLAGS) -o $@ $^
eboot.elf: eboot.o
	$(LD) --static -T eboot.x -o $@ $^
$(TITLE_ID)/eboot.bin: eboot.elf
	python3 $(ELF2SELF) --ptype fake --paid $(AUTHID) --auth-info "$(AUTHINFO)" \
	  --app-version $(PS4_SDK_VERSION) --fw-version $(PS4_SDK_VERSION) $^ $@
clean:
	rm -f *.o *.elf $(TITLE_ID)/eboot.bin
