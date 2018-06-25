/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_ret.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 *
 * @description
 *
 *  Values are 32 bit values layed out as follows:
 *  
 *   7 65 43210  76543210  76543210  76543210 |
 *  +-+--+------------------------------------+
 *  |S|EL|             Code                   |
 *  +-+--+------------------------------------+
 *
 *  S - Severity - indicates success/fail
 *      0 - Success
 *      1 - Fail
 *
 *  EL - Error level
 *      00 - General Error
 *      01 - Serious Error
 *      10 - Fatal Error
 *
 * 1. return value
 *      return TINY_RET_OK;
 *      return TINY_RET_E_SOCKET_FD;
 *      return TINY_RET_E_SOCKET_WRITE;
 *
 * 2. get return status
 *      TinyRet ret = tiny_tcp_async_connect(...);
 *      if (RET_SUCCEEDED(ret))
 *      {
 *          if (ret == TINY_RET_SOCKET_CONNECTED)
 *          {
 *              ...
 *          }
 *
 *          else if (ret == TINY_RET_SOCKET_CONNECTING)
 *          {
 *              ...
 *          }
 *      }
 *      else
 *      {
 *        ...
 *      }
 *
 *      if (RET_FAILED(ret))
 *      {
 *          if (ret == TINY_RET_SOCKET_CONNECT_FAILED)
 *          {
 *              ...
 *          }
 *      }
 *
 * 3. get return code
 *      uint32_t code = TINY_RET_GET_CODE(ret);
 *      LOG_D(TAG, "%s", tiny_ret_to_str(ret));
 */

#ifndef __TINY_RET_H__
#define __TINY_RET_H__

#include "tiny_typedef.h"
#include "tiny_api.h"
#include <tiny_lor.h>

TINY_BEGIN_DECLS


typedef uint32_t TinyRet;

#ifdef TINY_DEBUG
TINY_API
TINY_LOR
const char * tiny_ret_to_str(TinyRet ret);
#endif

#define RET_SUCCEEDED(r)                    (((TinyRet)(r)) >= 0)
#define RET_FAILED(r)                       (((TinyRet)(r)) < 0)

/* Severity Value */
#define SV_OK                               ((uint32_t)0)
#define SV_ERR                              ((uint32_t)1)

/* Error Level */
#define EL_NULL                             ((uint32_t)0)
#define EL_GENERAL                          ((uint32_t)1)
#define EL_SERIOUS                          ((uint32_t)2)
#define EL_FATAL                            ((uint32_t)3)

/* Code */
#define CODE_OK                             ((uint32_t)0)
#define CODE_PENDING                        ((uint32_t)1)
#define CODE_INTERNAL                       ((uint32_t)2)
#define CODE_OUT_OF_MEMORY                  ((uint32_t)3)
#define CODE_ARG_NULL                       ((uint32_t)4)
#define CODE_ARG_INVALID                    ((uint32_t)5)
#define CODE_NOT_IMPLEMENTED                ((uint32_t)6)
#define CODE_NEW                            ((uint32_t)7)
#define CODE_CONSTRUCT                      ((uint32_t)8)
#define CODE_TIMEOUT                        ((uint32_t)9)
#define CODE_NOT_FOUND                      ((uint32_t)10)
#define CODE_STARTED                        ((uint32_t)11)
#define CODE_STOPPED                        ((uint32_t)12)
#define CODE_POSITION_INVALID               ((uint32_t)13)
#define CODE_SELECT                         ((uint32_t)14)
#define CODE_SOCKET_FD                      ((uint32_t)15)
#define CODE_SOCKET_BIND                    ((uint32_t)16)
#define CODE_SOCKET_SETSOCKOPT              ((uint32_t)17)
#define CODE_SOCKET_LISTEN                  ((uint32_t)18)
#define CODE_SOCKET_READ                    ((uint32_t)19)
#define CODE_SOCKET_WRITE                   ((uint32_t)20)
#define CODE_SOCKET_CONNECTING              ((uint32_t)21)
#define CODE_SOCKET_CONNECTED               ((uint32_t)22)
#define CODE_SOCKET_DISCONNECTED            ((uint32_t)23)
#define CODE_SOCKET_ASCEPT                  ((uint32_t)24)
#define CODE_UUID_INVALID                   ((uint32_t)25)
#define CODE_HTTP_MSG_INVALID               ((uint32_t)26)
#define CODE_HTTP_TYPE_INVALID              ((uint32_t)27)
#define CODE_STATUS                         ((uint32_t)28)
#define CODE_ITEM_EXIST                     ((uint32_t)29)
#define CODE_URL_INVALID                    ((uint32_t)30)
#define CODE_URL_GET_FAILED                 ((uint32_t)31)
#define CODE_XML_INVALID                    ((uint32_t)32)
#define CODE_INITIALIZED                    ((uint32_t)41)
#define CODE_NOT_INITIALIZED                ((uint32_t)42)
#define CODE_FULL                           ((uint32_t)43)
#define CODE_EMPTY                          ((uint32_t)44)

/* Return the error level */
#define ERR_LEVEL(r)                        (((uint32_t)(r) & 0x7FFFFFFF) >> 29)

/* Return the code */
#define TINY_RET_CODE(r)                    ((uint32_t)((r) & 0x1FFFFFFF))

/* Create an TinyRet value from component pieces */
#define MAKE_RET(s, el, code)               ((uint32_t)(((uint32_t)(s) << 31) | ((uint32_t)(el) << 29) | ((uint32_t)(code))))

/* return value */
#define TINY_RET_OK                           MAKE_RET(SV_OK, EL_NULL, CODE_OK)
#define TINY_RET_PENDING                      MAKE_RET(SV_OK, EL_NULL, CODE_PENDING)
#define TINY_RET_TIMEOUT                      MAKE_RET(SV_OK, EL_NULL, CODE_TIMEOUT)
#define TINY_RET_E_INITIALIZED                MAKE_RET(SV_ERR, EL_GENERAL, CODE_INITIALIZED)
#define TINY_RET_E_NOT_INITIALIZED            MAKE_RET(SV_ERR, EL_GENERAL, CODE_NOT_INITIALIZED)
#define TINY_RET_E_INTERNAL                   MAKE_RET(SV_ERR, EL_GENERAL, CODE_INTERNAL)
#define TINY_RET_E_OUT_OF_MEMORY              MAKE_RET(SV_ERR, EL_GENERAL, CODE_OUT_OF_MEMORY)
#define TINY_RET_E_ARG_NULL                   MAKE_RET(SV_ERR, EL_GENERAL, CODE_ARG_NULL)
#define TINY_RET_E_ARG_INVALID                MAKE_RET(SV_ERR, EL_GENERAL, CODE_ARG_INVALID)
#define TINY_RET_E_NOT_IMPLEMENTED            MAKE_RET(SV_ERR, EL_GENERAL, CODE_NOT_IMPLEMENTED)
#define TINY_RET_E_NEW                        MAKE_RET(SV_ERR, EL_GENERAL, CODE_NEW)
#define TINY_RET_E_CONSTRUCT                  MAKE_RET(SV_ERR, EL_GENERAL, CODE_CONSTRUCT)
#define TINY_RET_E_TIMEOUT                    MAKE_RET(SV_ERR, EL_GENERAL, CODE_TIMEOUT)
#define TINY_RET_E_NOT_FOUND                  MAKE_RET(SV_ERR, EL_GENERAL, CODE_NOT_FOUND)
#define TINY_RET_E_STARTED                    MAKE_RET(SV_ERR, EL_GENERAL, CODE_STARTED)
#define TINY_RET_E_STOPPED                    MAKE_RET(SV_ERR, EL_GENERAL, CODE_STOPPED)
#define TINY_RET_E_POSITION_INVALID           MAKE_RET(SV_ERR, EL_GENERAL, CODE_POSITION_INVALID)
#define TINY_RET_E_SELECT                     MAKE_RET(SV_ERR, EL_GENERAL, CODE_SELECT)
#define TINY_RET_E_SOCKET_FD                  MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_FD)
#define TINY_RET_E_SOCKET_BIND                MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_BIND)
#define TINY_RET_E_SOCKET_SETSOCKOPT          MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_SETSOCKOPT)
#define TINY_RET_E_SOCKET_LISTEN              MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_LISTEN)
#define TINY_RET_E_SOCKET_READ                MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_READ)
#define TINY_RET_E_SOCKET_WRITE               MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_WRITE)
#define TINY_RET_E_SOCKET_CONNECTING          MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_CONNECTING)
#define TINY_RET_E_SOCKET_CONNECTED           MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_CONNECTED)
#define TINY_RET_E_SOCKET_DISCONNECTED        MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_DISCONNECTED)
#define TINY_RET_E_SOCKET_ASCEPT              MAKE_RET(SV_ERR, EL_GENERAL, CODE_SOCKET_ASCEPT)
#define TINY_RET_E_UUID_INVALID               MAKE_RET(SV_ERR, EL_GENERAL, CODE_UUID_INVALID)
#define TINY_RET_E_HTTP_MSG_INVALID           MAKE_RET(SV_ERR, EL_GENERAL, CODE_HTTP_MSG_INVALID)
#define TINY_RET_E_HTTP_TYPE_INVALID          MAKE_RET(SV_ERR, EL_GENERAL, CODE_HTTP_TYPE_INVALID)
#define TINY_RET_E_STATUS                     MAKE_RET(SV_ERR, EL_GENERAL, CODE_STATUS)
#define TINY_RET_E_ITEM_EXIST                 MAKE_RET(SV_ERR, EL_GENERAL, CODE_ITEM_EXIST)
#define TINY_RET_E_URL_INVALID                MAKE_RET(SV_ERR, EL_GENERAL, CODE_URL_INVALID)
#define TINY_RET_E_URL_GET_FAILED             MAKE_RET(SV_ERR, EL_GENERAL, CODE_URL_GET_FAILED)
#define TINY_RET_E_FULL                       MAKE_RET(SV_ERR, EL_GENERAL, CODE_FULL)


TINY_END_DECLS

#endif /* __TINY_RET_H__ */
