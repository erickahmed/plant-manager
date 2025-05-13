PROJECT = vase-control

MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = usbasp

ARDUINO_INSTALL_PATH = /usr/share/arduino
ARDUINO_CORE_PATH = $(ARDUINO_INSTALL_PATH)/hardware/arduino/avr/cores/arduino
ARDUINO_VARIANT_PATH = $(ARDUINO_INSTALL_PATH)/hardware/arduino/avr/variants/standard

CC = avr-gcc
CXX = avr-g++
LD = $(CXX)
OC = avr-objcopy
AD = avrdude
AR = avr-ar
RM = rm -f

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
CORE_DIR = $(BUILD_DIR)/core

CFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -I$(ARDUINO_CORE_PATH) -I$(ARDUINO_VARIANT_PATH) -I.
CXXFLAGS = $(CFLAGS) -fno-exceptions -fpermissive -std=gnu++11

LDFLAGS = -Wall -Os -mmcu=$(MCU) -Wl,--gc-sections
LIBS = -lm

SRCS_CPP = main.cpp moisture.cpp
HDRS = moisture.h config.h
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS_CPP:.cpp=.o))

CORE_C_SRCS = \
	wiring.c \
	wiring_analog.c \
	wiring_digital.c \
	hooks.c

CORE_CPP_SRCS = \
	abi.cpp \
	new.cpp \
	Print.cpp \
	Stream.cpp \
	HardwareSerial.cpp \
	HardwareSerial0.cpp \
	main.cpp

CORE_SRCS = $(addprefix $(ARDUINO_CORE_PATH)/, $(CORE_C_SRCS) $(CORE_CPP_SRCS))
CORE_OBJS = $(patsubst $(ARDUINO_CORE_PATH)/%.c,$(CORE_DIR)/%.o,$(filter %.c,$(CORE_SRCS))) \
            $(patsubst $(ARDUINO_CORE_PATH)/%.cpp,$(CORE_DIR)/%.o,$(filter %.cpp,$(CORE_SRCS)))

CORE_LIB = $(BUILD_DIR)/libcore.a

EXEC = moisture
ELF_TARGET = $(BUILD_DIR)/$(EXEC).elf
TARGET = $(BUILD_DIR)/$(EXEC).hex

all: $(TARGET)

$(shell mkdir -p $(OBJ_DIR) $(CORE_DIR))

$(ELF_TARGET): $(OBJS) $(CORE_LIB)
	$(LD) $(LDFLAGS) $(OBJS) -L$(BUILD_DIR) -lcore $(LIBS) -o $@

$(CORE_LIB): $(CORE_OBJS)
	$(AR) rcs $@ $^

# Compile your C++ files
$(OBJ_DIR)/%.o: %.cpp $(HDRS) Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(CORE_DIR)/%.o: $(ARDUINO_CORE_PATH)/%.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(CORE_DIR)/%.o: $(ARDUINO_CORE_PATH)/%.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(ELF_TARGET)
	$(OC) -O ihex -R .eeprom $< $@

flash: $(TARGET)
	$(AD) -v -p $(MCU) -c $(PROGRAMMER) -D -U flash:w:$<:i

clean:
	$(RM) $(TARGET) $(ELF_TARGET) $(OBJS) $(CORE_OBJS) $(CORE_LIB)
	$(RM) -r $(BUILD_DIR)

.PHONY: all flash clean
