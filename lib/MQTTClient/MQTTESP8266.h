#ifndef _MQTT_ESP8266_H_
#define _MQTT_ESP8266_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sys.h"

typedef struct Timer Timer;

struct Timer
{
	portTickType xTicksToWait;
	xTimeOutType xTimeOut;
};

typedef struct Network Network;

struct Network
{
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, int);
	int (*mqttwrite) (Network*, unsigned char*, int, int);
	void (*disconnect) (Network*);
};

void TimerInit(Timer*);
int TimerLeftMS(Timer*);
char TimerIsExpired(Timer*);
void TimerCountdownMS(Timer*, unsigned int);
void TimerCountdown(Timer*, unsigned int);
int ESP8266_read(Network*, unsigned char*, int, int);
int ESP8266_write(Network*, unsigned char*, int, int);
void ESP8266_disconnect(Network*);
void NetworkInit(Network*);
int ConnectNetwork(Network*, char*, int);

#endif
