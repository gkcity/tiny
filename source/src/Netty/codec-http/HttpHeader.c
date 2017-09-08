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
#include "tiny_char_util.h"
#include "tiny_log.h"
#include <ctype.h>
#include <tiny_snprintf.h>
#include <tiny_malloc.h>

#define TAG     "HttpHeader"

#define HTTP_LINE_LEN            128
#define HTTP_MAX_NAME_LEN        64
#define HTTP_MAX_VALUE_LEN       64

typedef struct _Header
{
    char    name[HTTP_MAX_NAME_LEN];
    char    value[HTTP_MAX_VALUE_LEN];
} Header;

TINY_LOR
static void data_delete_listener(void * data, void *ctx)
{
    Header * header = (Header *)data;
    tiny_free(header);
}

TINY_LOR
TinyRet HttpHeader_Construct(HttpHeader *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(HttpHeader));
        
        ret = TinyList_Construct(&thiz->list);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyList_SetDeleteListener(&thiz->list, data_delete_listener, NULL);

    } while (0);

    return ret;
}

TINY_LOR
TinyRet HttpHeader_Dispose(HttpHeader *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyList_Dispose(&thiz->list);

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
//
//TINY_LOR
//void HttpHeader_Copy(HttpHeader *dst, HttpHeader *src)
//{
//    uint32_t i = 0;
//    uint32_t count = 0;
//
//    RETURN_IF_FAIL(dst);
//    RETURN_IF_FAIL(src);
//
//    count = (uint32_t)TinyList_GetCount(&src->list);
//    for (i = 0; i < count; i++)
//    {
//        Header * header = (Header *)TinyList_GetAt(&src->list, i);
//        HttpHeader_Set(dst, header->name, header->value);
//    }
//}

TINY_LOR
void HttpHeader_Set(HttpHeader * thiz, const char *name, const char *value)
{
    Header * header = NULL;

    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);
    RETURN_IF_FAIL(value);

    header = (Header *)tiny_malloc(sizeof(Header));
    if (header != NULL)
    {
		memset(header, 0, sizeof(Header));
        strncpy(header->name, name, HTTP_MAX_NAME_LEN);
        strncpy(header->value, value, HTTP_MAX_VALUE_LEN);
        TinyList_AddTail(&thiz->list, header);
    }
}

TINY_LOR
void HttpHeader_SetInteger(HttpHeader * thiz, const char *name, uint32_t value)
{
    char v[32];

    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);

    memset(v, 0, 32);
    tiny_snprintf(v, 32, "%u", value);
    HttpHeader_Set(thiz, name, v);
}

//TINY_LOR
//uint32_t HttpHeader_GetCount(HttpHeader * thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, 0);
//
//    return (uint32_t) thiz->list.size;
//}

TINY_LOR
const char * HttpHeader_GetValue(HttpHeader * thiz, const char *name)
{
    char s1[HTTP_MAX_NAME_LEN];
    char s2[HTTP_MAX_NAME_LEN];

    Header * header = NULL;
    uint32_t count = 0;
    uint32_t i = 0;
    uint32_t j = 0;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    memset(s1, 0, HTTP_MAX_NAME_LEN);
    memset(s2, 0, HTTP_MAX_NAME_LEN);

    strncpy(s1, name, HTTP_MAX_NAME_LEN);
    s1[HTTP_MAX_NAME_LEN - 1] = 0;
    for (i = 0; i < strlen(s1); ++i)
    {
        s1[i] = (char)(tolower(s1[i]));
    }

    count = (uint32_t) thiz->list.size;
    for (i = 0; i < count; ++i)
    {
        header = (Header *)TinyList_GetAt(&thiz->list, i);

        strncpy(s2, header->name, HTTP_MAX_NAME_LEN);
        s2[HTTP_MAX_NAME_LEN - 1] = 0;
        for (j = 0; j < strlen(s2); ++j)
        {
            s2[j] = (char)tolower(s2[j]);
        }

        if (strncmp(s1, s2, HTTP_MAX_NAME_LEN) == 0)
        {
            return header->value;
        }
    }

    return NULL;
}

TINY_LOR
const char * HttpHeader_GetNameAt(HttpHeader * thiz, uint32_t index)
{
    Header * header = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);

    header = (Header *)TinyList_GetAt(&thiz->list, index);
    if (header != NULL)
    {
        return header->name;
    }

    return NULL;
}

TINY_LOR
const char * HttpHeader_GetValueAt(HttpHeader * thiz, uint32_t index)
{
    Header * header = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);

    header = (Header *)TinyList_GetAt(&thiz->list, index);
    if (header != NULL)
    {
        return header->value;
    }

    return NULL;
}

TINY_LOR
uint32_t HttpHeader_Parse(HttpHeader *thiz, const char *bytes, uint32_t length)
{
    const char *p = bytes;

    TinyList_RemoveAll(&thiz->list);

    // Headers.
    while ((is_char(*p) && !is_ctl(*p) && !is_tspecial(*p) && *p != '\r')
        || (*p == ' ' || *p == '\t'))
    {
        char name[HTTP_MAX_NAME_LEN];
        char value[HTTP_MAX_VALUE_LEN];
        int i = 0;

        memset(name, 0, HTTP_MAX_NAME_LEN);
        memset(value, 0, HTTP_MAX_VALUE_LEN);

        if (*p == ' ' || *p == '\t')
        {
            // Leading whitespace. Must be continuation of previous header's value.
            return 0;
        }
        else
        {
            // Start the next header.

            // Header name.
            i = 0;
            while (is_char(*p) && !is_ctl(*p) && !is_tspecial(*p) && *p != ':' && length > 0)
            {
                name[i++] = *p;
                p++;
                length--;
            }

            // Colon and space separates the header name from the header value.
            if (*p++ != ':')
            {
                return 0;
            }

            length--;

            // skip SPACE if necessary
            if (*p == ' ')
            {
                p++;
                length--;
            }
        }

        // Header value.
        i = 0;
        while ((*p != '\r') && (length > 0))
        {
            value[i++] = *p;
            p++;

            length--;
        }

        HttpHeader_Set(thiz, name, value);

        // CRLF.
        if (*p++ != '\r')
        {
            return 0;
        }

        length--;

        if (*p++ != '\n')
        {
            return 0;
        }

        length--;
    }

    if (length < 2)
    {
        return (uint32_t)(p - bytes);
    }

    // CRLF.
    if (*p++ != '\r')
    {
        return 0;
    }

    length--;

    if (*p++ != '\n')
    {
        return 0;
    }

    length--;

    // length of heads
    return (uint32_t)(p - bytes);
}