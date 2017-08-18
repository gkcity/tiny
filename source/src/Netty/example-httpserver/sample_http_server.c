#include <tiny_log.h>
#include <bootstrap/Bootstrap.h>
#include <channel/stream/StreamServerChannel.h>
#include <channel/stream/StreamServerChannelContext.h>
#include <channel/SocketChannel.h>
#include <channel/ChannelIdleStateHandler.h>
#include <codec-http/HttpMessageCodec.h>
#include "ExampleHandler.h"

#define TAG "HttpServerExample"

static void HttpServerInitializer(Channel *channel, void *ctx)
{
    printf("HttpServerInitializer: %s\n", channel->id);
    SocketChannel_AddLast(channel, ChannelIdleStateHandler(0, 0, 3));
    SocketChannel_AddLast(channel, ExampleHandler());
    SocketChannel_AddBefore(channel, ExampleHandler_Name, HttpMessageCodec());
}

static TinyRet init_http_server(Channel *server, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    StreamServerChannel_Initialize(server, HttpServerInitializer, NULL);

    do
    {
        ret = SocketChannel_Open(server, TYPE_TCP_SERVER);
        if (RET_FAILED(ret))
        {
            printf("SocketChannel_Open failed: %d\n", TINY_RET_CODE(ret));
            break;
        }

        ret = SocketChannel_Bind(server, port, false);
        if (RET_FAILED(ret))
        {
            printf("SocketChannel_Bind failed: %d\n", TINY_RET_CODE(ret));
            break;
        }

        ret = SocketChannel_SetBlock(server, false);
        if (RET_FAILED(ret))
        {
            printf("SocketChannel_SetBlock failed: %d\n", TINY_RET_CODE(ret));
            break;
        }

        ret = SocketChannel_Listen(server, ((StreamServerChannelContext *)server->ctx)->maxConnections);
        if (RET_FAILED(ret))
        {
            printf("SocketChannel_Listen failed: %d\n", TINY_RET_CODE(ret));
            break;
        }
    } while (0);

    return ret;
}

int main()
{
    TinyRet ret = TINY_RET_OK;
    Channel *server1 = NULL;
//    Channel *server2 = NULL;
    Bootstrap sb;

    tiny_socket_initialize();

    // new TCP Server
    server1 = StreamServerChannel_New(6);
    ret = init_http_server(server1, 9091);
    if (RET_FAILED(ret))
    {
        printf("init_http_server 1 failed: %d\n", TINY_RET_CODE(ret));
        return 0;
    }

    // new TCP Server
//    server2 = StreamServerChannel_New(2);
//    ret = init_http_server(server1, 9092);
//    if (RET_FAILED(ret))
//    {
//        printf("init_http_server 2 failed: %s\n", tiny_ret_to_str(ret));
//        return 0;
//    }

    // Starting Bootstrap
    ret = Bootstrap_Construct(&sb);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_Construct failed: %d\n", TINY_RET_CODE(ret));
        return 0;
    }

    ret = Bootstrap_AddChannel(&sb, server1);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_AddChannel failed: %d\n", TINY_RET_CODE(ret));
        return 0;
    }

//    ret = Bootstrap_AddChannel(&sb, server2);
//    if (RET_FAILED(ret))
//    {
//        printf("Bootstrap_AddChannel failed: %s\n", tiny_ret_to_str(ret));
//        return 0;
//    }

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