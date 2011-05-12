#	FreeRTOS.org V5.1.1 - Copyright (C) 2003-2008 Richard Barry.
#
#	This file is part of the FreeRTOS.org distribution.
#
#	FreeRTOS.org is free software; you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation; either version 2 of the License, or
#	(at your option) any later version.
#
#	FreeRTOS.org is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with FreeRTOS.org; if not, write to the Free Software
#	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#	A special exception to the GPL can be applied should you wish to distribute
#	a combined work that includes FreeRTOS.org, without being obliged to provide
#	the source code for any proprietary components.  See the licensing section 
#	of http://www.FreeRTOS.org for full details of how and when the exception
#	can be applied.
#
#	***************************************************************************
#	See http://www.FreeRTOS.org for documentation, latest information, license 
#	and contact details.  Please ensure to read the configuration and relevant 
#	port sections of the online documentation.
#	***************************************************************************


RTOS_SOURCE_DIR=../../FreeRTOS_Source


#CC=arm-elf-gcc
#OBJCOPY=arm-elf-objcopy
LDSCRIPT=lpc2368.ld

#PAT =/usr/local/arm_gc2/bin/./
PAT =../../TOOLCHAIN/bin/./
CC=$(PAT)arm-elf-gcc
OBJCOPY=$(PAT)arm-elf-objcopy
DISAMB=$(PAT)arm-elf-objdump
UKURAN=$(PAT)arm-elf-size
PENULIS= ../../modul/Penulis/./lpc_dbe

NAMA_FILE=420_medco

############# GPS ################
GPSDIR = ../../modul/nmealib/src
GPSMODULES = parse parser tok context time info gmath  
GPSOBJ = $(GPSMODULES:%=build/%.o) 
GPSLIB = lidah/libnmea.a
############# GPS ################


#LINKER_FLAGS=-mthumb -nostartfiles -Xlinker -o$(NAMA_FILE).elf -Xlinker -M -Xlinker -Map=rtosdemo.map 
LINKER_FLAGS=-mthumb -nostartfiles -Xlinker -o$(NAMA_FILE).elf -Xlinker -M -Xlinker -Map=$(NAMA_FILE).map 

OPTIM=-O1


CFLAGS= $(DEBUG) \
		$(OPTIM) \
		-I../../TOOLCHAIN/arm-elf/include/ \
		-T$(LDSCRIPT) \
		-I . \
		-I $(RTOS_SOURCE_DIR)/include \
		-I $(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC23xx \
		-I ../../modul/nmealib/include/ \
		-D ROWLEY_LPC23xx \
		-D THUMB_INTERWORK \
		-mcpu=arm7tdmi \
		-D PACK_STRUCT_END=__attribute\(\(packed\)\) \
		-D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) \
		-fomit-frame-pointer \
		-mthumb-interwork \

								
THUMB_SOURCE= \
		main.c \

#MODULES_GPS = parse parser tok context time info gmath  
GPS_SOURCE = \
#		../../app/gps/gps.c		\
#		../../modul/nmealib/src/parse.c \
#		../../modul/nmealib/src/parser.c \
#		../../modul/nmealib/src/context.c \
#		../../modul/nmealib/src/info.c \
#		../../modul/nmealib/src/gmath.c \
#		../../modul/nmealib/src/timegps.c \
#		../../modul/nmealib/src/tok.c \

ARM_SOURCE= \
#		$(RTOS_SOURCE_DIR)/portable/GCC/ARM7_LPC23xx/portISR.c \
#		../../modul/serial/serialISR.c \
#		../../GPIO/gpio_ISR.c 


THUMB_SOURCE += $(GPS_SOURCE)
#THUMB_SOURCE += $(UIP_SOURCE)

THUMB_OBJS = $(THUMB_SOURCE:.c=.o)
ARM_OBJS = $(ARM_SOURCE:.c=.o)

LIBS = -lc -lgcc -lm -Llib

###### GPS #######
LIBGPSI = -lm -Llidah -lnmea


all: gpso RTOSDemo.bin  sizeafter

RTOSDemo.bin : RTOSDemo.hex 
	$(OBJCOPY) $(NAMA_FILE).elf -O binary $(NAMA_FILE).bin
	 
RTOSDemo.hex : RTOSDemo.elf
	$(OBJCOPY) $(NAMA_FILE).elf -O ihex $(NAMA_FILE).hex
	$(DISAMB) $(NAMA_FILE).elf -d > $(NAMA_FILE).S

RTOSDemo.elf : $(THUMB_OBJS) $(ARM_OBJS) ../../system/boot.s Makefile 
	$(CC) $(CFLAGS) $(ARM_OBJS) $(THUMB_OBJS) $(LIBS) ../../system/boot.s $(LINKER_FLAGS)

$(THUMB_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) -mthumb $< -o $@
	
$(ARM_OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

clean : gpsclean
	rm $(THUMB_OBJS)
	rm $(ARM_OBJS)
	rm $(NAMA_FILE).elf
	rm $(NAMA_FILE).hex
	rm $(NAMA_FILE).bin
	rm $(NAMA_FILE).S
	rm $(NAMA_FILE).map
#	touch Makefile
	
tulis :
	$(PENULIS) -hex $(NAMA_FILE).hex /dev/ttyUSB0 115200 14748

tulis1 :
	$(PENULIS) -hex $(NAMA_FILE).hex /dev/ttyUSB1 115200 14748
		
MSG_SIZE_BEFORE = Size sebelum: 
MSG_SIZE_AFTER = Size setelah:

# Display size of file.
HEXSIZE = $(UKURAN) --target=$(FORMAT) $(NAMA_FILE).hex
ELFSIZE = $(UKURAN) -A $(NAMA_FILE).elf
sizebefore:
	@if [ -f $(NAMA_FILE).elf ]; then echo; echo $(MSG_SIZE_BEFORE); $(ELFSIZE); echo; fi

sizeafter:
	@if [ -f $(NAMA_FILE).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi

gpso: pregps $(GPSLIB)
	
pregps:
	mkdir -p build

$(GPSLIB): $(GPSOBJ)
	mkdir -p lidah
	ar rsc $@ $^
	ranlib $@
	
build/%.o: $(GPSDIR)/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

gpsclean:
	rm -f build/*.o lidah/*.a




