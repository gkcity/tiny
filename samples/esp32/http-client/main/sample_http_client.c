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
#include "MyClientHandler.h"

void tiny_print_mem_info(const char *tag, const char *function)
{
    printf("[%s/%s] stack = %d, free heap size: %d\n", tag, function,
      uxTaskGetStackHighWaterMark(NULL), system_get_free_heap_size());
}

static void HttpClientInitializer(Channel *channel, void *ctx)
{
    printf("HttpServerInitializer: %s\n", channel->id);
    SocketChannel_AddLast(channel, ChannelIdleStateHandler(3, 4, 5));
    SocketChannel_AddLast(channel, HttpMessageCodec());
    SocketChannel_AddLast(channel, MyClientHandler());
}

void sample_http_client(void *pvParameters)
{
    TinyRet ret = TINY_RET_OK;
    Channel *client = NULL;
    Bootstrap sb;

    printf("Start HTTP Client\n");

    // new TCP Server
    client = StreamClientChannel_New();

    ret = StreamClientChannel_Initialize(client, HttpClientInitializer, NULL);
    if (RET_FAILED(ret))
    {
        printf("StreamClientChannel_Initialize failed: %d\n", TINY_RET_CODE(ret));
        return;
    }

    ret = StreamClientChannel_Connect(client, "10.0.1.9", 8080, 3000);
    if (RET_FAILED(ret))
    {
        printf("StreamClientChannel_Connect failed: %d\n", TINY_RET_CODE(ret));
        return;
    }

    // Starting Bootstrap
    ret = Bootstrap_Construct(&sb);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_Construct failed: %d\n", TINY_RET_CODE(ret));
        return;
    }

    ret = Bootstrap_AddChannel(&sb, client);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_AddChannel failed: %d\n", TINY_RET_CODE(ret));
        return;
    }

    printf("Bootstrap running\n");

    ret = Bootstrap_Sync(&sb);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_Sync failed: %d\n", TINY_RET_CODE(ret));
        return;
    }

    printf("Bootstrap shutdown\n");

    Bootstrap_Shutdown(&sb);
    Bootstrap_Dispose(&sb);
}
