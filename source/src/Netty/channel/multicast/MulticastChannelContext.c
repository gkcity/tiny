/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   MulticastChannelContext.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include "channel/multicast/MulticastChannelContext.h"

static TinyRet MulticastChannelContext_Construct(MulticastChannelContext *thiz)
{
    memset(thiz, 0, sizeof(MulticastChannelContext));
    return TINY_RET_OK;
}

static void MulticastChannelContext_Dispose(MulticastChannelContext *thiz)
{
}

MulticastChannelContext * MulticastChannelContext_New(void)
{
    MulticastChannelContext *thiz = NULL;

    do
    {
        thiz = (MulticastChannelContext *)tiny_malloc(sizeof(MulticastChannelContext));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(MulticastChannelContext_Construct(thiz)))
        {
            MulticastChannelContext_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void MulticastChannelContext_Delete(MulticastChannelContext *thiz)
{
    RETURN_IF_FAIL(thiz);

    MulticastChannelContext_Dispose(thiz);
    tiny_free(thiz);
}