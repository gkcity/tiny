/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   StreamClientChannelContext.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include "StreamClientChannelContext.h"

TINY_LOR
static TinyRet StreamClientChannelContext_Construct(StreamClientChannelContext *thiz)
{
    TinyRet ret = TINY_RET_OK;

    return ret;
}

TINY_LOR
static void StreamClientChannelContext_Dispose(StreamClientChannelContext *thiz)
{
}

TINY_LOR
StreamClientChannelContext * StreamClientChannelContext_New(void)
{
    StreamClientChannelContext *thiz = NULL;

    do
    {
        thiz = (StreamClientChannelContext *)tiny_malloc(sizeof(StreamClientChannelContext));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(StreamClientChannelContext_Construct(thiz)))
        {
            StreamClientChannelContext_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void StreamClientChannelContext_Delete(StreamClientChannelContext *thiz)
{
    RETURN_IF_FAIL(thiz);

    StreamClientChannelContext_Dispose(thiz);
    tiny_free(thiz);
}