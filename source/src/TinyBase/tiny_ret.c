/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   ct_ret.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifdef TINY_DEBUG
#include "tiny_ret.h"

typedef struct _ValueDetail
{
    uint32_t value;
    const char *description;
} ValueDetail;

#define ENTRY(value, description)   {value, description}


static const ValueDetail code_details[] =
{
    ENTRY(0xFFFFFFFF, "invalid code"),
    ENTRY(CODE_OK, "OK"),
    ENTRY(CODE_PENDING, "Pending"),
    ENTRY(CODE_INTERNAL, "Internal"),
    ENTRY(CODE_OUT_OF_MEMORY, "Out of memory"),
    ENTRY(CODE_ARG_NULL, "Argument is null"),
    ENTRY(CODE_ARG_INVALID, "Argument is invalid"),
    ENTRY(CODE_NOT_IMPLEMENTED, "Not implemented"),
    ENTRY(CODE_NEW, "New"),
    ENTRY(CODE_CONSTRUCT, "Construct"),
    ENTRY(CODE_TIMEOUT, "Time is out"),
    ENTRY(CODE_NOT_FOUND, "Not Found"),
    ENTRY(CODE_STARTED, "Started"),
    ENTRY(CODE_STOPPED, "Stopped"),
    ENTRY(CODE_POSITION_INVALID, "Position is invalid"),
    ENTRY(CODE_SELECT, "Select"),
    ENTRY(CODE_SOCKET_FD, "Socket fd"),
    ENTRY(CODE_SOCKET_BIND, "Socket bind"),
    ENTRY(CODE_SOCKET_SETSOCKOPT, "Socket setsockopt"),
    ENTRY(CODE_SOCKET_LISTEN, "Socket listen"),
    ENTRY(CODE_SOCKET_READ, "Socket read"),
    ENTRY(CODE_SOCKET_WRITE, "Socket write"),
    ENTRY(CODE_SOCKET_CONNECTING, "Socket connecting"),
    ENTRY(CODE_SOCKET_CONNECTED, "Socket connected"),
    ENTRY(CODE_SOCKET_DISCONNECTED, "Socket disconnected"),
    ENTRY(CODE_SOCKET_ASCEPT, "Socket accept"),
    ENTRY(CODE_UUID_INVALID, "Uuid is invalid"),
    ENTRY(CODE_HTTP_MSG_INVALID, "Http message is invalid"),
    ENTRY(CODE_HTTP_TYPE_INVALID, "Http urn is invalid"),
    ENTRY(CODE_STATUS, "status is invalid"),
    ENTRY(CODE_ITEM_EXIST, "Item exist"),
    ENTRY(CODE_URL_INVALID, "Url invalid"),
    ENTRY(CODE_URL_GET_FAILED, "Url get failed"),
    ENTRY(CODE_XML_INVALID, "Xml invalid"),
    ENTRY(CODE_INITIALIZED, "initialized"),
    ENTRY(CODE_NOT_INITIALIZED, "not initialized"),
};

TINY_LOR
static const ValueDetail error_levels[] =
{
    ENTRY(EL_NULL, "NULL"),
    ENTRY(EL_GENERAL, "General error"),
    ENTRY(EL_SERIOUS, "Serious error"),
    ENTRY(EL_FATAL, "Fatal error"),
};

TINY_LOR
const char * tiny_ret_to_str(TinyRet ret)
{
    int32_t code = TINY_RET_CODE(ret);
    const char *description = NULL;
    int i = 0;

    for (i = 0; i < DIM(code_details); ++i)
    {
        if (code == code_details[i].value)
        {
            description = code_details[i].description;
            break;
        }
    }

    if (description == NULL)
    {
        printf("code: %d\n", code);
        description = code_details[0].description;
    }

    return description;
}
#endif