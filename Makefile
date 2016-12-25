# Author : Vaibhav Bansal

AR = xtensa-lx106-elf-ar
CC = xtensa-lx106-elf-gcc
NM = xtensa-lx106-elf-nm
CPP = xtensa-lx106-elf-g++
OBJCOPY = xtensa-lx106-elf-objcopy
OBJDUMP = xtensa-lx106-elf-objdump

CCFLAGS += 			\
	-Os			\
	-g			\
	-Wpointer-arith		\
	-fno-aggressive-loop-optimizations \
	-Wundef			\
	-Werror			\
	-Wl,-EL			\
	-fno-inline-functions	\
	-DMEMLEAK_DEBUG \
	-fdata-sections \
	-nostdlib       \
	-mlongcalls	\
	-mtext-section-literals \
	-ffunction-sections \
	-fno-builtin-printf

SDK_LIBDIR	= lib
SDK_LDDIR	= ld
SUBDIRS = MQTTClient
# Include
SDK_INCDIR	= extra_include include include/espressif include/freertos include/json include/udhcp include/lwip include/lwip/lwip include/lwip/ipv4 include/lwip/ipv6
LIB_INCDIR = lib/MQTTClient/include lib/MQTTPacket/include

SDK_INCDIR	:= $(addprefix -I $(SDK_PATH),$(SDK_INCDIR))
LIB_INCDIR := $(addprefix -I ,$(LIB_INCDIR))

SRC_DIR=lib/MQTTClient lib/MQTTPacket
BUILD_BASE = build
BUILD_DIR = lib/MQTTClient lib/MQTTPacket
#test.o: $(SRC)
#	$(CC) -o $@ -c $^ $(CCFLAGS) $(SDK_INCDIR) $(LIB_INCDIR) $(LIBS)

# define compile-objects
# $1/%.o:lib/MQTTClient/%.c
# 	$(CC) -o $@ -c $^ $(CCFLAGS) $(SDK_INCDIR) $(LIB_INCDIR) $(LIBS)
# endef
.PHONY: all
SRCS := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
objects := $(patsubst %.c,%.o,$(SRCS))

all: $(objects)

$(objects): %.o: %.c
	$(CC) -o $@ -c $^ $(CCFLAGS) $(SDK_INCDIR) $(LIB_INCDIR) $(LIBS)
	$(AR) ru libmqtt.a $@

clean:
	rm -f $(objects) *.a
