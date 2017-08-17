/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ChannelIdleStateHandler.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "tiny_time.h"
#include "ChannelIdleStateHandler.h"

#define TAG     "ChannelIdleStateHandler"

static TinyRet ChannelIdleStateHandler_Construct(ChannelHandler *thiz, uint32_t readerIdle, uint32_t writerIdle);
static TinyRet ChannelIdleStateHandler_Dispose(ChannelHandler *thiz);
static void SocketChannelIdleStateHandler_Delete(ChannelHandler *thiz);
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, void *data, uint32_t len);
static bool _channelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);
static int64_t _channelNextTimeout(Channel *channel, void *ctx);

ChannelHandler * ChannelIdleStateHandler(uint32_t readerIdle, uint32_t writerIdle)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(ChannelIdleStateHandler_Construct(thiz, readerIdle, writerIdle)))
        {
            SocketChannelIdleStateHandler_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static void SocketChannelIdleStateHandler_Delete(ChannelHandler *thiz)
{
    ChannelIdleStateHandler_Dispose(thiz);
    tiny_free(thiz);
}

static uint64_t getCurTimestamp()
{
	struct timeval cur;
	tiny_gettimeofday(&cur, NULL);
	return cur.tv_sec * 1000000 + cur.tv_usec;
}

static uint64_t getTimeOut(ChannelHandler* thiz)
{
	uint64_t ret = 0;
	uint64_t tmp = 0;
	uint64_t cur = getCurTimestamp();

	thiz->idle.timeout = 0;
	if (thiz->idle.readerIdleTimeSeconds > 0)
	{
		thiz->idle.timeout = cur + thiz->idle.readerIdleTimeSeconds;
	}
	if (thiz->idle.writerIdleTimeSeconds > 0)
	{
		tmp = cur + thiz->idle.writerIdleTimeSeconds;
		if (tmp < thiz->idle.timeout)
		{
			thiz->idle.timeout = tmp;
		}
	}
/*	if (thiz->idle.allIdleTimeSeconds > 0)
	{
		thiz->idle.allTimeout = cur + thiz->idle.allIdleTimeSeconds;
	}
	else
	{
		thiz->idle.allTimeout = 0;
	}*/
	return ret;
}

static TinyRet ChannelIdleStateHandler_Construct(ChannelHandler *thiz, uint32_t readerIdle, uint32_t writerIdle)
{
    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, ChannelIdleStateHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = SocketChannelIdleStateHandler_Delete;
    thiz->inType = DATA_RAW;
    thiz->outType = DATA_RAW;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = _channelWrite;
    thiz->nextTimeout = _channelNextTimeout;
    thiz->idle.readerIdleTimeSeconds = readerIdle * 1000000;
    thiz->idle.writerIdleTimeSeconds = writerIdle * 1000000;
    getTimeOut(thiz);

    return TINY_RET_OK;
}

static TinyRet ChannelIdleStateHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, void *data, uint32_t len)
{
	uint64_t cur = getCurTimestamp();
	uint64_t tmp = 0;

    	LOG_D(TAG, "_channelRead: %d bytes from %s", len, channel->id);

	// TODO: 计算读超时时间
	if (thiz->idle.readerIdleTimeSeconds > 0)
	{
		thiz->idle.timeout = cur + thiz->idle.readerIdleTimeSeconds;
		//LOG_D(TAG, "next read timeout is [%llu]", thiz->idle.readTimeout);
	}
    	// 没有处理，返回false
    	return false;
}

static bool _channelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
	uint64_t cur = getCurTimestamp();
    	LOG_D(TAG, "_channelWrite: %d bytes to %s", len, channel->id);

   	 // 	TODO: 计算写超时时间
	if (thiz->idle.writerIdleTimeSeconds > 0)
	{
		thiz->idle.timeout = cur + thiz->idle.writerIdleTimeSeconds;
		//LOG_D(TAG, "next write timeout is [%llu]", thiz->idle.writeTimeout);
	}
    	// 没有处理，返回false
    return false;
}

static int64_t _channelNextTimeout(Channel *channel, void *ctx)
{
    ChannelHandler *thiz = (ChannelHandler *)ctx;
    uint64_t cur = getCurTimestamp();
    int64_t ret = 0;
    int64_t tmp = 0;

    ret = thiz->idle.timeout - cur;
    LOG_D(TAG, "current time[%llu], timeoutstamp[%llu], timeout[%llu]", cur, thiz->idle.timeout, ret);

    //LOG_D(TAG, "_channelNextTimeout: %d", thiz->idle.allIdleTimeSeconds);
    /*
    if (thiz->idle.readTimeout > 0)
    {
    	tmp = thiz->idle.readTimeout - cur;
    	LOG_D(TAG, "current time[%llu], read timeout[%llu]", cur, thiz->idle.readTimeout);
    	if (tmp < 0)
    	{
    		LOG_I(TAG, "current time[%llu] is later than the read timeoutstamp[%llu], maybe blocked, so check if there is data to read immediately.", cur, thiz->idle.readTimeout);
    		tmp = 100000;
    	}
    	ret = tmp;
    }
    if (thiz->idle.writeTimeout > 0)
    {
    	tmp = thiz->idle.writeTimeout - cur;
    	LOG_D(TAG, "current time[%llu], write timeout[%llu]", cur, thiz->idle.writeTimeout);
    	if (tmp < 0)
    	{
    		LOG_I(TAG, "current time[%llu] is later than the write timeoutstamp[%llu], maybe blocked, so check if there is data to write immediately.", cur, thiz->idle.writeTimeout);
    		tmp = 100000;
    	}
    	if (tmp < ret) ret = tmp;
    }*/

    return ret;
}