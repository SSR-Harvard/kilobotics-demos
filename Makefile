
BASE = .
.PHONY: docs $(BASE)
KILOLIB = ./TemplateForCompiling/build/kilolib.a
TEST = ./TemplateForCompiling/
SRC = src
BIN = bin
$(BASE): ./$(BIN)/$(BASE).elf ./$(BIN)/$(BASE).hex ./$(BIN)/$(BASE).lss

CC = avr-gcc
AVRAR = avr-ar
AVROC = avr-objcopy
AVROD = avr-objdump
AVRUP = avrdude

PFLAGS = -P usb -c avrispmkII # user to reprogram OHC
CFLAGS = -mmcu=atmega328p -I ./TemplateForCompiling/ -Wall -gdwarf-2 -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -I .
CFLAGS += -DF_CPU=8000000
ASFLAGS = $(CFLAGS)


FLASH = -R .eeprom -R .fuse -R .lock -R .signature
EEPROM = -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0

%.lss: %.elf
	$(AVROD) -d -S $< > $@
	rm -f $(BIN)/*.elf $(BIN)/*.lss

%.hex: %.elf
	$(AVROC) -O ihex $(FLASH) $< $@

%.eep: %.elf
	$(AVROC) -O ihex $(EEPROM) $< $@

%.bin: %.elf
	$(AVROC) -O binary $(FLASH) $< $@


message_send.o: message_send.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(KILOLIB): $(TEST)/kilolib.o $(TEST)/message_crc.o $(TEST)/message_send.o | ./$(TEST)/build/
	$(AVRAR) rcs $@ $(TEST)/kilolib.o $(TEST)/message_crc.o $(TEST)/message_send.o
	rm -f *.o

./$(BIN)/$(BASE).elf: ./src/$(BASE).c $(KILOLIB) | ./$(SRC)/
	$(CC) $(CFLAGS) -o $@ $< $(KILOLIB)



docs:
	cat message.h kilolib.h message_crc.h | grep -v "^\#" > docs/kilolib.h
	(cd docs; doxygen)

clean:
	rm -fR ./$(BIN)/$(BASE).elf ./$(BIN)/$(BASE).hex ./$(BIN)/$(BASE).lss
