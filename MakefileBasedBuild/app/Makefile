# ----------------------------------------------------------------------------
#         ATMEL Microcontroller Software Support 
# ----------------------------------------------------------------------------
# Copyright (c) 2010, Atmel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without

# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice,
# this list of conditions and the disclaimer below.
#
# Atmel's name may not be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

#   Makefile for compiling the ADC12 Example project

#-------------------------------------------------------------------------------
#        User-modifiable options
#-------------------------------------------------------------------------------

# Chip & board used for compilation
# (can be overriden by adding CHIP=chip and BOARD=board to the command-line)
CHIP  = sam3x8
BOARD = sam3x_ek

# Defines which are the available memory targets for the SAM3S-EK board.
MEMORIES = flash #sram

# Trace level used for compilation
# (can be overriden by adding TRACE_LEVEL=#number to the command-line)
# TRACE_LEVEL_DEBUG      5
# TRACE_LEVEL_INFO       4
# TRACE_LEVEL_WARNING    3
# TRACE_LEVEL_ERROR      2
# TRACE_LEVEL_FATAL      1
# TRACE_LEVEL_NO_TRACE   0
TRACE_LEVEL = 4

# Optimization level, put in comment for debugging
OPTIMIZATION = -O2

# Output file basename
OUTPUT = test_$(BOARD)_$(CHIP)

# Output directories
BIN = bin
OBJ = obj

#-------------------------------------------------------------------------------
#		Tools
#-------------------------------------------------------------------------------

# Tool suffix when cross-compiling
CROSS_COMPILE = arm-none-eabi-

# Libraries
LIBRARIES = ../Atmel/sam3x/sam3x-ek/libraries
# Chip library directory
CHIP_LIB = $(LIBRARIES)/libchip_sam3x
# Board library directory
BOARD_LIB = $(LIBRARIES)/libboard_sam3x-ek
# USB library directory
USB_LIB = $(LIBRARIES)/usb

LIBS = -Wl,--start-group -lgcc -lc -lchip_$(CHIP)_gcc_rel -lboard_$(BOARD)_gcc_rel -Wl,--end-group

LIB_PATH = -L$(CHIP_LIB)/lib
LIB_PATH += -L$(BOARD_LIB)/lib
LIB_PATH+=-L=/lib/thumb2
LIB_PATH+=-L=/../lib/gcc/arm-none-eabi/4.4.1/thumb2

# Compilation tools
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
GDB = $(CROSS_COMPILE)gdb
NM = $(CROSS_COMPILE)nm

# Flags
INCLUDES  = -I$(CHIP_LIB)
INCLUDES += -I$(BOARD_LIB)
INCLUDES += -I$(LIBRARIES)
INCLUDES += -I.

CFLAGS += -fno-strict-aliasing
CFLAGS += -Wall -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int
CFLAGS += -Werror-implicit-function-declaration -Wmain -Wparentheses
CFLAGS += -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs
CFLAGS += -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef
CFLAGS += -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings
CFLAGS += -Wsign-compare -Waggregate-return -Wstrict-prototypes
CFLAGS += -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations
CFLAGS += -Wredundant-decls -Wnested-externs -Winline -Wlong-long
CFLAGS += -Wno-unused
#CFLAGS += -Wunreachable-code
CFLAGS += -Wcast-align
#CFLAGS += -Wmissing-noreturn
#CFLAGS += -Wconversion

# To reduce application size use only integer printf function.
CFLAGS += -Dprintf=iprintf

# -mlong-calls  -Wall
CFLAGS += --param max-inline-insns-single=500 -mcpu=cortex-m3 -mthumb -ffunction-sections
CFLAGS += -g $(OPTIMIZATION) $(INCLUDES) -D$(CHIP) -DTRACE_LEVEL=$(TRACE_LEVEL)
ASFLAGS = -mcpu=cortex-m3 -mthumb -Wall -g $(OPTIMIZATION) $(INCLUDES) -D$(CHIP) -D__ASSEMBLY__
LDFLAGS= -mcpu=cortex-m3 -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=ResetException -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols
#LD_OPTIONAL=-Wl,--print-gc-sections -Wl,--stats

#-------------------------------------------------------------------------------
#		Files
#-------------------------------------------------------------------------------

# Directories where source files can be found

VPATH += ./

# Objects built from C source files
C_OBJECTS += main.o BT.o I2C.o baro.o fwk.o als.o mag.o accel.o hygro.o LEDs.o printf.o dbg.o Audio.o coop.o
C_OBJECTS += SD.o simpleOgg.o RTC.o btSDP.o btRFCOMM.o sgBuf.o btL2CAP.o eliza.o capsense.o btA2DP.o ADK.o
C_OBJECTS += usb_drv.o f_ff.o f_sdDisk.o v_bitwise.o v_codebook.o v_dsp.o v_floor0.o accessory.o usbh.o
C_OBJECTS += v_floor1.o v_floor_lookup.o v_framing.o v_info.o v_mapping0.o v_mdct.o v_misc.o v_res012.o
C_OBJECTS += v_vorbisfile.o

# Append OBJ and BIN directories to output filename
OUTPUT := $(BIN)/$(OUTPUT)
LIBS += $(A_OBJECTS)
DEPS :=

#-------------------------------------------------------------------------------
#		Rules
#-------------------------------------------------------------------------------

all: libchip_$(CHIP)_gcc_rel.a libboard_$(BOARD)_gcc_rel.a $(BIN) $(OBJ) $(MEMORIES)

libchip_$(CHIP)_gcc_rel.a:
	$(MAKE) -C $(CHIP_LIB)/build/gcc $(CHIP)

libboard_$(BOARD)_gcc_rel.a:
	$(MAKE) -C $(BOARD_LIB)/build/gcc $(BOARD)

$(BIN) $(OBJ): $(A_OBJECTS)
	mkdir $@ || true

%.d:

define RULES
C_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(C_OBJECTS))
ASM_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(ASM_OBJECTS))
DEPS += $(addprefix $(OBJ)/$(1)_, $(C_OBJECTS:.o=.d))

$(1): $$(ASM_OBJECTS_$(1)) $$(C_OBJECTS_$(1))
	@echo mkdir -p $$(dir $$@)
	@$(CC) $(LIB_PATH) $(LDFLAGS) $(LD_OPTIONAL) -T"$(BOARD_LIB)/resources/gcc/$(CHIP)/$$@.ld" -Wl,-Map,$(OUTPUT)-$$@.map -o $(OUTPUT)-$$@.elf $$^ $(LIBS)
	$(NM) $(OUTPUT)-$$@.elf >$(OUTPUT)-$$@.elf.txt
	$(OBJCOPY) -O binary $(OUTPUT)-$$@.elf $(OUTPUT)-$$@.bin
	$(OBJDUMP) -d $(OUTPUT)-$$@.elf > $(OUTPUT)-$$@.lst
	$(SIZE) $$^ $(OUTPUT)-$$@.elf

$$(C_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.c
	@mkdir -p $$(dir $$@)
	@$(CC) $(CFLAGS) -D$(1) -c -MD -MT $$@ -MF $$(@:%o=%d) -o $$@ $$<

$$(ASM_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.S
	@mkdir -p $$(dir $$@)
	@$(CC) $(ASFLAGS) -D$(1) -c -MD -MT $$@ -MF $$(@:%o=%d) -o $$@ $$<

debug_$(1): $(1)
	$(GDB) -x "$(BOARD_LIB)/resources/gcc/$(BOARD)_$(1).gdb" -ex "reset" -readnow -se $(OUTPUT)-$(1).elf
endef

$(foreach MEMORY, $(MEMORIES), $(eval $(call RULES,$(MEMORY))))

clean:
	rm -fR $(OBJ) $(BIN)

ifeq ($(filter $(MAKECMDGOALS), clean), )
-include $(DEPS)
endif
