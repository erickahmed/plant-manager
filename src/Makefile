PROJECT = plant-manager

MCU = atmega328p
F_CPU = 16000000UL
PROGRAMMER = arduino
PORT ?= /dev/ttyACM0

CC = avr-gcc
LD = avr-ld
OC = avr-objcopy
AD = avrdude
CFLAGS = -Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)

SRCS = main.c moisture.cpp
HDRS = moisture.h config.h
OBJS = main.o moisture.o config.o
EXEC = moisture

TARGET = moisture.hex

$(TARGET) :$(OBJS)
