/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpMessageCodec.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <channel/SocketChannel.h>
#include <tiny_log.h>
#include <tiny_malloc.h>
#include "HttpMessageCodec.h"
#include "HttpMessage.h"

#define TAG     "HttpMessageCodec"

TINY_LOR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
	LOG_MEM(TAG, "_channelRead");

	do
	{
		if (type != DATA_RAW)
		{
			LOG_D(TAG, "_channelRead inType error: %d", type);
			break;
		}

		if (thiz->data == NULL)
		{
			thiz->data = HttpMessage_New();
			if (thiz->data == NULL)
			{
				LOG_D(TAG, "HttpMessage_New failed");
				break;
			}
		}

		if (RET_FAILED(HttpMessage_Parse((HttpMessage *)thiz->data, data, len)))
		{
			LOG_D(TAG, "HttpMessage_Parse failed!");
			HttpMessage_Delete((HttpMessage *)thiz->data);
			thiz->data = NULL;
			break;
		}

		if (((HttpMessage *)thiz->data)->parser_status != HttpParserDone)
		{
			LOG_D(TAG, "parser_status != HttpParserDone");
			HttpMessage_Delete((HttpMessage *)thiz->data);
			thiz->data = NULL;
			break;
		}

		if (HttpMessage_IsContentFull((HttpMessage *)thiz->data))
		{
            SocketChannel_NextRead(channel, DATA_HTTP_MESSAGE, thiz->data, len);
			HttpMessage_Delete((HttpMessage *)thiz->data);
			thiz->data = NULL;
		}
	} while (0);

	return true;
}

TINY_LOR
static TinyRet HttpMessageCodec_Dispose(ChannelHandler *thiz)
{
	RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

	if (thiz->data != NULL)
	{
		HttpMessage * p = (HttpMessage *)thiz->data;
		HttpMessage_Delete(p);
		thiz->data = NULL;
	}

	memset(thiz, 0, sizeof(ChannelHandler));

	return TINY_RET_OK;
}

TINY_LOR
static void HttpMessageCodec_Delete(ChannelHandler *thiz)
{
	HttpMessageCodec_Dispose(thiz);
	tiny_free(thiz);
}

TINY_LOR
static TinyRet HttpMessageCodec_Construct(ChannelHandler *thiz)
{
	memset(thiz, 0, sizeof(ChannelHandler));

	strncpy(thiz->name, HttpMessageCodec_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = HttpMessageCodec_Delete;
	thiz->inType = DATA_RAW;
	thiz->outType = DATA_HTTP_MESSAGE;
	thiz->channelRead = _channelRead;
	thiz->channelWrite = NULL;
	thiz->data = NULL;

	return TINY_RET_OK;
}

TINY_LOR
ChannelHandler * HttpMessageCodec(void)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(HttpMessageCodec_Construct(thiz)))
        {
            HttpMessageCodec_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}