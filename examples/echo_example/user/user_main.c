/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "esp_common.h"
#include "espressif/espconn.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "uart.h"
#include "MQTTClient.h"

#define DEMO_AP_SSID "VB_NET"
#define DEMO_AP_PASSWORD "Password@123"
/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}
void messageArrived(MessageData* data)
{
  int i =0;
	printf("Message arrived on topic:%s \n",data->message->payload);
  printf("Size of data %d\n",(int)data->message->payloadlen );
  for(i=0; i<(int)data->message->payloadlen;i++)
    printf("%c",((char *)data->message->payload)[i]);
}

void prvMQTTEchoTask(void *pvParameters)
{
	/* connect to m2m.eclipse.org, subscribe to a topic, send and receive messages regularly every 1 sec */
	MQTTClient client;
	Network network;
	unsigned char sendbuf[80], readbuf[80];
	int rc = 0,
		count = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;

	pvParameters = 0;
	NetworkInit(&network);
	MQTTClientInit(&client, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

	char* address = "iot.eclipse.org";
	if ((rc = ConnectNetwork(&network, address, 1883)) != 0)
		printf("Return code from network connect is %d\n", rc);

#if defined(MQTT_TASK)
	if ((rc = MQTTStartTask(&client)) != pdPASS)
		printf("Return code from start tasks is %d\n", rc);
#endif

	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = "FreeRTOS_sample";

	if ((rc = MQTTConnect(&client, &connectData)) != 0)
		printf("Return code from MQTT connect is %d\n", rc);
	else
		printf("MQTT Connected\n");

	if ((rc = MQTTSubscribe(&client, "FreeRTOS/sample/#", 2, messageArrived)) != 0)
		printf("Return code from MQTT subscribe is %d\n", rc);

	while (++count)
	{
		MQTTMessage message;
		char payload[30];

		message.qos = 1;
		message.retained = 0;
		message.payload = payload;
		sprintf(payload, "message number %d", count);
		message.payloadlen = strlen(payload);

		if ((rc = MQTTPublish(&client, "FreeRTOS/sample/a", &message)) != 0)
			printf("Return code from MQTT publish is %d\n", rc);
#if !defined(MQTT_TASK)
		if ((rc = MQTTYield(&client, 1000)) != 0)
			printf("Return code from yield is %d\n", rc);
#endif
	}

	/* do not return */
}

void wifi_task(void *pvParameters)
{
    struct station_config sta_config;
    struct ip_info ip_config;
    bzero(&sta_config, sizeof(struct station_config));

  	sprintf(sta_config.ssid, "VB_NET");
  	sprintf(sta_config.password, "Doodle@123");
    wifi_station_set_config(&sta_config);
  	os_printf("%s\n", __func__);
  	wifi_get_ip_info(STATION_IF, &ip_config);
  	while(ip_config.ip.addr == 0){
  		vTaskDelay(1000 / portTICK_RATE_MS);
  		wifi_get_ip_info(STATION_IF, &ip_config);
      printf("Connecting...\n");
  	}
    printf("Connected\n");
    xTaskCreate(prvMQTTEchoTask,	/* The function that implements the task. */
			"MQTTEcho0",			/* Just a text name for the task to aid debugging. */
			512,	/* The stack size is defined in FreeRTOSIPConfig.h. */
			NULL,		/* The task parameter, not used in this case. */
			3,		/* The priority assigned to the task is defined in FreeRTOSConfig.h. */
			NULL);				/* The task handle is not used. */
    vTaskDelete(NULL);
}
/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    user_rf_cal_sector_set();
    // Initialize UART for debugging
    uart_init_new();
    UART_SetBaudrate(0,115200);
    wifi_set_opmode(STATION_MODE);
    uint8_t wifiStatus = STATION_IDLE;
    printf("SDK version:%s\n\r", system_get_sdk_version());
    //wifi_station_scan(NULL,scan_done);
    xTaskCreate(wifi_task, "wifi_task", 256, NULL, 2, NULL);
}
