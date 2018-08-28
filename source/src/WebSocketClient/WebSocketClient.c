/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WsClient.c
 *
 * @remark
 *
 */
#include <tiny_log.h>
#include <tiny_malloc.h>
#include <channel/stream/StreamClientChannel.h>
#include <channel/SocketChannel.h>
#include <channel/ChannelIdleStateHandler.h>
#include <codec-http/HttpMessageCodec.h>
#include "WsClient.h"
#include "WsClientHandler.h"

#define TAG     "WsClient"


TINY_LOR
static TinyRet WsClient_Construct(WsClient *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(WsClient));

        thiz->client = StreamClientChannel_New();
        if (thiz->client == NULL)
        {
            LOG_E(TAG, "StreamClientChannel_New FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = Bootstrap_Construct(&thiz->bootstrap);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Bootstrap_Construct failed: %d", TINY_RET_CODE(ret));
            break;
        }

    } while (false);

    return ret;
}

TINY_LOR
static void WsClient_Dispose(WsClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    Bootstrap_Dispose(&thiz->bootstrap);
}

TINY_LOR
WsClient * WsClient_New(void)
{
    WsClient *thiz = NULL;

    do
    {
        thiz = (WsClient *)tiny_malloc(sizeof(WsClient));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED");
            break;
        }

        if (RET_FAILED(WsClient_Construct(thiz)))
        {
            WsClient_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void WsClient_Delete(WsClient *thiz)
{
    RETURN_IF_FAIL(thiz);

    WsClient_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static void WsClientInitializer(Channel *channel, void *ctx)
{
    HttpExchange * exchange = (HttpExchange *)ctx;
    LOG_D(TAG, "WsClientInitializer: %s", channel->id);
    SocketChannel_AddLast(channel, ChannelIdleStateHandler(0, 0, exchange->timeout));
    SocketChannel_AddLast(channel, HttpMessageCodec());
    SocketChannel_AddLast(channel, WsClientHandler(exchange));
}

TINY_LOR
TinyRet WsClient_Send(WsClient *thiz, HttpExchange *exchange)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ret = StreamClientChannel_Initialize(thiz->client, WsClientInitializer, exchange);
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

        ret = Bootstrap_Shutdown(&thiz->bootstrap);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Bootstrap_Shutdown failed: %d", TINY_RET_CODE(ret));
            break;
        }

    } while (false);

    return ret;
}