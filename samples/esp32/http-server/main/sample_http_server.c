#include <bootstrap/Bootstrap.h>
#include <channel/stream/StreamServerChannel.h>
#include <channel/stream/StreamServerChannelContext.h>
#include <channel/SocketChannel.h>
#include <channel/ChannelIdleStateHandler.h>
#include <codec-http/HttpMessageCodec.h>
#include "ExampleHandler.h"

static void HttpServerInitializer(Channel *channel, void *ctx)
{
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

        ret = SocketChannel_Listen(server, ((StreamServerChannelContext *)server->context)->maxConnections);
        if (RET_FAILED(ret))
        {
            printf("SocketChannel_Listen failed: %d\n", TINY_RET_CODE(ret));
            break;
        }
    } while (0);

    return ret;
}

void sample_http_server_run(void *pvParameters)
{
    TinyRet ret = TINY_RET_OK;
    Channel *server1 = NULL;
    Bootstrap sb;

    printf("Start HTTP Server\n");

    // new TCP Server
    server1 = StreamServerChannel_New(6);
    StreamServerChannel_Initialize(server1, HttpServerInitializer, NULL);

    ret = init_http_server(server1, 9090);
    if (RET_FAILED(ret))
    {
        printf("init_http_server failed: %d\n", TINY_RET_CODE(ret));
        return;
    }

    printf("Bind Port: %d\n", server1->local.socket.port);

    // Starting Bootstrap
    ret = Bootstrap_Construct(&sb);
    if (RET_FAILED(ret))
    {
        printf("Bootstrap_Construct failed: %d\n", TINY_RET_CODE(ret));
        return;
    }

    printf("Bootstrap AddChannel: HTTP Server\n");

    ret = Bootstrap_AddChannel(&sb, server1);
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
