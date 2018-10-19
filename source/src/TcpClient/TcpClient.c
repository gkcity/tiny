/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpClient.c
 *
 * @remark
 *
 */
#include <tiny_log.h>
#include <tiny_malloc.h>
#include <channel/stream/StreamClientChannel.h>
#include <channel/SocketChannel.h>
#include <channel/ChannelIdleStateHandler.h>
#include "TcpClient.h"
#include "TcpClientHandler.h"

#define TAG     "TcpClient"


TINY_LOR
static TinyRet TcpClient_Construct(TcpClient *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TcpClient));

        thiz->client = StreamClientChannel_New();
        if (thiz->client == NULL)
        {
            LOG_E(TAG, "StreamClientChannel_New FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = Bootstrap_Construct(&thiz->bootstrap, NULL, NULL);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Bootstrap_Construct failed: %d", TINY_RET_CODE(ret));
            break;
        }

    } while (false);

    return ret;
}

TINY_LOR
static void TcpClient_Dispose(TcpClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    Bootstrap_Dispose(&thiz->bootstrap);
}

TINY_LOR
TcpClient * TcpClient_New(void)
{
    TcpClient *thiz = NULL;

    do
    {
        thiz = (TcpClient *)tiny_malloc(sizeof(TcpClient));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED");
            break;
        }

        if (RET_FAILED(TcpClient_Construct(thiz)))
        {
            TcpClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void TcpClient_Delete(TcpClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    TcpClient_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static void TcpClientInitializer(Channel *channel, void *ctx)
{
    TcpExchange * exchange = (TcpExchange *)ctx;
    LOG_D(TAG, "TcpClientInitializer: %s", channel->id);
    SocketChannel_AddLast(channel, ChannelIdleStateHandler(0, 0, exchange->timeout));
    SocketChannel_AddLast(channel, TcpClientHandler(exchange));
}

TINY_LOR
TinyRet TcpClient_Send(TcpClient *thiz, TcpExchange *exchange)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ret = StreamClientChannel_Initialize(thiz->client, TcpClientInitializer, exchange);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "StreamClientChannel_Initialize failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ret = StreamClientChannel_Connect(thiz->client, exchange->ip, exchange->port, exchange->timeout * 1000);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "StreamClientChannel_Connect failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ret = Bootstrap_AddChannel(&thiz->bootstrap, thiz->client);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Bootstrap_AddChannel failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ret = Bootstrap_Sync(&thiz->bootstrap);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Bootstrap_Sync failed: %d", TINY_RET_CODE(ret));
            break;
        }

        Bootstrap_Shutdown(&thiz->bootstrap);
    } while (false);

    return ret;
}