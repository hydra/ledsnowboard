
# The name of your project (used to name the compiled .hex file)
TARGET = LedSnowboard

CFLAGS_TEENSY_31 = -DF_CPU=96000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -D__MK20DX256__
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mcpu=cortex-m4 -fsingle-precision-constant -Wdouble-promotion

# configurable options
OPTIONS = $(CFLAGS_TEENSY_31) $(CFLAGS_CORTEX_M4) 

# options needed by many Arduino libraries
OPTIONS += -DARDUINO=105


#************************************************************************
# Location of Teensyduino utilities, Toolchain, and Arduino Libraries.
# To use this makefile without Arduino, copy the resources from these
# locations and edit the pathnames.  The rest of Arduino is not needed.
#************************************************************************

# path location for Teensy Loader, teensy_post_compile and teensy_reboot
#TOOLSPATH = ../../../tools   # on Linux
#TOOLSPATH = ../../../tools/avr/bin   # on Mac or Windows
TOOLSPATH = $(TEENSYDUINO_HOME)/hardware/tools

# path location for Arduino libraries (currently not used)
#LIBRARYPATH = ../../../../libraries
LIBRARYPATH = $(TEENSYDUINO_HOME)/libraries

# path location for the arm-none-eabi compiler
#COMPILERPATH = ../../../tools/arm-none-eabi/bin
COMPILERPATH = $(TEENSYDUINO_HOME)/hardware/tools/arm-none-eabi/bin

COREPATH = $(TEENSYDUINO_HOME)/hardware/teensy/cores/teensy3

LIBRARY_FLAGS = \
    -I$(LIBRARYPATH)/OctoWS2811 \
    -I$(LIBRARYPATH)/Wire \
    -I../../Libraries/I2Cdev \
    -I../../Libraries/MPU6050 \

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -Wall -g -Os -nostdlib -MMD $(OPTIONS) -I. -I$(COREPATH) $(LIBRARY_FLAGS)

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS = -I$(COREPATH)

# linker options
LDFLAGS = -Os -Wl,--gc-sections -mcpu=cortex-m4 -mthumb -T$(COREPATH)/mk20dx256.ld

# additional libraries to link
LIBS = -lm


# names for the compiler programs
CC = $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX = $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY = $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE = $(abspath $(COMPILERPATH))/arm-none-eabi-size

ifeq ($(OS),Windows_NT)
#cygwin
SYSTEM_PWD = $(shell cygpath -w -a .)
else
#osx/linux
SYSTEM_PWD = $(abspath $(shell pwd))
endif
SRC_TEENSY_C = \
        mk20dx128.c \
        pins_teensy.c \
        nonstd.c \
        analog.c \
        serial1.c \
        usb_desc.c \
        usb_dev.c \
        usb_mem.c \
        usb_serial.c \
        yield.c
        
SRC_TEENSY_CPP = \
        Print.cpp \
        WString.cpp \
        usb_inst.cpp \

STANDARD_LIBRARIES = \
        $(LIBRARYPATH)/OctoWS2811/OctoWS2811.cpp \
        $(LIBRARYPATH)/Wire/Wire.cpp \
        ../../Libraries/I2Cdev/I2Cdev.cpp \
        ../../Libraries/MPU6050/MPU6050.cpp \

C_FILES := $(wildcard *.c)
CPP_FILES := $(wildcard *.cpp)
OBJS := \
        $(C_FILES:.c=.o) \
        $(CPP_FILES:.cpp=.o) \
        $(addprefix $(COREPATH)/, $(SRC_TEENSY_CPP:.cpp=.o)) \
        $(addprefix $(COREPATH)/, $(SRC_TEENSY_C:.c=.o)) \
        $(STANDARD_LIBRARIES:.cpp=.o) \


# the actual makefile rules (all .o files built by GNU make's default implicit rules)

all: $(TARGET).hex

$(TARGET).elf: $(OBJS) $(COREPATH)/mk20dx256.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@
ifeq ($(OS),Windows_NT)
	#cygwin
	# give the teensy loader a chance before teensy_post_compile looks for it
	$(abspath $(TOOLSPATH))/teensy &
	sleep 1 
else eq ($(OSTYPE),darwin13)
	open $(abspath $(TOOLSPATH))/teensy &
endif

	$(abspath $(TOOLSPATH))/teensy_post_compile -file=$(basename $@) -path="$(SYSTEM_PWD)" -tools=$(abspath $(TOOLSPATH))
	-$(abspath $(TOOLSPATH))/teensy_reboot


# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.d $(TARGET).elf $(TARGET).hex


