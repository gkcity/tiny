/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   TinyMap.c
 *
 * @version 2013.8.6

 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "TinyMap.h"
#include "TinyMapItem.h"

/*-----------------------------------------------------------------------------
*
* Private API declare
*
*-----------------------------------------------------------------------------*/
#define TAG     "TinyMap"

TINY_LOR
static bool item_compare(void *data, void *ctx);

TINY_LOR
static int TinyMap_FindKey(TinyList * thiz, const char *key);

TINY_LOR
static void on_item_delete_listener(void * data, void *ctx);

/*-----------------------------------------------------------------------------
*
* Public API
*
*-----------------------------------------------------------------------------*/
TINY_LOR
TinyRet TinyMap_Construct(TinyMap *thiz, TinyContainerItemDeleteListener listener, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TinyMap));

        ret = TinyList_Construct(&thiz->list, on_item_delete_listener, thiz);
        if (RET_FAILED(ret))
        {
            break;
        }

        thiz->data_delete_listener = listener;
        thiz->data_delete_listener_ctx = ctx;
    } while (0);

    return ret;
}

TINY_LOR
TinyRet TinyMap_Dispose(TinyMap *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyList_Dispose(&thiz->list);

    return TINY_RET_OK;
}

TINY_LOR
TinyMap * TinyMap_New(TinyContainerItemDeleteListener listener, void *ctx)
{
    TinyMap *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinyMap *)tiny_malloc(sizeof(TinyMap));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinyMap_Construct(thiz, listener, ctx);
        if (RET_FAILED(ret))
        {
            TinyMap_Delete(thiz);
            thiz = NULL;
            break;
        }

    } while (0);

    return thiz;
}

TINY_LOR
void TinyMap_Delete(TinyMap *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMap_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
void * TinyMap_GetValue(TinyMap *thiz, const char *key)
{
    int index = 0;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    index = TinyList_Foreach(&thiz->list, item_compare, (void *)key);
    if (index >= 0)
    {
        TinyMapItem * item = (TinyMapItem *)TinyList_GetAt(&thiz->list, index);
        return item->value;
    }

    return NULL;
}

TINY_LOR
TinyRet TinyMap_Insert(TinyMap *thiz, const char *key, void *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        int index = 0;
        TinyMapItem * item = NULL;

        index = TinyMap_FindKey(&thiz->list, key);
        if (index >= 0)
        {
            ret = TINY_RET_E_ITEM_EXIST;
            break;
        }

        item = TinyMapItem_New(key, value);
        if (item == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->list, item);
        if (RET_FAILED(ret))
        {
            TinyMapItem_Delete(item);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet TinyMap_Erase(TinyMap *thiz, const char *key)
{
    int index = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    index = TinyMap_FindKey(&thiz->list, key);
    if (index < 0)
    {
        return TINY_RET_E_NOT_FOUND;
    }

    TinyList_RemoveAt(&thiz->list, index);
    return TINY_RET_OK;
}

/*-----------------------------------------------------------------------------
*
* Private API
*
*-----------------------------------------------------------------------------*/
TINY_LOR
static bool item_compare(void *data, void *ctx)
{
    TinyMapItem * item = (TinyMapItem *)data;
    const char * key = (const char *)ctx;

    return (strncmp(item->key, key, TINY_MAP_MAX_KEY_LEN) == 0);
}

TINY_LOR
static int TinyMap_FindKey(TinyList * thiz, const char *key)
{
    return TinyList_Foreach(thiz, item_compare, (void *)key);
}

TINY_LOR
static void on_item_delete_listener(void * data, void *ctx)
{
    TinyMap * thiz = (TinyMap *)ctx;
    TinyMapItem * item = (TinyMapItem *)data;

    if (thiz->data_delete_listener != NULL) 
    {
        thiz->data_delete_listener(item->value, thiz->data_delete_listener_ctx);
    }

    TinyMapItem_Delete(item);
}