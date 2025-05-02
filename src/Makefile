PROJECT = plant-manager

# Flags
MCU = atmega328p
F_CPU = 16000000UL

CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude
CFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)

PROGRAMMER = arduino
PORT = /dev/ttyACM0

SRC = main.c
OBJ = $(SRC:.c=.o)

# Default target
all: $(PROJECT).hex

# Compiler
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Linker
$(PROJECT).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Create hex file
$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Uploader
upload: $(PROJECT).hex
	$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$<:i

# Cleaner
clean:
	rm -f *.o *.elf *.hex

.PHONY: all upload clean
