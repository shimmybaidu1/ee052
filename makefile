##############################################################################
# 
# Makefile
#
# EE/CS 52: ARM VoIP Phone Project
#
# Revision History
# 5/3/2009   Arthur Chang    Initial Revision
# 6/9/2009   Arthur Chang    Updated to dump to text file and ihex
#                            Added separate make for startup and UI
#                            Improved file organization
# 6/9/2009   Arthur Chang    Updated to work with new UI and lwip-1.3.0
# 2/16/2010  Glen George     Reorganized file and added some comments
# 2/13/2010  Glen George     Updated comments and added ability to link
#                               without the LwIP code
# 1/24/2012  Glen George     Updated formatting and reorganized file somewhat
#                               based on Joseph Schmitz's 1/30/2010 version
# 1/29/2012  Glen George     Made minor fixes
# 2/7/2012   Glen George     Made minor fixes to armstart target
#
##############################################################################


### Options ##################################################################

# remove the comment on the following line to exclude the LwIP code
# NO_LWIP = 1



### Variables ################################################################


# C compiler definitions

PROC = arm
TYPE = none-eabi

CC      = $(PROC)-$(TYPE)-gcc
NM      = $(PROC)-$(TYPE)-nm
OBJDUMP = $(PROC)-$(TYPE)-objdump
OBJCOPY = $(PROC)-$(TYPE)-objcopy


# directory definitions

SRCDIR  = src
OBJDIR  = obj
LWIPDIR = $(SRCDIR)/lwip-1.3.2/src
SYSDIR  = $(SRCDIR)/sys
VOIPDIR = $(SRCDIR)/voip101


# compiler flags (depend on whether compiling LwIP code or not)
#         -gdwarf-2             use DWARFv2 for debugging format
#         -Wall                 show most warnings
#         -march=armv4t         specify processor architecture
#         -fno-builtin          disregard any c library function names
#         -mcpu=arm920t         specify processor version
#         -O0                   do not optimize the C code
#         -mlittle-endian       use little endian memory
#         -g                    generate debugging output
#

BASICCFLAGS = -gdwarf-2 -Wall -march=armv4t -fno-builtin -mcpu=arm920t \
	      -mlittle-endian -O0 -g


# take care of whether LwIP code should be compiled or not
ifdef  NO_LWIP
CFLAGS = $(BASICCFLAGS) -DNO_LWIP
else
CFLAGS = $(BASICCFLAGS)
endif


# setup include directories (depends on whether including LWIP code or not)
#     -I<directoy>     search this directory for include files while compiling
#     -L<directory>    search this directory for files while linking

SYSINC = -I$(SYSDIR)

CSINC  = -I"c:\arm\codesourcery\arm-none-eabi\include" \
	 -L"c:\codesourcery\arm-none-eabi\lib"

# take care of whether LwIP code should be included or not
ifdef  NO_LWIP
LWIPINC = 
else
LWIPINC = -I$(LWIPDIR)/include \
          -I$(LWIPDIR)/include/arch -I$(LWIPDIR)/include/lwip \
          -I$(LWIPDIR)/include/ipv4 -I$(LWIPDIR)/include/netif
endif

# finally, put it all together
INCLUDES = $(SYSINC) $(CSINC) $(LWIPINC)



### LwIP files ###############################################################

# basic LwIP files

LWIPCOREFILES=$(LWIPDIR)/core/mem.c $(LWIPDIR)/core/memp.c \
	$(LWIPDIR)/core/netif.c $(LWIPDIR)/core/pbuf.c \
	$(LWIPDIR)/core/raw.c $(LWIPDIR)/core/stats.c \
	$(LWIPDIR)/core/sys.c $(LWIPDIR)/core/tcp.c \
	$(LWIPDIR)/core/tcp_in.c $(LWIPDIR)/core/tcp_out.c \
	$(LWIPDIR)/core/udp.c $(LWIPDIR)/core/dhcp.c


# IPv4 files for LwIP

LWIPCORE4FILES=$(LWIPDIR)/core/ipv4/icmp.c $(LWIPDIR)/core/ipv4/ip.c \
	$(LWIPDIR)/core/ipv4/inet.c $(LWIPDIR)/core/ipv4/ip_addr.c \
	$(LWIPDIR)/core/ipv4/ip_frag.c $(LWIPDIR)/core/ipv4/inet_chksum.c


# files implementing various generic network interface functions for LwIP

LWIPNETIFFILES=$(LWIPDIR)/netif/etharp.c


# definitions for all of the LwIP files

ifdef  NO_LWIP
# no LwIP files
LWIPFILES  =
LWIPFILESW =
LWIPOBJS   =
else
# want the LwIP files
LWIPFILES  = $(LWIPCOREFILES) $(LWIPCORE4FILES) $(LWIPNETIFFILES)
LWIPFILESW = $(wildcard $(LWIPFILES))
LWIPOBJS   = $(notdir $(LWIPFILES:.c=.o))
endif



### VoIP files ###############################################################

VOIPFILES = $(wildcard $(VOIPDIR)/*.c)
VOIPOBJS  = $(notdir $(VOIPFILES:.c=.o))



### low-level assembly files #################################################

# user must define the symbols SYSOBJS and BOOTOBJS below
# example definitions:
# SYSOBJS= boot.o keypad.o display.o
# BOOTOBJS= crt0.o

SYSOBJS  = crt0.o
BOOTOBJS = boot.o



### Rules ####################################################################

# specify virtual path for object files
vpath %.o $(OBJDIR)

# rule for building objects from C sources
%.o:
	echo $(VOIPFILESW)
	echo $(patsubst %.o,%.c,$(<))
	$(CC) $(CFLAGS) $(INCLUDES) -c $(patsubst %.o,%.c,$(<)) -o $(OBJDIR)/$@



### Command Line Targets #####################################################

# target for building all code

armvoip: .depend $(SYSOBJS) $(LWIPOBJS) $(VOIPOBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -v -Tldscript \
		-L"c:\arm\codesourcery\lib\gcc\arm-none-eabi\4.3.3" \
		-Wl,-Map=armvoip.map -lgcc -nostartfiles -o armvoip.elf \
		$(addprefix $(OBJDIR)/,$(SYSOBJS)) \
		$(addprefix $(OBJDIR)/,$(LWIPOBJS)) \
		$(addprefix $(OBJDIR)/,$(VOIPOBJS))
	$(OBJDUMP) -d armvoip.elf > armvoip.txt
	$(OBJCOPY) -O ihex armvoip.elf armvoip


# target for building boot code

armstart: $(BOOTOBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -Ttext 0x0 \
		-L"c:\arm\codesourcery\arm-none-eabi" \
		-lgcc -nostartfiles -o armstart.elf \
		$(addprefix $(OBJDIR)/,$(BOOTOBJS))
	$(OBJDUMP) -d armstart.elf > armstart.txt
	$(OBJCOPY) -O ihex armstart.elf armstart

	
# default target - builds everything

.PHONY: all
all: armvoip armstart


# target for cleaning house

.PHONY: clean
clean:
	rm .depend armvoip.txt armvoip.map armvoip.elf \
            armstart.txt armstart.map armstart.elf $(OBJDIR)\*.o



### General Targets ##########################################################

# target for generating dependency tree for the C code

.depend: $(LWIPFILES) $(VOIPDIR)/*.c
	$(CC) $(CFLAGS) $(INCLUDES) -MM $^ > .depend


# target for found dependencies (automatically generated above)

include .depend


# targets for low-level code

# user must supply these targets and dependencies
# example definition:
#  audio.o: $(SYSDIR)/audio.s $(SYSDIR)/at92rm9200.inc $(SYSDIR)/audio.inc
#        $(CC) $(CFLAGS) $(INCLUDES) -c -o $(OBJDIR)/audio.o $(SYSDIR)/audio.s


boot.s: $(SYSDIR)/at91rm9200.inc $(SYSDIR)/system.inc
crt0.s: $(SYSDIR)/at91rm9200.inc $(SYSDIR)/system.inc

boot.o: $(SYSDIR)/boot.s
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $(OBJDIR)/boot.o $(SYSDIR)/boot.s

crt0.o: $(SYSDIR)/crt0.s
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $(OBJDIR)/crt0.o $(SYSDIR)/crt0.s
