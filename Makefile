
.PHONY: docs sync
KILOLIB = ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/build/kilolib.a
TEST = ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/
#KILOLIB = build/kilolib.a
#KILOLIB = ../sync/sync/kilolib.a
sync: ~/Desktop/HarvardREU/Kilobots_Hex_Files/sync/sync.elf ~/Desktop/HarvardREU/Kilobots_Hex_Files/sync/sync.hex ~/Desktop/HarvardREU/Kilobots_Hex_Files/sync/sync.lss
#sync: sync.elf sync.hex sync.lss

CC = avr-gcc
AVRAR = avr-ar
AVROC = avr-objcopy
AVROD = avr-objdump
AVRUP = avrdude

PFLAGS = -P usb -c avrispmkII # user to reprogram OHC
CFLAGS = -mmcu=atmega328p -I ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/ -Wall -gdwarf-2 -O3 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -I .
CFLAGS += -DF_CPU=8000000
ASFLAGS = $(CFLAGS)

FLASH = -R .eeprom -R .fuse -R .lock -R .signature
EEPROM = -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0  

%.lss: %.elf
	$(AVROD) -d -S $< > $@

%.hex: %.elf
	$(AVROC) -O ihex $(FLASH) $< $@

%.eep: %.elf
	$(AVROC) -O ihex $(EEPROM) $< $@

%.bin: %.elf
	$(AVROC) -O binary $(FLASH) $< $@ 

~/Desktop/HarvardREU/Kilobots_Hex_Files/sync:
	mkdir -p $@

message_send.o: message_send.S
	$(CC) $(CFLAGS) -c -o $@ $<

#$(KILOLIB): kilolib.o message_crc.o message_send.o | ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/build/
$(KILOLIB): $(TEST)/kilolib.o $(TEST)/message_crc.o $(TEST)/message_send.o | ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/build/
#$(KILOLIB): ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/kilolib.o ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/message_crc.o ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/message_send.o | ~/Desktop/HarvardREU/Kilobots_Hex_Files/TemplateForCompiling/build/
	$(AVRAR) rcs $@ $(TEST)/kilolib.o $(TEST)/message_crc.o $(TEST)/message_send.o 
	rm -f *.o

~/Desktop/HarvardREU/Kilobots_Hex_Files/sync/sync.elf: ~/Desktop/HarvardREU/Kilobots_Hex_Files/sync/sync.c $(KILOLIB) | ~/Desktop/HarvardREU/Kilobots_Hex_Files/sync
	$(CC) $(CFLAGS) -o $@ $< $(KILOLIB) 

docs:
	cat message.h kilolib.h message_crc.h | grep -v "^\#" > docs/kilolib.h
	(cd docs; doxygen)

clean:
	rm -fR ~/Desktop/HarvardREU/Kilobots_Hex_Files/sync/*.elf ~/Desktop/HarvardREU/Kilobots_Hex_Files/sync/*.hex ~/Desktop/HarvardREU/Kilobots_Hex_Files/sync/*.lss
