CC := avr-gcc
OBJCOPY := avr-objcopy
CFLAGS := -Os
# CFLAGS := -Wall -Wextra -Wpedantic -Werror -Os
LDFLAGS := -Wl,--print-memory-usage

SRC_DIR := src
INC_DIR := include
LIB_DIR := lib
OBJ_DIR := obj
BIN_DIR := bin

# Configurable variables
DEVICE := attiny10
F_CPU = 8000000
PROGRAMMER := usbasp
PORT := usb

CFLAGS += -mmcu=$(DEVICE) -DF_CPU=$(F_CPU)
CFLAGS += -I$(INC_DIR)

OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/$(SRC_DIR)/%.o, $(wildcard $(SRC_DIR)/*.c))

# light_ws2812
LIGHT_WS812_PATH = $(LIB_DIR)/light_ws2812/light_ws2812_AVR/Light_WS2812
LIGHT_WS812_CFLAGS := -I$(LIGHT_WS812_PATH)
CFLAGS += $(LIGHT_WS812_CFLAGS)
OBJS += $(patsubst $(LIGHT_WS812_PATH)/%.c, $(OBJ_DIR)/$(LIGHT_WS812_PATH)/%.o, $(wildcard $(LIGHT_WS812_PATH)/*.c))

TARGET_ELF := $(BIN_DIR)/firmware.elf
TARGET_HEX := $(BIN_DIR)/firmware.hex

.PHONY: all clean flash

all: $(TARGET_HEX)

$(TARGET_ELF): $(OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^
	avr-size --format=avr --mcu=$(DEVICE) $<

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR)

flash: $(TARGET_HEX)
	avrdude -P $(PORT) -c $(PROGRAMMER) -p $(DEVICE) -U flash:w:$<:i
