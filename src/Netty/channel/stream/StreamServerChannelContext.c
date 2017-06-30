/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   StreamServerChannelContext.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include "StreamServerChannelContext.h"

static void _OnChannelRemoved(void * data, void *ctx)
{
    Channel *channel = (Channel *)data;
    channel->onRemove(channel);
}

static TinyRet StreamServerChannelContext_Construct(StreamServerChannelContext *thiz)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ret = TinyList_Construct(&thiz->channels);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyList_SetDeleteListener(&thiz->channels, _OnChannelRemoved, NULL);
    } while (0);

    return ret;
}

static void StreamServerChannelContext_Dispose(StreamServerChannelContext *thiz)
{
    TinyList_Dispose(&thiz->channels);
}

StreamServerChannelContext * StreamServerChannelContext_New(void)
{
    StreamServerChannelContext *thiz = NULL;

    do
    {
        thiz = (StreamServerChannelContext *)tiny_malloc(sizeof(StreamServerChannelContext));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(StreamServerChannelContext_Construct(thiz)))
        {
            StreamServerChannelContext_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void StreamServerChannelContext_Delete(StreamServerChannelContext *thiz)
{
    RETURN_IF_FAIL(thiz);

    StreamServerChannelContext_Dispose(thiz);
    tiny_free(thiz);
}