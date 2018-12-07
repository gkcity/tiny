/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   UnicastChannelContext.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include "channel/unicast/UnicastChannelContext.h"

TINY_LOR
static TinyRet UnicastChannelContext_Construct(UnicastChannelContext *thiz)
{
    memset(thiz, 0, sizeof(UnicastChannelContext));
    return TINY_RET_OK;
}

TINY_LOR
static void UnicastChannelContext_Dispose(UnicastChannelContext *thiz)
{
}

TINY_LOR
UnicastChannelContext * UnicastChannelContext_New(void)
{
    UnicastChannelContext *thiz = NULL;

    do
    {
        thiz = (UnicastChannelContext *)tiny_malloc(sizeof(UnicastChannelContext));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UnicastChannelContext_Construct(thiz)))
        {
            UnicastChannelContext_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void UnicastChannelContext_Delete(UnicastChannelContext *thiz)
{
    RETURN_IF_FAIL(thiz);

    UnicastChannelContext_Dispose(thiz);
    tiny_free(thiz);
}