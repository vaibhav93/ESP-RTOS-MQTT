/* Author : Vaibhav Bansal
   Description : ESP8266 MQTT Library for  RTOS SDK 1.4.0
                 Based on eclipse PAHO
   License : BSD
*/
#include "MQTTESP8266.h"

/*Initialize Timer*/
void TimerInit(Timer* timer)
{
	timer->xTicksToWait = 0;
	memset(&timer->xTimeOut, '\0', sizeof(timer->xTimeOut));
}

int TimerLeftMS(Timer* timer)
{
	xTaskCheckForTimeOut(&timer->xTimeOut, &timer->xTicksToWait); /* updates xTicksToWait to the number left */
	return (timer->xTicksToWait < 0) ? 0 : (timer->xTicksToWait * portTICK_RATE_MS);
}

char TimerIsExpired(Timer *timer)
{
  return xTaskCheckForTimeOut(&timer->xTimeOut, &timer->xTicksToWait) == pdTRUE;
}

void TimerCountdownMS(Timer* timer, unsigned int timeout_ms)
{
	timer->xTicksToWait = timeout_ms / portTICK_RATE_MS; /* convert milliseconds to ticks */
	vTaskSetTimeOutState(&timer->xTimeOut); /* Record the time at which this function was entered. */
}

void TimerCountdown(Timer* timer, unsigned int timeout)
{
	TimerCountdownMS(timer, timeout * 1000);
}

int ESP8266_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  int recvLen = 0;
  struct timeval tv;
  tv.tv_sec = timeout_ms/1000;
  tv.tv_usec = 0;
  fd_set readset;

  FD_ZERO(&readset);
  FD_SET(n->my_socket,&readset);

  if (lwip_select(n->my_socket + 1, &readset, NULL, NULL, &tv) > 0 && FD_ISSET(n->my_socket, &readset)) {
			recvLen = lwip_read(n->my_socket, buffer, len);
	} else {
    return -1;
  }

	return recvLen;
}

int ESP8266_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
  int sendLen = 0;
  struct timeval tv;
  tv.tv_sec = timeout_ms/1000;
  tv.tv_usec = 0;
  fd_set writeset;

  FD_ZERO(&writeset);
  FD_SET(n->my_socket,&writeset);

  if (lwip_select(n->my_socket + 1, NULL, &writeset, NULL, &tv) > 0 && FD_ISSET(n->my_socket, &writeset)) {
      sendLen = lwip_write(n->my_socket, buffer, len);
  } else {
    return -1;
  }

  return sendLen;
}

void ESP8266_disconnect(Network* n)
{
  n->my_socket = 0;
	lwip_close(n->my_socket);
}

void NetworkInit(Network* n)
{
	n->my_socket = 0;
	n->mqttread = ESP8266_read;
	n->mqttwrite = ESP8266_write;
	n->disconnect = ESP8266_disconnect;
}

LOCAL int host2addr(const char *hostname , struct in_addr *in)
{
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    rv = getaddrinfo(hostname, 0 , &hints , &servinfo);
    if (rv != 0)
    {
        return rv;
    }

    // loop through all the results and get the first resolve
    for (p = servinfo; p != 0; p = p->ai_next)
    {
        h = (struct sockaddr_in *)p->ai_addr;
        in->s_addr = h->sin_addr.s_addr;
    }
    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}

int ConnectNetwork(Network* n, char* host, int port)
{
    struct sockaddr_in addr;
    int ret;

    if (host2addr(host, &(addr.sin_addr)) != 0)
    {
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    n->my_socket = lwip_socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( n->my_socket < 0 )
    {
        return -1;
    }
    ret = lwip_connect(n->my_socket, ( struct sockaddr *)&addr, sizeof(struct sockaddr_in));
    if( ret < 0 )
    {
        lwip_close(n->my_socket);
        return ret;
    }

    return ret;
}
