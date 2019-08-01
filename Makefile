# definitions
TARGET = firmware
MCU = atmega168
SRC = main.c
CFLAGS = -mmcu=$(MCU) -Os -Wall -Werror

# executables
CC      = avr-gcc
OBJCOPY = avr-objcopy
SIZE    = avr-size --format=avr --mcu=$(MCU)

# generate list of objects
CFILES    = $(filter %.c, $(SRC))
OBJ       = $(CFILES:.c=.o)

# compile all files
all: $(TARGET).hex

# remove compiled files
clean:
	rm -f $(TARGET).hex $(TARGET).elf $(TARGET).o
	rm -f *.hex *.elf *.o

# objects from c files
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# elf file
$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET).elf $(OBJ)

# hex file
$(TARGET).hex: $(TARGET).elf
	rm -f $(TARGET).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	$(SIZE) $(TARGET).elf
