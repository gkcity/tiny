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

static TinyRet _Selector_Select(Selector *thiz, int64_t ms);
static TinyRet _Selector_LoopOnce(Selector *thiz);

//Selector * Selector_New(void)
//{
//    Selector *thiz = NULL;
//
//    do
//    {
//        thiz = (Selector *)tiny_malloc(sizeof(Selector));
//        if (thiz == NULL)
//        {
//            break;
//        }
//
//        if (RET_FAILED(Selector_Construct(thiz)))
//        {
//            Selector_Delete(thiz);
//            thiz = NULL;
//            break;
//        }
//    } while (0);
//
//    return thiz;
//}

//void Selector_Delete(Selector *thiz)
//{
//    RETURN_IF_FAIL(thiz);
//
//    Selector_Dispose(thiz);
//    tiny_free(thiz);
//}

TinyRet Selector_Construct(Selector *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(Selector));

    return TINY_RET_OK;
}

TinyRet Selector_Dispose(Selector *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

void Selector_Register(Selector *thiz, int fd, SelectorOperation op)
{
    RETURN_IF_FAIL(thiz);

//    LOG_D(TAG, "Selector_Register: %d", fd);

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

static TinyRet _Selector_Select(Selector *thiz, int64_t us)
{
    TinyRet result = TINY_RET_OK;
    int ret = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "_Selector_Select, timeout: %ld us", us);

    if (us == 0)
    {
        ret = tiny_select(thiz->max_fd, &thiz->read_set, &thiz->write_set, NULL, NULL);
    }
    else
    {
        struct timeval tv;
        tv.tv_sec = us / 1000000;
        tv.tv_usec = us % 1000000;

        ret = tiny_select(thiz->max_fd, &thiz->read_set, &thiz->write_set, NULL, &tv);
    }

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

bool Selector_IsReadable(Selector *thiz, int fd)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return FD_ISSET(fd, &thiz->read_set);
}

bool Selector_IsWriteable(Selector *thiz, int fd)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return FD_ISSET(fd, &thiz->write_set);
}

static TinyRet _Selector_LoopOnce(Selector *thiz)
{
    TinyRet ret = TINY_RET_OK;

    /**
     * Reset
     */
    thiz->max_fd = 0;
    FD_ZERO(&thiz->read_set);
    FD_ZERO(&thiz->write_set);

    ret = thiz->onPreSelect(thiz, thiz->ctx);
    if (ret != TINY_RET_OK)
    {
        return ret;
    }

    ret = _Selector_Select(thiz, thiz->us);
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

TinyRet Selector_Loop(Selector *thiz)
{
    while (1)
    {
        TinyRet ret = _Selector_LoopOnce(thiz);
        if (RET_FAILED(ret))
        {
            break;
        }
    }

    return TINY_RET_OK;
}