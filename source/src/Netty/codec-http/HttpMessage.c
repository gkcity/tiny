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
#include "HttpMessage.h"
#include "tiny_char_util.h"
#include "tiny_url_split.h"
#include "tiny_log.h"

#define TAG                 "HttpMessage"
#define CONTENT_LENGTH      "Content-Length"

/* HTTP/1.1 0 X */
#define HTTP_STATUS_LINE_MIN_LEN        14  /* strlen(HTTP/1.1 X 2\r\n) */
#define HTTP_REQUEST_LINE_MIN_LEN       14  /* X * HTTP/1.1\r\n */
#define HTTP_HEAD_LEN                   256
#define HTTP_LINE_LEN                   256

TINY_LOR
static uint32_t HttpMessage_LoadStatusLine(HttpMessage * thiz, const char *bytes, uint32_t len);

TINY_LOR
static uint32_t HttpMessage_LoadRequestLine(HttpMessage * thiz, const char *bytes, uint32_t len);

TINY_LOR
static void HttpMessage_ToBytesWithoutContent(HttpMessage *thiz);

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

	if (thiz->_bytes != NULL)
	{
		tiny_free(thiz->_bytes);
		thiz->_bytes = NULL;
		thiz->_size = 0;
	}

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

//TINY_LOR
//void HttpMessage_SetIp(HttpMessage *thiz, const char *ip)
//{
//    RETURN_IF_FAIL(thiz);
//    RETURN_IF_FAIL(ip);
//
//    strncpy(thiz->ip, ip, TINY_IP_LEN);
//}
//
//TINY_LOR
//const char * HttpMessage_GetIp(HttpMessage *thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//
//    return thiz->ip;
//}
//
//TINY_LOR
//void HttpMessage_SetPort(HttpMessage *thiz, uint16_t port)
//{
//    RETURN_IF_FAIL(thiz);
//
//    thiz->port = port;
//}
//
//TINY_LOR
//uint16_t HttpMessage_GetPort(HttpMessage *thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, 0);
//
//    return thiz->port;
//}

TINY_LOR
TinyRet HttpMessage_Parse(HttpMessage * thiz, const char *bytes, uint32_t length)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        uint32_t readable_length = length;
        uint32_t first_line_length = 0;
        uint32_t head_length = 0;
        const char * content_length = NULL;

        if (thiz->parser_status == HttpParserIncomplete)
        {
            //uint32_t left_length = thiz->content_length - thiz->content_length_loaded;
            uint32_t length_will_be_read = readable_length;
            uint32_t loaded_length = HttpContent_AddBytes(&thiz->content, bytes, length_will_be_read);
            thiz->consume_length = loaded_length;
            thiz->consume_length++;
            thiz->content_length_loaded += loaded_length;
            thiz->parser_status = (thiz->content_length_loaded < thiz->content_length) ? HttpParserIncomplete : HttpParserDone;
            break;
        }

        // load first line
        first_line_length = HttpMessage_LoadStatusLine(thiz, bytes, readable_length);
        if (first_line_length == 0)
        {
            first_line_length = HttpMessage_LoadRequestLine(thiz, bytes, readable_length);
            if (first_line_length == 0)
            {
                LOG_D(TAG, "HttpMessage_Parse => invalid first line");
                thiz->parser_status = HttpParserError;
                ret = TINY_RET_E_HTTP_MSG_INVALID;
                break;
            }
        }

        thiz->consume_length += first_line_length;
        readable_length = length - first_line_length;

        // load headers
        head_length = HttpHeader_Parse(&thiz->header, bytes + first_line_length, readable_length);
        if (head_length == 0)
        {
            LOG_D(TAG, "HttpMessage_Parse => invalid headers");
            thiz->parser_status = HttpParserHeaderIncomplete;
            break;
        }

        content_length = HttpHeader_GetValue(&thiz->header, CONTENT_LENGTH);
        thiz->content_length = (uint32_t)((content_length != NULL) ? atoi(content_length) : 0);
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

        readable_length = length - first_line_length - head_length;
        if (readable_length > 0)
        {
            const char *content_start = bytes + first_line_length + head_length;
            uint32_t length_will_be_read = (readable_length < thiz->content_length) ? readable_length : thiz->content_length;
            thiz->content_length_loaded = HttpContent_AddBytes(&thiz->content, content_start, length_will_be_read);
            thiz->consume_length += thiz->content_length_loaded;
            thiz->parser_status = (thiz->content_length_loaded >= thiz->content_length) ? HttpParserDone : HttpParserIncomplete;
        }
    } while (0);
    
    return ret;
}


TINY_LOR
const char * HttpMessage_GetBytesWithoutContent(HttpMessage *thiz)
{
	RETURN_VAL_IF_FAIL(thiz, NULL);

	if (thiz->_bytes == NULL)
	{
		HttpMessage_ToBytesWithoutContent(thiz);
	}

	return thiz->_bytes;
}

TINY_LOR
uint32_t HttpMessage_GetBytesSizeWithoutContent(HttpMessage *thiz)
{
	if (thiz->_bytes == NULL)
	{
		HttpMessage_ToBytesWithoutContent(thiz);
	}

	return thiz->_size;
}

TINY_LOR
static void HttpMessage_ToBytesWithoutContent(HttpMessage *thiz)
{
	do
	{
		uint32_t buffer_size = 0;
		char line[HTTP_LINE_LEN];

		if (thiz->_bytes != NULL)
		{
			tiny_free(thiz->_bytes);
			thiz->_bytes = NULL;
			break;
		}

		if (thiz->type == HTTP_UNDEFINED)
		{
            LOG_D(TAG, "HTTP TYPE invalid");
			break;
		}

		// calculate size
		buffer_size = HTTP_LINE_LEN + thiz->header.list.size * HTTP_HEAD_LEN;

		thiz->_bytes = (char *)tiny_malloc(buffer_size);
		if (thiz->_bytes == NULL)
		{
            LOG_E(TAG, "tiny_malloc failed: %d", buffer_size);
			break;
		}

		memset(thiz->_bytes, 0, buffer_size);

		// first line
		memset(line, 0, HTTP_LINE_LEN);
		if (thiz->type == HTTP_REQUEST)
		{
			tiny_snprintf(line,
				HTTP_LINE_LEN,
				"%s %s %s/%d.%d\r\n",
				thiz->request_line.method,
				thiz->request_line.uri,
				thiz->protocol_identifier,
				thiz->version.major,
				thiz->version.minor);
		}
		else
		{
			// RESPONSE
			tiny_snprintf(line,
				HTTP_LINE_LEN,
				"%s/%d.%d %d %s\r\n",
				thiz->protocol_identifier,
				thiz->version.major,
				thiz->version.minor,
				thiz->status_line.code,
				thiz->status_line.status);
		}
		line[HTTP_LINE_LEN - 1] = 0;

		strncat(thiz->_bytes, line, buffer_size);
		thiz->_size = (uint32_t)(strlen(thiz->_bytes));

		// headers
		for (uint32_t i = 0; i < thiz->header.list.size; ++i)
		{
			const char * name = HttpHeader_GetNameAt(&thiz->header, i);
			const char * value = HttpHeader_GetValueAt(&thiz->header, i);

			memset(line, 0, HTTP_LINE_LEN);
			tiny_snprintf(line, HTTP_LINE_LEN, "%s: %s\r\n", name, value);
			line[HTTP_LINE_LEN - 1] = 0;

			strncpy(thiz->_bytes + thiz->_size, line, HTTP_LINE_LEN);
			thiz->_size = (uint32_t)(strlen(thiz->_bytes));
		}

		// \r\n
		memset(line, 0, HTTP_LINE_LEN);
		tiny_snprintf(line, HTTP_LINE_LEN, "\r\n");
		line[HTTP_LINE_LEN - 1] = 0;

		strncat(thiz->_bytes, line, buffer_size);
		thiz->_size = (uint32_t)(strlen(thiz->_bytes));
	} while (0);
}

//TINY_LOR
//void HttpMessage_SetType(HttpMessage * thiz, HttpType type)
//{
//    RETURN_IF_FAIL(thiz);
//
//    thiz->type = type;
//}
//
//TINY_LOR
//HttpType HttpMessage_GetType(HttpMessage * thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, HTTP_UNDEFINED);
//
//    return thiz->type;
//}

TINY_LOR
void HttpMessage_SetMethod(HttpMessage *thiz, const char * method)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(method);

    strncpy(thiz->request_line.method, method, HTTP_METHOD_LEN);
}

TINY_LOR
void HttpMessage_SetUri(HttpMessage *thiz, const char * uri)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(uri);
    
    strncpy(thiz->request_line.uri, uri, HTTP_URI_LEN);
}

//TINY_LOR
//const char * HttpMessage_GetMethod(HttpMessage *thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//
//    return thiz->request_line.method;
//}
//
//TINY_LOR
//const char * HttpMessage_GetUri(HttpMessage *thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//
//    return thiz->request_line.uri;
//}

TINY_LOR
void HttpMessage_SetResponse(HttpMessage *thiz, int code, const char *status)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(status);

    thiz->type = HTTP_RESPONSE;
    thiz->status_line.code = code;
    strncpy(thiz->status_line.status, status, HTTP_STATUS_LEN);
}

//TINY_LOR
//const char * HttpMessage_GetStatus(HttpMessage *thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//
//    return thiz->status_line.status;
//}
//
//TINY_LOR
//int HttpMessage_GetStatusCode(HttpMessage *thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, 0);
//
//    return thiz->status_line.code;
//}

TINY_LOR
void HttpMessage_SetVersion(HttpMessage *thiz, int major, int minor)
{
    RETURN_IF_FAIL(thiz);

    thiz->version.major = major;
    thiz->version.minor = minor;
}

//TINY_LOR
//int HttpMessage_GetMajorVersion(HttpMessage *thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, 0);
//
//    return thiz->version.major;
//}
//
//TINY_LOR
//int HttpMessage_GetMinorVersion(HttpMessage *thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, 0);
//
//    return thiz->version.minor;
//}
//
//TINY_LOR
//HttpHeader * HttpMessage_GetHeader(HttpMessage * thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//
//    return &thiz->header;
//}
//
//TINY_LOR
//HttpContent * HttpMessage_GetContent(HttpMessage * thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//
//    return &thiz->content;
//}

//TINY_LOR
//void HttpMessage_SetHeader(HttpMessage * thiz, const char *name, const char *value)
//{
//    RETURN_IF_FAIL(thiz);
//    RETURN_IF_FAIL(name);
//    RETURN_IF_FAIL(value);
//
//    HttpHeader_Set(&thiz->header, name, value);
//}

//TINY_LOR
//void HttpMessage_SetHeaderInteger(HttpMessage * thiz, const char *name, uint32_t value)
//{
//    RETURN_IF_FAIL(thiz);
//    RETURN_IF_FAIL(name);
//
//    HttpHeader_SetInteger(&thiz->header, name, value);
//}

//TINY_LOR
//const char * HttpMessage_GetHeaderValue(HttpMessage * thiz, const char *name)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//    RETURN_VAL_IF_FAIL(name, NULL);
//
//    return HttpHeader_GetValue(&thiz->header, name);
//}
//
//TINY_LOR
//uint32_t HttpMessage_GetHeaderCount(HttpMessage * thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, 0);
//
//    return HttpHeader_GetCount(&thiz->header);
//}
//
//TINY_LOR
//const char * HttpMessage_GetHeaderNameAt(HttpMessage * thiz, uint32_t index)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//
//    return HttpHeader_GetNameAt(&thiz->header, index);
//}

//TINY_LOR
//const char * HttpMessage_GetHeaderValueAt(HttpMessage * thiz, uint32_t index)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//
//    return HttpHeader_GetValueAt(&thiz->header, index);
//}

//TINY_LOR
//const char * HttpMessage_GetContentObject(HttpMessage * thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, NULL);
//    return thiz->content.buf;
//}

//TINY_LOR
//uint32_t HttpMessage_GetContentSize(HttpMessage * thiz)
//{
//    RETURN_VAL_IF_FAIL(thiz, 0);
//
//    return HttpContent_GetSize(&thiz->content);
//}
//
//TINY_LOR
//bool HttpMessage_IsMethodEqual(HttpMessage * thiz, const char *method)
//{
//    RETURN_VAL_IF_FAIL(thiz, false);
//
//    return STR_EQUAL(thiz->request_line.method, method);
//}

TINY_LOR
bool HttpMessage_IsContentFull(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return (thiz->content.buf_size == thiz->content.data_size);
}

//TINY_LOR
//TinyRet HttpMessage_SetContentSize(HttpMessage *thiz, uint32_t size)
//{
//    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
//
//    return HttpContent_SetSize(&thiz->content, size);
//}

//TINY_LOR
//TinyRet HttpMessage_AddContentObject(HttpMessage *thiz, const char *bytes, uint32_t size)
//{
//    RETURN_VAL_IF_FAIL(thiz, 0);
//
//    return HttpContent_AddBytes(&thiz->content, bytes, size);
//}

TINY_LOR
static uint32_t HttpMessage_LoadStatusLine(HttpMessage * thiz, const char *bytes, uint32_t len)
{
	uint32_t i = 0;
    uint32_t count = 0;
    const char *p = bytes;

    if (len < HTTP_STATUS_LINE_MIN_LEN)
    {
        return 0;
    }

    // protocol identifier.
    count = (uint32_t)(strlen(thiz->protocol_identifier));
    for (i = 0; i < count; ++i)
    {
        if (p[i] != thiz->protocol_identifier[i])
        {
            return 0;
        }
    }

    p += count;

    // Slash.
    if (*p++ != '/')
    {
        return 0;
    }

    // Major version number.
    if (!is_digit(*p))
    {
        return 0;
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
        return 0;
    }

    // Minor version number.
    if (!is_digit(*p))
    {
        return 0;
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
        return 0;
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
        return 0;
    }

    // status
    memset(thiz->status_line.status, 0, HTTP_STATUS_LEN);
    while (!is_ctl(*p))
    {
        thiz->status_line.status[i++] = *p;
        p++;
    }

    // CRLF.
    if (*p++ != '\r')
    {
        return 0;
    }

    if (*p++ != '\n')
    {
        return 0;
    }

    thiz->type = HTTP_RESPONSE;

    // length of status line
    return (uint32_t)(p - bytes);
}

TINY_LOR
static uint32_t HttpMessage_LoadRequestLine(HttpMessage * thiz, const char *bytes, uint32_t len)
{
	uint32_t i = 0;
    uint32_t count = 0;
    const char *p = bytes;

    if (len < HTTP_STATUS_LINE_MIN_LEN)
    {
        return 0;
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
        return 0;
    }

    // Space.
    if (*p++ != ' ')
    {
        return 0;
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
        return 0;
    }

    // Space.
    if (*p++ != ' ')
    {
        return 0;
    }

    // protocol identifier.
    count = (uint32_t)strlen(thiz->protocol_identifier);
    for (i = 0; i < count; ++i)
    {
        if (p[i] != thiz->protocol_identifier[i])
        {
            return 0;
        }
    }
    p += count;

    // Slash.
    if (*p++ != '/')
    {
        return 0;
    }

    // Major version number.
    if (! is_digit(*p)) 
    {
        return 0;
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
        return 0;
    }

    // Minor version number.
    if (!is_digit(*p))
    {
        return 0;
    }

    thiz->version.minor = 0;
    while (is_digit(*p))
    {
        thiz->version.minor = thiz->version.minor * 10 + *p - '0';
        p++;
    }

    // CRLF.
    if (*p++ != '\r')
    {
        return 0;
    }

    if (*p++ != '\n')
    {
        return 0;
    }

    thiz->type = HTTP_REQUEST;

    // length of status line
    return (uint32_t)(p - bytes);
}

TINY_LOR
TinyRet HttpMessage_SetRequest(HttpMessage *thiz, const char * method, const char *url)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(method, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(url, TINY_RET_E_ARG_NULL);

    do
    {
        char ip[TINY_IP_LEN];
        uint16_t port = 0;
        char uri[512];
        char host[128];
        
        memset(ip, 0, TINY_IP_LEN);
        memset(uri, 0, 512);
        memset(host, 0, 128);

        ret = url_split(url, ip, TINY_IP_LEN, &port, uri, 512);
        if (RET_FAILED(ret))
        {
            LOG_W(TAG, "url_split: %s", tiny_ret_to_str(ret));
            break;
        }

        if (port == 80)
        {
            tiny_snprintf(host, 128, "%s", ip);
        }
        else
        {
            tiny_snprintf(host, 128, "%s:%d", ip, port);
        }

        strncpy(thiz->ip, ip, TINY_IP_LEN);
        thiz->port = port;
        thiz->type = HTTP_REQUEST;

        HttpMessage_SetVersion(thiz, 1, 1);
        HttpMessage_SetMethod(thiz, method);
        HttpMessage_SetUri(thiz, uri);
        HttpHeader_Set(&thiz->header, "HOST", host);
    } while (0);

    return ret;
}
