
BASE = .
.PHONY: docs $(BASE)
#KILOLIB = ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/build/kilolib.a
KILOLIB = ./TemplateForCompiling/build/kilolib.a
TEST = ./TemplateForCompiling/
#KILOLIB = build/kilolib.a
#KILOLIB = ../phototaxis/phototaxis/kilolib.a
#phototaxis: phototaxis.elf phototaxis.hex phototaxis.lss
#./$(BASE): ## added
#	mkdir -p $@
SRC = src
BIN = bin
$(BASE): ./$(BIN)/$(BASE).elf ./$(BIN)/$(BASE).hex ./$(BIN)/$(BASE).lss
#$(BASE): ./$(SRC)/$(BASE).elf ./$(SRC)/$(BASE).hex ./$(SRC)/$(BASE).lss

CC = avr-gcc
AVRAR = avr-ar
AVROC = avr-objcopy
AVROD = avr-objdump
AVRUP = avrdude

#BASE = .
PFLAGS = -P usb -c avrispmkII # user to reprogram OHC
CFLAGS = -mmcu=atmega328p -I ./TemplateForCompiling/ -Wall -gdwarf-2 -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -I .
CFLAGS += -DF_CPU=8000000
ASFLAGS = $(CFLAGS)
#$(BASE): ./$(BASE)/$(BASE).elf ./$(BASE)/$(BASE).hex ./$(BASE)/$(BASE).lss


FLASH = -R .eeprom -R .fuse -R .lock -R .signature
EEPROM = -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0

%.lss: %.elf
#$(BASE)/%.lss: %.elf
	$(AVROD) -d -S $< > $@
	rm -f $(BIN)/*.elf $(BIN)/*.lss

%.hex: %.elf
#$(BASE)/%.hex: %.elf
	$(AVROC) -O ihex $(FLASH) $< $@

%.eep: %.elf
#$(BASE)/%.eep: %.elf
	$(AVROC) -O ihex $(EEPROM) $< $@

%.bin: %.elf
#$(BASE)/%.bin: %.elf
	$(AVROC) -O binary $(FLASH) $< $@

#./$(BASE):
#	mkdir -p $@

message_send.o: message_send.S
	$(CC) $(CFLAGS) -c -o $@ $<

#$(KILOLIB): kilolib.o message_crc.o message_send.o | ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/build/
$(KILOLIB): $(TEST)/kilolib.o $(TEST)/message_crc.o $(TEST)/message_send.o | ./$(TEST)/build/
#$(KILOLIB): ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/kilolib.o ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/message_crc.o ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/message_send.o | ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/build/
	$(AVRAR) rcs $@ $(TEST)/kilolib.o $(TEST)/message_crc.o $(TEST)/message_send.o
	rm -f *.o

./$(BIN)/$(BASE).elf: ./src/$(BASE).c $(KILOLIB) | ./$(SRC)/
#./$(SRC)/$(BASE).elf: ./$(SRC)/$(BASE).c $(KILOLIB) | ./$(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(KILOLIB)

#./$(BIN)/$(BASE).hex:
#	rm -f $(BIN)/$(BASE).elf $(BIN)/$(BASE).lss


docs:
	cat message.h kilolib.h message_crc.h | grep -v "^\#" > docs/kilolib.h
	(cd docs; doxygen)

clean:
	m -fR ./$(BIN)/*.elf ./$(BIN)/*.hex ./$(BIN)/*.lss
	#rm -fR ./$(SRC)/*.elf ./$(SRC)/*.hex ./$(SRC)/*.lss
