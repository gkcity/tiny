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

#define TEST_URI "/dd/homekit/spec/service?type=urn:homekit-spec:service:fan:00000040"

void sample_http_get(void *pvParameters)
{
    HttpClient *client = HttpClient_New();

    HttpExchange *exchange = HttpExchange_New("39.106.171.204", 8080, "GET", TEST_URI, 10, NULL, 0);

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
