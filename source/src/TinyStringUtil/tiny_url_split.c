/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   tiny_url_split.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_url_split.h"
#include "tiny_char_util.h"

TINY_LOR
TinyRet url_split(const char *url, char ip[], uint32_t ip_len, uint16_t *port, char uri[], uint32_t uri_len)
{
    /* http://10.0.1.1:8080/1.mp4 */
    TinyRet ret = TINY_RET_OK;
    const char *p = url;
    uint32_t i = 0;

    do
    {
        if (strlen(url) < strlen("http://x"))
        {
            ret = TINY_RET_E_URL_INVALID;
            break;
        }

        if (*p++ != 'h')
        {
            ret = TINY_RET_E_URL_INVALID;
            break;
        }

        if (*p++ != 't')
        {
            ret = TINY_RET_E_URL_INVALID;
            break;
        }

        if (*p++ != 't')
        {
            ret = TINY_RET_E_URL_INVALID;
            break;
        }

        if (*p++ != 'p')
        {
            ret = TINY_RET_E_URL_INVALID;
            break;
        }

        if (*p++ != ':')
        {
            ret = TINY_RET_E_URL_INVALID;
            break;
        }

        if (*p++ != '/')
        {
            ret = TINY_RET_E_URL_INVALID;
            break;
        }

        if (*p++ != '/')
        {
            ret = TINY_RET_E_URL_INVALID;
            break;
        }

        /* ip */
        i = 0;
        while (1)
        {
            if (*p == ':' || *p == '/')
            {
                break;
            }

            ip[i++] = *p++;

            if (i >= ip_len)
            {
                ret = TINY_RET_E_URL_INVALID;
                break;
            }
        }

        /* skip : or / */
        p++;

        /* port */
        *port = 0;
        while (1)
        {
            if (!is_digit(*p))
            {
                break;
            }

            *port = (*port) * 10 + *p - '0';

            p++;
        }

        i = 0;
        while (1)
        {
            if (*p == 0)
            {
                break;
            }

            uri[i++] = *p++;

            if (i >= uri_len)
            {
                ret = TINY_RET_E_URL_INVALID;
                break;
            }
        }

    } while (0);

    return ret;
}