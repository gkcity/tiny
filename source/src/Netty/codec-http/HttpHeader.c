/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpHeader.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "HttpHeader.h"
#include <tiny_log.h>
#include <ctype.h>
#include <tiny_snprintf.h>
#include <tiny_malloc.h>
#include <TinyMapItem.h>
#include <tiny_str_equal.h>

#define TAG     "HttpHeader"

#define HTTP_MAX_NAME_LEN               128
#define HTTP_MAX_VALUE_LEN              256

TINY_LOR
static void data_delete_listener(void * data, void *ctx)
{
    tiny_free(data);
}

TINY_LOR
TinyRet HttpHeader_Construct(HttpHeader *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(HttpHeader));
        
        ret = TinyMap_Construct(&thiz->values, data_delete_listener, NULL);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

TINY_LOR
TinyRet HttpHeader_Dispose(HttpHeader *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyMap_Dispose(&thiz->values);

    return TINY_RET_OK;
}

//TINY_LOR
//HttpHeader * HttpHeader_New(void)
//{
//    HttpHeader *thiz = NULL;
//
//    do
//    {
//        TinyRet ret = TINY_RET_OK;
//
//        thiz = (HttpHeader *)tiny_malloc(sizeof(HttpHeader));
//        if (thiz == NULL)
//        {
//            break;
//        }
//
//        ret = HttpHeader_Construct(thiz);
//        if (RET_FAILED(ret))
//        {
//            HttpHeader_Delete(thiz);
//            thiz = NULL;
//            break;
//        }
//    } while (0);
//
//    return thiz;
//}
//
//TINY_LOR
//void HttpHeader_Delete(HttpHeader *thiz)
//{
//    RETURN_IF_FAIL(thiz);
//
//    HttpHeader_Dispose(thiz);
//    tiny_free(thiz);
//}


TINY_LOR
TinyRet HttpHeader_Set(HttpHeader * thiz, const char *name, const char *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do 
    {
//        char key[HTTP_MAX_NAME_LEN];
//        char *v = NULL;
//        uint32_t vLength = 0;
//
//        memset(key, 0, HTTP_MAX_NAME_LEN);
//
//        for (uint32_t i = 0; i < strlen(name); ++i)
//        {
//            key[i] = (char)tolower((int)(name[i]));
//        }
//
//        vLength = (uint32_t) strlen(value);
//        v = tiny_malloc(vLength + 1);
//        if (v == NULL)
//        {
//            ret = TINY_RET_E_NEW;
//            break;
//        }
//
//        memset(v, 0, vLength + 1);
//        strncpy(v, value, vLength);
//
//        TinyMap_Erase(&thiz->values, key);
//
//        ret = TinyMap_Insert(&thiz->values, key, (void *)v);
//        if (RET_FAILED(ret))
//        {
//            tiny_free(v);
//        }

        char *v = NULL;
        uint32_t vLength = 0;

        for (uint32_t i = 0; i < thiz->values.list.size; ++i)
        {
            TinyMapItem *item = TinyList_GetAt(&thiz->values.list, i);
            if (str_equal(name, item->key, true))
            {
                TinyMap_Erase(&thiz->values, item->key);
                break;
            }
        }

        vLength = (uint32_t) strlen(value);
        v = tiny_malloc(vLength + 1);
        if (v == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
        memset(v, 0, vLength + 1);
        strncpy(v, value, vLength);

        ret = TinyMap_Insert(&thiz->values, name, (void *)v);
        if (RET_FAILED(ret))
        {
            tiny_free(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet HttpHeader_SetInteger(HttpHeader * thiz, const char *name, uint32_t value)
{
    char v[32];

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    memset(v, 0, 32);
    tiny_snprintf(v, 32, "%u", value);

    return HttpHeader_Set(thiz, name, v);
}

TINY_LOR
TinyRet HttpHeader_SetHost(HttpHeader * thiz, const char *ip, uint16_t port)
{
    char v[64];

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ip, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(port, TINY_RET_E_ARG_NULL);

    memset(v, 0, 64);
    tiny_snprintf(v, 64, "%s:%d", ip, port);

    return HttpHeader_Set(thiz, "HOST", v);
}

TINY_LOR
const char * HttpHeader_GetValue(HttpHeader * thiz, const char *name)
{
//    char key[HTTP_MAX_NAME_LEN];

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

//    memset(key, 0, HTTP_MAX_NAME_LEN);
//
//    for (uint32_t i = 0; i < strlen(name); ++i)
//    {
//        key[i] = (char) tolower((int)(name[i]));
//    }
//
//    return (const char *) TinyMap_GetValue(&thiz->values, key);

    for (uint32_t i = 0; i < thiz->values.list.size; ++i)
    {
        TinyMapItem *item = TinyList_GetAt(&thiz->values.list, i);
        if (str_equal(name, item->key, true))
        {
            return item->value;
        }
    }

    return NULL;
}

TINY_LOR
uint32_t HttpHeader_GetSize(HttpHeader * thiz)
{
    uint32_t size = 0;

    for (uint32_t i = 0; i < thiz->values.list.size; ++i)
    {
        TinyMapItem *item = (TinyMapItem *) TinyList_GetAt(&thiz->values.list, i);

        // <key> + ': ' + <value> + '\r\n'
        size += (uint32_t) (strlen(item->key) + 2 + strlen((const char *)item->value) + 2);
    }

    size += 2;

    return size;
}

TINY_LOR
TinyRet HttpHeader_GetContentLength(HttpHeader * thiz, uint32_t *length)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        const char *value = HttpHeader_GetValue(thiz, CONTENT_LENGTH);
        char *stop = NULL;

        if (value == NULL)
        {
            (*length) = 0;
            break;
        }

        *length = (uint32_t)(strtol(value, &stop, 10));

        if (stop == value)
        {
            ret = TINY_RET_E_ARG_INVALID;
        }
    } while (false);

    return ret;
}

TINY_LOR
static bool HttpHeader_ParseLine(HttpHeader *thiz, Line *line)
{
    bool ret = false;
    uint32_t separator = line->offset;

    while (separator < line->length)
    {
        if (line->value[separator] == ':')
        {
            break;
        }

        separator ++;
    }

    do
    {
        char key[HTTP_MAX_NAME_LEN];
        char value[HTTP_MAX_VALUE_LEN];
        uint32_t keyLength = separator - line->offset;
        uint32_t valueLength = line->length - separator - 1;

        if (separator == line->length)
        {
            break;
        }

        if (keyLength > HTTP_MAX_NAME_LEN || keyLength == 0)
        {
            break;
        }

        if (valueLength > HTTP_MAX_VALUE_LEN || valueLength == 0)
        {
            break;
        }

        memset(key, 0, HTTP_MAX_NAME_LEN);
        memcpy(key, line->value + line->offset, keyLength);

        // skip ' '
        separator ++;

        while (separator < line->length)
        {
            if (line->value[separator] != ' ')
            {
                break;
            }

            separator ++;
            valueLength --;
        }

        memset(value, 0, HTTP_MAX_VALUE_LEN);
        memcpy(value, line->value + separator, valueLength);

        if (RET_FAILED(HttpHeader_Set(thiz, key, value)))
        {
            LOG_E(TAG, "HttpHeader_Set failed!");
            break;
        }

        ret = true;
    } while (false);

    return ret;
}

TINY_LOR
TinyRet HttpHeader_Parse(HttpHeader *thiz, Bytes *bytes)
{
    TinyRet ret = TINY_RET_OK;

    while (true)
    {
        Line line;

        line.value = NULL;
        line.length = 0;
        line.offset = 0;

        if (! Bytes_GetLine(bytes, &line))
        {
            LOG_E(TAG, "Bytes_GetLine Failed");
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        if (line.length == 0)
        {
            break;
        }

        if (! HttpHeader_ParseLine(thiz, &line))
        {
            LOG_E(TAG, "HttpHeader_ParseLine Failed");
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }
    }

    return ret;
}
