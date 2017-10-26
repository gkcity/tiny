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

void tiny_print_mem_info(const char *tag, const char *function)
{
    printf("[%s/%s] stack = %d, free heap size: %d\n", tag, function,
      uxTaskGetStackHighWaterMark(NULL), system_get_free_heap_size());
}

#define URI "/homekit/instance/device?type=urn:homtkit-spec:device:lightbulb:00000000:arrizo-v1"

/**
 * NO ACTION
 */
#define ZHIMI_FAN "/instance/device?type=urn:miot-spec:device:fan:00000A04:zhimi"

/**
 * Contains Action without arguments
 */
#define ROCK_SWEEPER "/instance/device?type=urn:miot-spec:device:sweeper:00000A0A:rock"

/**
 * Contains Action with arguments
 */
#define CHUNMI_COOKER "/instance/device?type=urn:miot-spec:device:cooker:00000A08:chunmi"

void sample_http_get(void *pvParameters)
{
    HttpClient *client = HttpClient_New();

    //HttpExchange *exchange = HttpExchange_New("47.93.60.147", 8080, "GET", CHUNMI_COOKER, 5, NULL, 0);
    HttpExchange *exchange = HttpExchange_New("127.0.0.1", 9000, "GET", URI, 5, NULL, 0);

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