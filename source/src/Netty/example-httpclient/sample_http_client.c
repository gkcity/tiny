#include <tiny_log.h>
#include <bootstrap/Bootstrap.h>
#include <channel/stream/StreamClientChannel.h>
#include <channel/SocketChannel.h>
#include <codec-http/HttpMessageCodec.h>
#include "MyClientHandler.h"

#define TAG "HttpServerExample"

static void HttpClientInitializer(Channel *channel, void *ctx)
{
    printf("HttpClientInitializer: %s\n", channel->id);
    SocketChannel_AddLast(channel, MyClientHandler());
    SocketChannel_AddBefore(channel, MyClientHandler_Name, HttpMessageCodec());
}

int main()
{
    TinyRet ret = TINY_RET_OK;
    Channel *client = NULL;
    Bootstrap sb;

    tiny_socket_initialize();

    // new HTTP client
    client = StreamClientChannel_New();

    ret = StreamClientChannel_Initialize(client, HttpClientInitializer, NULL);
    if (RET_FAILED(ret))
    {
        printf("StreamClientChannel_Initialize failed: %d\n", TINY_RET_CODE(ret));
        return 0;
    }

    ret = StreamClientChannel_Connect(client, "127.0.0.1", 8080);
    if (RET_FAILED(ret))
    {
        // printf("StreamClientChannel_Connect failed: %d\n", TINY_RET_CODE(ret));
        printf("StreamClientChannel_Connect failed: %s\n", tiny_ret_to_str(ret));
        return 0;
    }

    // Starting Bootstrap
    ret = Bootstrap_Construct(&sb);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_Construct failed: %d\n", TINY_RET_CODE(ret));
        return 0;
    }

    ret = Bootstrap_AddChannel(&sb, client);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_AddChannel failed: %d\n", TINY_RET_CODE(ret));
        return 0;
    }

    ret = Bootstrap_Sync(&sb);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_Sync failed: %d\n", TINY_RET_CODE(ret));
        return 0;
    }

    ret = Bootstrap_Shutdown(&sb);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_Shutdown failed: %d\n", TINY_RET_CODE(ret));
        return 0;
    }

    Bootstrap_Dispose(&sb);

    tiny_socket_finalize();

    return 0;
}