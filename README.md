# MQTT library for ESP8266
#### for Espressif RTOS SDK v1.4.0 based on [Eclipse Paho](https://eclipse.org/paho/clients/c/embedded/)

This project aims to provide an independent MQTT library which can be seamlessly added to any existing ESP8266 project. It is inspired from

  - [esp_mqtt](https://github.com/tuanpmt/esp_mqtt) by tuanpmt
  - [ESP-RTOS-PAHO](https://github.com/baoshi/ESP-RTOS-Paho) by baoshi

Despite being excellent projects, it requires some effort to include MQTT in your own project from these.
### Prerequisites
1. **Xtensa lx106 architecture toolchain** to build the library. If you are using [Espressif lubuntu VM](https://espressif.com/en/support/explore/get-started/esp8266/getting-started-guide), this is already installed and added to your path. If you wish to set up your own, use [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk).
2. **ESP8266_RTOS_SDK v1.4.0** [Github](https://github.com/espressif/ESP8266_RTOS_SDK)
### Build
1. Make sure Xtensa toolchain is added to PATH
2. Export SDK_PATH with path to Espressif SDK installation.
```sh
$ export SDK_PATH=/path/to/espressif_rtos_sdk_dir
```
3. Build the library and install it in the SDK.
```sh
$ make && make install
```
### Usage
1. In your project Makefile add **-lmqtt** to LINKFLAGS_eagle.app.v6
```
LINKFLAGS_eagle.app.v6 = \
	-L$(SDK_PATH)/lib        \
  ...
  -lmqtt      \
	-lcirom     \
	-lcrypto	  \
	...
```
and include headers towards the end of Makefile
```
...
INCLUDES := $(INCLUDES) -I $(PDIR)include
INCLUDES += -I $(SDK_PATH)/include/mqtt
sinclude $(SDK_PATH)/Makefile
...
```
### Todos
 - SSL support
 - esp-open-rtos supprt
 - Add examples

 MIT License
