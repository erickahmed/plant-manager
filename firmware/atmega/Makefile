PROJECT = vase-control

MCU = atmega328p
F_CPU = 16000000UL

CC = avr-gcc
LD = $(CC)
OC = avr-objcopy
RM = rm -f

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

SRCS_C = source/adc.c source/moisture.c source/main.c source/pump.c

OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS_C:.c=.o)))

CFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Iinclude
LDFLAGS = -Wall -Os -mmcu=$(MCU)
LIBS = -lm

EXEC = atmega
ELF_TARGET = $(BUILD_DIR)/$(EXEC).elf
TARGET = $(BUILD_DIR)/$(EXEC).hex

all: $(TARGET)

$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BUILD_DIR))

$(ELF_TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJ_DIR)/%.o: source/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(ELF_TARGET)
	$(OC) -O ihex -R .eeprom $< $@

clean:
	$(RM) $(TARGET) $(ELF_TARGET) $(OBJS)
	$(RM) -r $(BUILD_DIR)

.PHONY: all clean
