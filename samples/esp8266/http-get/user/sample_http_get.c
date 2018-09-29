/**
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <bootstrap/Bootstrap.h>
#include <channel/stream/StreamClientChannel.h>
#include <channel/stream/StreamClientChannelContext.h>
#include <channel/SocketChannel.h>
#include <channel/ChannelIdleStateHandler.h>
#include <codec-http/HttpMessageCodec.h>
#include <HttpClient.h>

#include "sample_http_get.h"

#define URI "/dd/instance/urn:xiot-spec:device:modbus-master:00000001:geekcity-hub:1"

void sample_http_get(void *pvParameters)
{
    HttpClient *client = HttpClient_New();

    HttpExchange *exchange = HttpExchange_New("10.0.1.9", 9000, "GET", URI, 30, NULL, 0);

    if (RET_SUCCEEDED(HttpClient_Send(client, exchange)))
    {
        printf("HTTP/1.1 %d\n\n", exchange->status);

        if (exchange->status == HTTP_STATUS_OK)
        {
            printf("%s\n", exchange->content);
        }
    }

    HttpExchange_Delete(exchange);
    HttpClient_Delete(client);
}
