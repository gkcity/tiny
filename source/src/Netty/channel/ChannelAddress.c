/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ChannelAddress.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_snprintf.h>
#include "ChannelAddress.h"

TINY_LOR
void ChannelAddress_Set(ChannelAddress *thiz, ChannelType type, uint32_t address, uint16_t port)
{
    switch (type)
    {
        case TYPE_UDP:
        case TYPE_TCP_SERVER:
        case TYPE_TCP_CONNECTION:
            thiz->socket.address = address;
            thiz->socket.port = port;
            const uint8_t *a = (uint8_t *)&address;
            tiny_snprintf(thiz->socket.ip, TINY_IP_LEN, "%d.%d.%d.%d", a[0], a[1], a[2], a[3]);
            break;

        case TYPE_RS232:
            break;

        case TYPE_RS485:
            break;
    }
}