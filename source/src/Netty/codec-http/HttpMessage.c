/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpMessage.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include <tiny_snprintf.h>
#include <tiny_malloc.h>
#include <TinyMapItem.h>
#include "HttpMessage.h"
#include "tiny_char_util.h"
#include "tiny_log.h"

#define TAG                 "HttpMessage"

/* HTTP/1.1 0 X */
#define HTTP_STATUS_LINE_MIN_LEN        14  /* strlen(HTTP/1.1 X 2\r\n) */
#define HTTP_REQUEST_LINE_MIN_LEN       14  /* X * HTTP/1.1\r\n */

TINY_LOR
static bool HttpMessage_ParseLine(HttpMessage * thiz, Line *line);

TINY_LOR
HttpMessage * HttpMessage_New(void)
{
    HttpMessage *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (HttpMessage *)tiny_malloc(sizeof(HttpMessage));
        if (thiz == NULL)
        {
            break;
        }

        ret = HttpMessage_Construct(thiz);
        if (RET_FAILED(ret))
        {
            HttpMessage_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TINY_LOR
HttpMessage * HttpMessage_NewHttpGet(const char *uri)
{
    HttpMessage *thiz = NULL;

    do
    {
        thiz = HttpMessage_New();
        if (thiz == NULL)
        {
            break;
        }

        HttpMessage_SetMethod(thiz, "GET");
        HttpMessage_SetVersion(thiz, 1, 1);
        HttpMessage_SetUri(thiz, uri);
    } while (false);

    return thiz;
}

TINY_LOR
HttpMessage * HttpMessage_NewHttpResponse(int code, const char *status, const char *type, const uint8_t *content, uint32_t length)
{
    HttpMessage *thiz = NULL;

    do
    {
        thiz = HttpMessage_New();
        if (thiz == NULL)
        {
            break;
        }

        HttpMessage_SetResponse(thiz, code, status);
        HttpMessage_SetVersion(thiz, 1, 1);

        if (type != NULL && content != NULL && length > 0)
        {
            HttpMessage_SetContent(thiz, type, length, content);
        }
    } while (false);

    return thiz;
}

TINY_LOR
TinyRet HttpMessage_Construct(HttpMessage *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(HttpMessage));

        ret = HttpHeader_Construct(&thiz->header);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpContent_Construct(&thiz->content);
        if (RET_FAILED(ret))
        {
            break;
        }

        strncpy(thiz->protocol_identifier, PROTOCOL_HTTP, PROTOCOL_LEN);
    }
    while (0);

    return ret;
}

TINY_LOR
TinyRet HttpMessage_Dispose(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    HttpContent_Dispose(&thiz->content);
    HttpHeader_Dispose(&thiz->header);

    return TINY_RET_OK;
}

TINY_LOR
void HttpMessage_Delete(HttpMessage *thiz)
{
    RETURN_IF_FAIL(thiz);

    HttpMessage_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
void HttpMessage_SetProtocolIdentifier(HttpMessage * thiz, const char *identifier)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(identifier);

    strncpy(thiz->protocol_identifier, identifier, PROTOCOL_LEN);
}

TINY_LOR
TinyRet HttpMessage_Parse(HttpMessage * thiz, const char *data, uint32_t length)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        Bytes bytes;
        Line line;

        bytes.value = data;
        bytes.length = length;
        bytes.offset = 0;

        line.value = NULL;
        line.length = 0;
        line.offset = 0;

        if (thiz->parser_status == HttpParserIncomplete)
        {
            uint32_t loaded_length = HttpContent_LoadBytes(&thiz->content, &bytes);
            thiz->consume_length = loaded_length;
            thiz->consume_length++;
            thiz->content_length_loaded += loaded_length;
            thiz->parser_status = (thiz->content_length_loaded < thiz->content_length) ? HttpParserIncomplete : HttpParserDone;
            break;
        }

        if (! Bytes_GetLine(&bytes, &line))
        {
            LOG_D(TAG, "Bytes_GetLine => invalid first line");
            thiz->parser_status = HttpParserError;
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        if (! HttpMessage_ParseLine(thiz, &line))
        {
            LOG_D(TAG, "HttpMessage_ParseLine => invalid first line");
            thiz->parser_status = HttpParserError;
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        if (RET_FAILED(HttpHeader_Parse(&thiz->header, &bytes)))
        {
            LOG_D(TAG, "HttpMessage_Parse => invalid headers");
            thiz->parser_status = HttpParserHeaderIncomplete;
            break;
        }

        if (RET_FAILED(HttpHeader_GetContentLength(&thiz->header, &thiz->content_length)))
        {
            LOG_D(TAG, "HttpHeader_GetContentLength failed!");
            thiz->parser_status = HttpParserHeaderIncomplete;
            break;
        }

        if (thiz->content_length == 0)
        {
            thiz->parser_status = HttpParserDone;
            break;
        }

        ret = HttpContent_SetSize(&thiz->content, thiz->content_length);
        if (RET_FAILED(ret))
        {
            thiz->parser_status = HttpParserError;
            break;
        }

        thiz->parser_status = HttpParserIncomplete;

        if ((bytes.length - bytes.offset) > 0)
        {
            thiz->content_length_loaded = HttpContent_LoadBytes(&thiz->content, &bytes);
            thiz->consume_length += thiz->content_length_loaded;
            thiz->parser_status = (thiz->content_length_loaded >= thiz->content_length) ? HttpParserDone : HttpParserIncomplete;
        }
    } while (false);

    return ret;
}

TINY_LOR
void HttpMessage_SetMethod(HttpMessage *thiz, const char * method)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(method);

    thiz->type = HTTP_REQUEST;
    strncpy(thiz->request_line.method, method, HTTP_METHOD_LEN);
}

TINY_LOR
void HttpMessage_SetUri(HttpMessage *thiz, const char * uri)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(uri);
    
    strncpy(thiz->request_line.uri, uri, HTTP_URI_LEN);
}

TINY_LOR
void HttpMessage_SetResponse(HttpMessage *thiz, int code, const char *status)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(status);

    thiz->type = HTTP_RESPONSE;
    thiz->status_line.code = code;
    strncpy(thiz->status_line.status, status, HTTP_STATUS_LEN);
}

TINY_LOR
void HttpMessage_SetVersion(HttpMessage *thiz, int major, int minor)
{
    RETURN_IF_FAIL(thiz);

    thiz->version.major = major;
    thiz->version.minor = minor;
}

TINY_LOR
TinyRet HttpMessage_SetContent(HttpMessage *thiz, const char *type, uint32_t length, const uint8_t *content)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ret = HttpHeader_Set(&thiz->header, "Content-Type", type);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpHeader_SetInteger(&thiz->header, "Content-Length", length);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpContent_SetSize(&thiz->content, length);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpContent_PutBytes(&thiz->content, length, content);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (false);

    return ret;
}

TINY_LOR
bool HttpMessage_IsContentFull(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return (thiz->content.buf_size == thiz->content.data_size);
}

TINY_LOR
static TinyRet HttpMessage_ParseStatusLine(HttpMessage * thiz, Line *line)
{
    uint32_t i = 0;
    uint32_t count = 0;
    const char *p = line->value + line->offset;

    if ((line->length - line->offset) < HTTP_STATUS_LINE_MIN_LEN)
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // protocol identifier.
    count = (uint32_t)(strlen(thiz->protocol_identifier));
    for (i = 0; i < count; ++i)
    {
        if (p[i] != thiz->protocol_identifier[i])
        {
            return TINY_RET_E_ARG_INVALID;
        }
    }

    p += count;

    // Slash.
    if (*p++ != '/')
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // Major version number.
    if (!is_digit(*p))
    {
        return TINY_RET_E_ARG_INVALID;
    }

    thiz->version.major = 0;
    while (is_digit(*p))
    {
        thiz->version.major = thiz->version.major * 10 + *p - '0';
        p++;
    }

    // Dot.
    if (*p++ != '.')
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // Minor version number.
    if (!is_digit(*p))
    {
        return TINY_RET_E_ARG_INVALID;
    }

    thiz->version.minor = 0;
    while (is_digit(*p))
    {
        thiz->version.minor = thiz->version.minor * 10 + *p - '0';
        p++;
    }

    // Space.
    if (*p++ != ' ')
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // status code
    thiz->status_line.code = 0;
    while (is_digit(*p))
    {
        thiz->status_line.code = thiz->status_line.code * 10 + *p - '0';
        p++;
    }

    // Space.
    if (*p++ != ' ')
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // status
    memset(thiz->status_line.status, 0, HTTP_STATUS_LEN);
    while (!is_ctl(*p))
    {
        thiz->status_line.status[i++] = *p;
        p++;
    }

    thiz->type = HTTP_RESPONSE;

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet HttpMessage_ParseRequestLine(HttpMessage * thiz, Line *line)
{
    uint32_t i = 0;
    uint32_t count = 0;
    const char *p = line->value + line->offset;

    if ((line->length - line->offset) < HTTP_REQUEST_LINE_MIN_LEN)
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // Request method.
    memset(thiz->request_line.method, 0, HTTP_METHOD_LEN);
    i = 0;
    while (is_char(*p) && !is_ctl(*p) && !is_tspecial(*p) && *p != ' ')
    {
        thiz->request_line.method[i++] = *p;
        p++;
    }

    if (strlen(thiz->request_line.method) == 0)
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // Space.
    if (*p++ != ' ')
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // URI.
    memset(thiz->request_line.uri, 0, HTTP_URI_LEN);
    i = 0;
    while (!is_ctl(*p) && *p != ' ')
    {
        thiz->request_line.uri[i++] = *p;
        p++;
    }

    if (strlen(thiz->request_line.uri) == 0)
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // Space.
    if (*p++ != ' ')
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // protocol identifier.
    count = (uint32_t)strlen(thiz->protocol_identifier);
    for (i = 0; i < count; ++i)
    {
        if (p[i] != thiz->protocol_identifier[i])
        {
            return TINY_RET_E_ARG_INVALID;
        }
    }
    p += count;

    // Slash.
    if (*p++ != '/')
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // Major version number.
    if (! is_digit(*p))
    {
        return TINY_RET_E_ARG_INVALID;
    }

    thiz->version.major = 0;
    while (is_digit(*p))
    {
        thiz->version.major = thiz->version.major * 10 + *p - '0';
        p++;
    }

    // Dot.
    if (*p++ != '.')
    {
        return TINY_RET_E_ARG_INVALID;
    }

    // Minor version number.
    if (!is_digit(*p))
    {
        return TINY_RET_E_ARG_INVALID;
    }

    thiz->version.minor = 0;
    while (is_digit(*p))
    {
        thiz->version.minor = thiz->version.minor * 10 + *p - '0';
        p++;
    }

    thiz->type = HTTP_REQUEST;

    return TINY_RET_OK;
}

TINY_LOR
static bool HttpMessage_ParseLine(HttpMessage * thiz, Line *line)
{
    bool ret = true;

    do
    {
        if (RET_SUCCEEDED(HttpMessage_ParseStatusLine(thiz, line)))
        {
            break;
        }

        if (RET_SUCCEEDED(HttpMessage_ParseRequestLine(thiz, line)))
        {
            break;
        }

        ret = false;
    } while (false);

    return ret;
}

TINY_LOR
TinyRet HttpMessage_SetRequest(HttpMessage *thiz, const char * method, const char *uri)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(method, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(uri, TINY_RET_E_ARG_NULL);

    thiz->type = HTTP_REQUEST;
    HttpMessage_SetVersion(thiz, 1, 1);
    HttpMessage_SetMethod(thiz, method);
    HttpMessage_SetUri(thiz, uri);

    return TINY_RET_OK;
}