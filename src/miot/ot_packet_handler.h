
#ifndef __OT_PACKET_HANDLER__
#define __OT_PACKET_HANDLER__

#include <channel/ChannelHandler.h>
#include "ot_cloud.h"
#include "ByteBuf/byte_buf.h"

#define OtPacketHandler_Name  "OtPacketHandler"

TINY_BEGIN_DECLS
;
typedef enum _StateEnum
{
    PACKET_STATE_HEADER,
    PACKET_STATE_BODY,
    PACKET_STATE_ERROR
} StateEnum;

typedef struct _OtPacketCtx
{
    ByteBuf  buf;
    StateEnum state;
    int index;

    MiioCloudCtx* cloudCtx;
} OtPacketCtx;

ChannelHandler * OtPacketHandler(MiioCloudCtx * cloudCtx);


TINY_END_DECLS

#endif /* __EXAMPLE_HANDLER_H__ */

