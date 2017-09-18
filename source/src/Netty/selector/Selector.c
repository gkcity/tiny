/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   Selector.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include <tiny_socket.h>
#include "Selector.h"
#include "tiny_log.h"

#define TAG     "Selector"

TINY_LOR
static TinyRet Selector_Select(Selector *thiz, int64_t us)
{
    TinyRet result = TINY_RET_OK;
    int ret = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_I(TAG, "Selector_Select, timeout: %ds %dms %dus", (int) (us / 1000000), (int) (us % 1000000) / 1000, (int) (us % 1000));

    if (us == 0)
    {
        ret = tiny_select(thiz->max_fd, &thiz->read_set, &thiz->write_set, NULL, NULL);
    }
    else
    {
        struct timeval tv;
        tv.tv_sec = (long) (us / 1000000);
        tv.tv_usec = (long) (us % 1000000);

        ret = tiny_select(thiz->max_fd, &thiz->read_set, &thiz->write_set, NULL, &tv);
    }

    LOG_D(TAG, "select: %d", ret);

    if (ret == 0)
    {
        LOG_D(TAG, "select timeout");
        result = TINY_RET_TIMEOUT;
    }
    else if (ret < 0)
    {
        LOG_D(TAG, "select failed");
        result = TINY_RET_E_INTERNAL;
    }
    else
    {
        result = TINY_RET_OK;
    }

    return result;
}

TINY_LOR
static TinyRet Selector_LoopOnce(Selector *thiz)
{
    TinyRet ret = TINY_RET_OK;

    LOG_I(TAG, "Selector_LoopOnce");

    /**
     * Reset
     */
    thiz->max_fd = 0;
    FD_ZERO(&thiz->read_set);
    FD_ZERO(&thiz->write_set);

    ret = thiz->onPreSelect(thiz, thiz->ctx);
    if (ret != TINY_RET_OK)
    {
        LOG_D(TAG, "onPreSelect failed");
        return ret;
    }

    ret = Selector_Select(thiz, thiz->us);
    switch (ret)
    {
        case TINY_RET_OK:
            ret = thiz->onPostSelect(thiz, thiz->ctx);
            break;

        case TINY_RET_TIMEOUT:
            ret = thiz->onSelectTimeout(thiz, thiz->ctx);
            break;

        default:
            break;
    }

    return ret;
}

TINY_LOR
TinyRet Selector_Construct(Selector *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(Selector));
    thiz->running = false;

    return TINY_RET_OK;
}

TINY_LOR
TinyRet Selector_Dispose(Selector *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

TINY_LOR
void Selector_Register(Selector *thiz, int fd, SelectorOperation op)
{
    RETURN_IF_FAIL(thiz);

    LOG_I(TAG, "Selector_Register: %d, Operation: 0x%x", fd, op);

    if ((op & SELECTOR_OP_READ) != 0)
    {
        FD_SET(fd, &thiz->read_set);
    }

    if ((op & SELECTOR_OP_WRITE) != 0)
    {
        FD_SET(fd, &thiz->write_set);
    }

    /**
    * NOTE
    *   max_fd is not used on Windows,
    *   but on linux/unix it MUST Greater than socket_fd.
    */
    if (thiz->max_fd <= fd)
    {
        thiz->max_fd = fd + 1;
    }
}

TINY_LOR
bool Selector_IsReadable(Selector *thiz, int fd)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return (FD_ISSET(fd, &thiz->read_set) != 0);
}

TINY_LOR
bool Selector_IsWriteable(Selector *thiz, int fd)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return (FD_ISSET(fd, &thiz->write_set) != 0);
}

TINY_LOR
TinyRet Selector_Loop(Selector *thiz)
{
    thiz->running = true;

    while (true)
    {
        TinyRet ret = Selector_LoopOnce(thiz);
        if (RET_FAILED(ret))
        {
            break;
        }
    }

    thiz->running = false;

    LOG_I(TAG, "Selector_Loop Finished!");

    return TINY_RET_OK;
}