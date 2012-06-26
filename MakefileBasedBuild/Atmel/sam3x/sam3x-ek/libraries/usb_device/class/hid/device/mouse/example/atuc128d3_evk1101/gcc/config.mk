# Hey Emacs, this is a -*- makefile -*-

# The purpose of this file is to define the build configuration variables used
# by the generic Makefile. See Makefile header for further information.

# Copyright (c) 2009 Atmel Corporation. All rights reserved. 
#
# Redistribution and use in source and binary forms, with or without 
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer. 
#
# 2. Redistributions in binary form must reproduce the above copyright notice, 
# this list of conditions and the following disclaimer in the documentation 
# and/or other materials provided with the distribution.
#
# 3. The name of Atmel may not be used to endorse or promote products derived 
# from this software without specific prior written permission.  
#
# 4. This software may only be redistributed and used in connection with an Atmel 
# AVR product. 
#
# THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED 
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE 
# EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR 
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
#

# Base paths
PRJ_PATH = ../../../../../../../../../..
APPS_PATH = $(PRJ_PATH)/common/applications
BRDS_PATH = $(PRJ_PATH)/avr32/boards
COMP_PATH = $(PRJ_PATH)/avr32/components
DRVR_PATH = $(PRJ_PATH)/avr32/drivers
SERV_PATH = $(PRJ_PATH)/avr32/services
UTIL_PATH = $(PRJ_PATH)/avr32/utils
BRDS_COM_PATH = $(PRJ_PATH)/common/boards
COMP_COM_PATH = $(PRJ_PATH)/common/components
SERV_COM_PATH = $(PRJ_PATH)/common/services
UTIL_COM_PATH = $(PRJ_PATH)/common/utils

THRD_PRJ_PATH  = $(PRJ_PATH)
THRD_PATH      = $(THRD_PRJ_PATH)/third_party
THRD_APPS_PATH = $(THRD_PATH)/applications
THRD_BRDS_PATH = $(THRD_PATH)/boards
THRD_COMP_PATH = $(THRD_PATH)/components
THRD_DRVR_PATH = $(THRD_PATH)/drivers
THRD_SERV_PATH = $(THRD_PATH)/services
THRD_UTIL_PATH = $(THRD_PATH)/utils

# CPU architecture: {ap|ucr1|ucr2}
ARCH = ucr3

# Part: {none|ap7xxx|uc3xxxxx}
PART = uc3b0128


# Flash memories: [{cfi|internal}@address,size]...
FLASH = internal@0x80000000,128Kb

# Clock source to use when programming: [{xtal|extclk|int}]
PROG_CLOCK = int

# Device/Platform/Board include path
PLATFORM_INC_PATH = \
  $(BRDS_PATH)/

# Target name: {*.a|*.elf}
TARGET = $(PART)-usb_device_mouse.elf

# Definitions: [-D name[=definition]...] [-U name...]
# Things that might be added to DEFS:
#   BOARD             Board used: see $(BRDS_PATH)/board.h
#   EXT_BOARD         Extension board used (if any): see $(BRDS_PATH)/board.h
DEFS = -D BOARD=EVK1101 -D __AVR32_UC128D3__ -U __AVR32_UC3B0128__

# Include path
INC_PATH = \
  $(UTIL_PATH)/ \
  $(UTIL_PATH)/preprocessor/ \
  $(UTIL_COM_PATH)/ \
  $(BRDS_COM_PATH)/ \
  $(DRVR_PATH)/gpio/ \
  $(DRVR_PATH)/intc/ \
  $(DRVR_PATH)/eic/ \
  $(DRVR_PATH)/pm/ \
  $(DRVR_PATH)/scif/ \
  $(DRVR_PATH)/flashcdw/ \
  $(DRVR_PATH)/usbc/ \
  $(COMP_PATH)/joystick/skrhabe010/ \
  $(SERV_COM_PATH)/basic/clock/ \
  $(SERV_COM_PATH)/basic/sleepmgr/ \
  $(SERV_COM_PATH)/usb/ \
  $(SERV_COM_PATH)/usb/udc/ \
  $(SERV_COM_PATH)/usb/class/hid/ \
  $(SERV_COM_PATH)/usb/class/hid/device/ \
  $(SERV_COM_PATH)/usb/class/hid/device/mouse/ \
  ../../ \
  ../ \

#  $(DRVR_PATH)/pm/pm.c \
# C source files
CSRCS = \
  $(DRVR_PATH)/intc/intc.c \
  $(DRVR_PATH)/flashcdw/flashcdw.c \
  $(DRVR_PATH)/gpio/gpio.c \
  $(DRVR_PATH)/eic/eic.c \
  $(DRVR_PATH)/usbc/usbc_device.c \
  $(BRDS_PATH)/evk1101/init.c \
  $(BRDS_PATH)/evk1101/led_uc3d.c \
  $(SERV_COM_PATH)/basic/clock/uc3d/sysclk.c \
  $(SERV_COM_PATH)/basic/clock/uc3d/pll.c \
  $(SERV_COM_PATH)/basic/clock/uc3d/osc.c \
  $(SERV_COM_PATH)/basic/sleepmgr/uc3/sleepmgr.c \
  $(SERV_COM_PATH)/usb/udc/udc.c \
  $(SERV_COM_PATH)/usb/class/hid/device/udi_hid.c \
  $(SERV_COM_PATH)/usb/class/hid/device/mouse/udi_hid_mouse.c \
  $(SERV_COM_PATH)/usb/class/hid/device/mouse/udi_hid_mouse_desc.c \
  ../../main.c \
  ../ui.c \

# Assembler source files
ASSRCS = \
  $(UTIL_PATH)/startup/trampoline_uc3.S \
  $(DRVR_PATH)/intc/exception.S

# Library path
LIB_PATH = 

# Libraries to link with the project
LIBS = 

# Linker script file if any
LINKER_SCRIPT = $(UTIL_PATH)/linker_scripts/atuc_d/128/gcc/link_uc128dx.lds

# Options to request or suppress warnings: [-fsyntax-only] [-pedantic[-errors]] [-w] [-Wwarning...]
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
WARNINGS = -Wall

# Options for debugging: [-g]...
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
DEBUG = -g

# Options that control optimization: [-O[0|1|2|3|s]]...
# For further details, refer to the chapter "GCC Command Options" of the GCC manual.
OPTIMIZATION = -Os -fsection-anchors -ffunction-sections

# Extra flags to use when preprocessing
CPP_EXTRA_FLAGS =

# Extra flags to use when compiling
C_EXTRA_FLAGS =

# Extra flags to use when assembling
AS_EXTRA_FLAGS =

# Extra flags to use when linking
LD_EXTRA_FLAGS = -Wl,--direct-data,--gc-sections,-e,_trampoline

# Documentation path
DOC_PATH = \
  ../doc/

# Documentation configuration file
DOC_CFG = \
  ../doxyfile.doxygen
