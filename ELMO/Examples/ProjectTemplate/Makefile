##
## University of Bristol – Open Access Software Licence
## Copyright (c) 2016, The University of Bristol, a chartered
## corporation having Royal Charter number RC000648 and a charity
## (number X1121) and its place of administration being at Senate
## House, Tyndall Avenue, Bristol, BS8 1TH, United Kingdom.
## All rights reserved
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##
## 1. Redistributions of source code must retain the above copyright
## notice, this list of conditions and the following disclaimer.
##
## 2. Redistributions in binary form must reproduce the above
## copyright notice, this list of conditions and the following
## disclaimer in the documentation and/or other materials provided
## with the distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
## COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
## INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
## SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
## STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
## OF THE POSSIBILITY OF SUCH DAMAGE.
##
## Any use of the software for scientific publications or commercial
## purposes should be reported to the University of Bristol
## (OSI-notifications@bristol.ac.uk and quote reference 2668). This is
## for impact and usage monitoring purposes only.
##
## Enquiries about further applications and development opportunities
## are welcome. Please contact elisabeth.oswald@bristol.ac.uk
##


# Remove to display makefile output
Q		:= @

# Target main project.
BINARY = main

# Depdent projects.
DEP = dep1 dep2 # Change this list to the dependencies.
DEPOBJ:=$(addsuffix '.o',${DEP})
DEPD:=$(addsuffix '.d',${DEP})

VECTORS = vector.o
FP_FLAGS	?= -msoft-float
ARCH_FLAGS	= -mthumb -mcpu=cortex-m0 $(FP_FLAGS)

###############################################################################
# Executables

PREFIX		?= arm-none-eabi
CC		:= $(PREFIX)-gcc
LD		:= $(PREFIX)-gcc
OBJCOPY		:= $(PREFIX)-objcopy
OBJDUMP		:= $(PREFIX)-objdump

###############################################################################
# Source files

LDSCRIPT	= $(BINARY).ld
OBJS		+= $(BINARY).o
ELMOASMFUNCTIONS = elmoasmfunctions.o


###############################################################################
# C flags

CFLAGS += -Os -g
CFLAGS += -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS += -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS += -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C & C++ preprocessor common flags

CPPFLAGS	+= -MD
CPPFLAGS	+= -Wall -Wundef

###############################################################################
# Linker flags

LDFLAGS		+= --static -nostartfiles
LDFLAGS		+= -T$(LDSCRIPT)
LDFLAGS		+= -Wl,-Map=$(*).map
LDFLAGS		+= -Wl,--gc-sections

###############################################################################
# Used libraries

LDLIBS		+= -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

###############################################################################
###############################################################################
###############################################################################

all: elf bin list
elf: $(BINARY).elf 
bin: $(BINARY).bin
list: $(BINARY).list

images: $(BINARY).images

$(BINARY).images: $(BINARY).bin $(BINARY).list $(BINARY).map

$(BINARY).bin: $(BINARY).elf
	$(Q)$(OBJCOPY) -Obinary $(BINARY).elf $(BINARY).bin

$(BINARY).list: $(BINARY).elf
	$(Q)$(OBJDUMP) -S $(BINARY).elf > $(BINARY).list

$(BINARY).elf $(BINARY).map: $(OBJS) $(LDSCRIPT)
	$(Q)$(LD) $(LDFLAGS) $(VECTORS) $(ARCH_FLAGS) $(BINARY).S $(OBJS) ${DEPOBJ} $(ELMOASMFUNCTIONS) $(LDLIBS) -o $(BINARY).elf

$(BINARY).o: $(BINARY).c ${DEP}
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $(BINARY).o -c $(BINARY).c

$(DEP): $@.c
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -c $@.c

clean:
	$(Q)$(RM) $(BINARY).o ${DEPOBJ} ${DEPD} $(BINARY).d $(BINARY).elf $(BINARY).bin $(BINARY).list $(BINARY).map *~
