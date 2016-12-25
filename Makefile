# Author : Vaibhav Bansal

AR = xtensa-lx106-elf-ar
CC = xtensa-lx106-elf-gcc
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

SDK_INCDIR	:= $(addprefix -I $(SDK_PATH)/,$(SDK_INCDIR))
LIB_INCDIR := $(addprefix -I ,$(LIB_INCDIR))

SRC_DIR=lib/MQTTClient lib/MQTTPacket
SRCS := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJECTS := $(patsubst %.c,%.o,$(SRCS))

.PHONY: all

all: $(OBJECTS)

$(OBJECTS): %.o: %.c
	$(CC) -o $@ -c $^ $(CCFLAGS) $(SDK_INCDIR) $(LIB_INCDIR) $(LIBS)
	$(AR) ru libmqtt.a $@

install:
	cp libmqtt.a $(SDK_PATH)/lib
	cp -a lib/MQTTClient/include/. $(SDK_PATH)/include/mqtt
	cp -a lib/MQTTPacket/include/. $(SDK_PATH)/include/mqtt

clean:
	rm -f $(OBJECTS) *.a
