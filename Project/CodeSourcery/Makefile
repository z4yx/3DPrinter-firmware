# Makefile to create the binary 
# by Qibo Zhang (Michael) China
#
# mail: sudazqb@163.com
# MSN: zhangqibo_1985@hotmail.com
#DATE:2007-12-19
#
# modifyied from Martin Thomas's luminary makefile
#


# Hey Emacs, this is a -*- makefile -*-

#
#  This file gets included by the makefiles in
#  the examples - it's not a full makefile since
#  SRC is defined in the "real" makefiles and not here

#
# WinARM template makefile 
# by Martin Thomas, Kaiserslautern, Germany 
# <eversmith@heizung-thomas.de>
#
# based on the WinAVR makefile written by Eric B. Weddington, Jörg Wunsch, et al.
# Released to the Public Domain
# Please read the make user manual!
#
#
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make program = Download the hex file to the device
#
# (TODO: make filename.s = Just compile filename.c into the assembler code only)
#
# To rebuild project do "make clean" then "make all".
#
# Changelog:
# - 17. Feb. 2005  - added thumb-interwork support (mth)
# - 28. Apr. 2005  - added C++ support (mth)
# - 29. Arp. 2005  - changed handling for lst-Filename (mth)
# -  1. Nov. 2005  - exception-vector placement options (mth)
# - 15. Nov. 2005  - added library-search-path (EXTRA_LIB...) (mth)
# -  2. Dec. 2005  - fixed ihex and binary file extensions (mth)
# - 22. Feb. 2006  - added AT91LIBNOWARN setting (mth)
# - 19. Apr. 2006  - option FLASH_TOOL (default lpc21isp); variable IMGEXT (mth)
# - 23. Jun. 2006  - option USE_THUMB_MODE -> THUMB/THUMB_IW
# -  3. Aug. 2006  - added -ffunction-sections -fdata-sections to CFLAGS
#                    and --gc-sections to LDFLAGS. Only availble for gcc 4 (mth)
# -  4. Aug. 2006  - pass SUBMDL-define to frontend (mth)
# - 11. Nov. 2006  - FLASH_TOOL-config, TCHAIN-config (mth)

# Toolchain prefix (i.e arm-none-eabi -> arm-none-eabi-gcc.exe)
TCHAIN = arm-none-eabi

# MCU name and submodel
MCU      = cortex-m3
SUBMDL   = stm32f10x_md

STM32LIB_CHIP_TYPE = $(shell echo $(SUBMDL) | tr a-z A-Z)

# must be yes - only THUMB2 on M3 no ARM:
USE_THUMB_MODE = YES

## Create ROM-Image (final)
RUN_MODE=ROM_RUN
## Create RAM-Image (debugging) 
##( not used: example does not fit in AT91SAM7S64 RAM )
#RUN_MODE=RAM_RUN

## Exception-Vector placement not used so far in M3-examples
## (placement settings ignored when using "RAM_RUN")
## - Exception vectors in ROM:
#VECTOR_LOCATION=VECTORS_IN_ROM
## - Exception vectors in RAM:
##VECTOR_LOCATION=VECTORS_IN_RAM

# Target file name (without extension).
TARGET = main

#source file path
SRC_DIR = ../

# List C source files here. (C dependencies are automatically generated.)
# use file-extension c for "c-only"-files

# SRC files
SRC += $(wildcard $(SRC_DIR)../Libraries/STM32F10x_StdPeriph_Driver/src/*.c)
SRC += $(wildcard $(SRC_DIR)../Project/src/*.c)
SRC += $(wildcard $(SRC_DIR)../Project/src/lowlevel/*.c)
SRC += $(wildcard $(SRC_DIR)../Project/src/fatfs/*.c)

# List C source files here which must be compiled in ARM-Mode.
# use file-extension c for "c-only"-files
SRCARM  = 

# List C++ source files here.
# use file-extension cpp for C++-files (use extension .cpp)
CPPSRC = 

# List C++ source files here which must be compiled in ARM-Mode.
# use file-extension cpp for C++-files (use extension .cpp)
CPPSRCARM = 

# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC = startup_$(SUBMDL).S

# List Assembler source files here which must be assembled in ARM-Mode..
ASRCARM  = 

## Output format. (can be ihex or binary)
## (binary i.e. for openocd, lmiflash and SAM-BA, hex i.e. for lpc21isp and uVision)
#FORMAT = ihex
FORMAT = binary

## Configre flash tool - supported: UVISION, OPENOCD, LMIFLASH
## use FORMAT = binary for OpenOCD and lmiflash
## use FORMAT = ihex for uVision
#FLASH_TOOL = UVISION
#FLASH_TOOL = OPENOCD
FLASH_TOOL = LMIFLASH

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
# OPT = s
OPT = 0

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
#DEBUG = stabs
DEBUG = dwarf-2

# Path to Linker-Scripts
LINKERSCRIPTPATH = ./

# List any extra directories to look for include files here.
#     Each directory must be seperated by a space.
#EXTRAINCDIRS = ../.. ../../utils ../../lib_src
EXTRAINCDIRS = $(SRC_DIR)../Project/include
EXTRAINCDIRS += $(SRC_DIR)../Project/src/fatfs
EXTRAINCDIRS += $(SRC_DIR)../Libraries/STM32F10x_StdPeriph_Driver/inc 
EXTRAINCDIRS += $(SRC_DIR)../Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
EXTRAINCDIRS += $(SRC_DIR)../Libraries/CMSIS/CM3/CoreSupport

# List any extra directories to look for library files here.
#     Each directory must be seperated by a space.
EXTRA_LIBDIRS = ../

## Using the Atmel AT91_lib produces warning with
## the default warning-levels. 
## yes - disable these warnings; no - keep default settings
#AT91LIBNOWARN = yes
AT91LIBNOWARN = no

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options for C here
CDEFS =  -D$(RUN_MODE) -Dgcc

# Place -I options here
CINCS =

# Place -D or -U options for ASM here
ADEFS =  -D$(RUN_MODE)

ifdef VECTOR_LOCATION
CDEFS += -D$(VECTOR_LOCATION)
ADEFS += -D$(VECTOR_LOCATION)
endif

CDEFS += -D$(STM32LIB_CHIP_TYPE) -DUSE_STDPERIPH_DRIVER
ADEFS += -D$(STM32LIB_CHIP_TYPE)

# Compiler flags.

ifeq ($(USE_THUMB_MODE),YES)
THUMB    = -mthumb
THUMB_IW = -mthumb-interwork
else 
THUMB    = 
THUMB_IW = 
endif

#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
#
# Flags for C and C++ (arm-elf-gcc/arm-elf-g++)
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
# CFLAGS += -mfix-cortex-m3-ldrd
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wimplicit 
# disabled here to avoid warning from game-source:
# CFLAGS += -Wcast-align
CFLAGS += -Wpointer-arith -Wswitch
CFLAGS += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
CFLAGS += -Wa,-adhlns=$(subst $(suffix $<),.lst,$<) 
CFLAGS += $(patsubst %,-I%,$(EXTRAINCDIRS))

# flags only for C
CONLYFLAGS += -Wnested-externs 
CONLYFLAGS += $(CSTANDARD)

ifneq ($(AT91LIBNOWARN),yes)
#AT91-lib warnings with:
CFLAGS += -Wcast-qual
CONLYFLAGS += -Wmissing-prototypes 
CONLYFLAGS += -Wstrict-prototypes
CONLYFLAGS += -Wmissing-declarations
endif

# flags only for C++ (arm-elf-g++)
CPPFLAGS = -fno-rtti -fno-exceptions

# Assembler flags.
#  -Wa,...:    tell GCC to pass this to the assembler.
#  -ahlns:     create listing
#  -g$(DEBUG): have the assembler create line number information
ASFLAGS = $(ADEFS) -Wa,-adhlns=$(<:.S=.lst),--g$(DEBUG)


#Additional libraries.

# Extra libraries
#    Each library-name must be seperated by a space.
#    To add libxyz.a, libabc.a and libefsl.a: 
#    EXTRA_LIBS = xyz abc efsl
#EXTRA_LIBS = efsl
EXTRA_LIBS = 

#Support for newlibc-lpc (file: libnewlibc-lpc.a)
#NEWLIBLPC = -lnewlib-lpc

MATH_LIB = -lm

# CPLUSPLUS_LIB = -lstdc++

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
#    --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(TARGET).map -Wl,-cref -Wl,--gc-sections
LDFLAGS += $(NEWLIBLPC) $(MATH_LIB)
LDFLAGS += -lc -lgcc 
LDFLAGS += $(CPLUSPLUS_LIB)
LDFLAGS += $(patsubst %,-L%,$(EXTRA_LIBDIRS))
LDFLAGS += $(patsubst %,-l%,$(EXTRA_LIBS))

# Set Linker-Script Depending On Selected Memory and Controller
ifeq ($(RUN_MODE),RAM_RUN)
LDFLAGS +=-T$(LINKERSCRIPTPATH)$(SUBMDL)-RAM.ld
else 
LDFLAGS +=-T$(LINKERSCRIPTPATH)$(SUBMDL)-ROM.ld
endif

# ---------------------------------------------------------------------------

# Define directories, if needed.
## DIRARM = c:/WinARM/
## DIRARMBIN = $(DIRAVR)/bin/
## DIRAVRUTILS = $(DIRAVR)/utils/bin/

# Define programs and commands.
SHELL = sh
CC = $(TCHAIN)-gcc
CPP = $(TCHAIN)-g++
AR = $(TCHAIN)-ar
OBJCOPY = $(TCHAIN)-objcopy
OBJDUMP = $(TCHAIN)-objdump
SIZE = $(TCHAIN)-size
NM = $(TCHAIN)-nm
REMOVE = rm -f
REMOVEDIR = rm -r
COPY = cp

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = "-------- begin (mode: $(RUN_MODE)) --------"
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_ARM = "Compiling C (ARM-only):"
MSG_COMPILINGCPP = Compiling C++:
MSG_COMPILINGCPP_ARM = "Compiling C++ (ARM-only):"
MSG_ASSEMBLING = Assembling:
MSG_ASSEMBLING_ARM = "Assembling (ARM-only):"
MSG_CLEANING = Cleaning project:
MSG_FORMATERROR = Can not handle output-format
MSG_LPC21_RESETREMINDER = You may have to bring the target in bootloader-mode now.

# Define all object files.
COBJ      = $(SRC:.c=.o) 
AOBJ      = $(ASRC:.S=.o)
COBJARM   = $(SRCARM:.c=.o)
AOBJARM   = $(ASRCARM:.S=.o)
CPPOBJ    = $(CPPSRC:.cpp=.o) 
CPPOBJARM = $(CPPSRCARM:.cpp=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(ASRCARM:.S=.lst) $(SRC:.c=.lst) $(SRCARM:.c=.lst)
LST += $(CPPSRC:.cpp=.lst) $(CPPSRCARM:.cpp=.lst)

# Compiler flags to generate dependency files.
### GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS  = -mcpu=$(MCU) $(THUMB_IW) -I. $(CFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -mcpu=$(MCU) $(THUMB_IW) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: begin gccversion sizebefore build sizeafter finished end

ifeq ($(FORMAT),ihex)
build: elf hex lss sym
hex: $(TARGET).hex
IMGEXT=hex
else 
ifeq ($(FORMAT),binary)
build: elf bin lss sym
bin: $(TARGET).bin
IMGEXT=bin
else 
$(error "$(MSG_FORMATERROR) $(FORMAT)")
endif
endif

elf: $(TARGET).elf
lss: $(TARGET).lss 
sym: $(TARGET).sym

# Eye candy.
begin:
	@echo --
	@echo $(MSG_BEGIN)

finished:
	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)
	@echo


# Display size of file.
HEXSIZE = $(SIZE) --target=$(FORMAT) $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf
sizebefore:
#	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi
#	echo $(MSG_SIZE_AFTER)
#	$(ELFSIZE)

# Display compiler version information.
gccversion : 
	@$(CC) --version


# Program the device with Keil's uVision (needs configured uVision-Workspace). 
program: $(TARGET).$(IMGEXT)
	@echo
ifeq ($(FLASH_TOOL),UVISION)
	@echo "Flash-programming with uVision"
	C:\Keil\uv3\Uv3.exe -f ../../tools/uvisionflash811.uv2 -ouvisionflash.txt
else
ifeq ($(FLASH_TOOL),OPENOCD)
	@echo "Flash-programming with OpenOCD"
	cp $(TARGET).bin ../../tools/tmpflash.bin
	cd ../../tools && make program
	rm ../../tools/tmpflash.bin
else
ifeq ($(FLASH_TOOL),LMIFLASH)
	@echo "Flash-programming with lmiflash"
	../../tools/lmiflash.exe -f $(TARGET).$(IMGEXT) -v -r
endif
endif
endif

# Create final output file (.hex) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) $< $@
	
# Create final output file (.bin) from ELF output file.
%.bin: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O $(FORMAT) $< $@


# Create extended listing file from ELF output file.
# testing: option -C
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -D $< > $@


# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@


# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM)
%.elf:  $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(THUMB) $(ALL_CFLAGS) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM) --output $@ $(LDFLAGS)
#	arm-elf-gcc $(THUMB) $(ALL_CFLAGS) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM) --output $@ $(LDFLAGS)
#	$(CPP) $(THUMB) $(ALL_CFLAGS) $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM) --output $@ $(LDFLAGS)
## mist arm-elf-ld -Map=$(TARGET).map --cref --gc-sections $(AOBJARM) $(AOBJ) $(COBJARM) $(COBJ) $(CPPOBJ) $(CPPOBJARM) --output $@

# Compile: create object files from C source files. ARM/Thumb
$(COBJ) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(THUMB) $(ALL_CFLAGS) $(CONLYFLAGS) $< -o $@ 

# Compile: create object files from C source files. ARM-only
$(COBJARM) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING_ARM) $<
	$(CC) -c $(ALL_CFLAGS) $(CONLYFLAGS) $< -o $@ 

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJ) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP) $<
	$(CPP) -c $(THUMB) $(ALL_CFLAGS) $(CPPFLAGS) $< -o $@ 

# Compile: create object files from C++ source files. ARM-only
$(CPPOBJARM) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP_ARM) $<
	$(CPP) -c $(ALL_CFLAGS) $(CPPFLAGS) $< -o $@ 


# Compile: create assembler files from C source files. ARM/Thumb
## does not work - TODO - hints welcome
##$(COBJ) : %.s : %.c
##	$(CC) $(THUMB) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files. ARM/Thumb
$(AOBJ) : %.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(THUMB) $(ALL_ASFLAGS) $< -o $@


# Assemble: create object files from assembler source files. ARM-only
$(AOBJARM) : %.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING_ARM) $<
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean: begin clean_list finished end


clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).bin
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).a90
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(COBJ)
	$(REMOVE) $(CPPOBJ)
	$(REMOVE) $(AOBJ)
	$(REMOVE) $(COBJARM)
	$(REMOVE) $(CPPOBJARM)
	$(REMOVE) $(AOBJARM)
	$(REMOVE) $(LST)
	$(REMOVE) $(SRC:.c=.s)
	$(REMOVE) $(SRC:.c=.d)
	$(REMOVE) $(SRCARM:.c=.s)
	$(REMOVE) $(SRCARM:.c=.d)
	$(REMOVE) $(CPPSRC:.cpp=.s) 
	$(REMOVE) $(CPPSRC:.cpp=.d)
	$(REMOVE) $(CPPSRCARM:.cpp=.s) 
	$(REMOVE) $(CPPSRCARM:.cpp=.d)
	$(REMOVE) .dep/*
#	$(REMOVEDIR) .dep
# remove files created by uVision during flash programming
	$(REMOVE) $(TARGET).hex.plg
	$(REMOVE) uvisionflash.txt


# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex bin lss sym clean clean_list program

