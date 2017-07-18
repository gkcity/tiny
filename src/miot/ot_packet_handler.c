
#include <tiny_malloc.h>
#include <tiny_log.h>
#include <channel/SocketChannel.h>
#include "ot_packet.h"
#include "ot_packet_handler.h"
#include "ot_handler.h"

#define TAG "OtPacketHandler"

static TinyRet OtPacketCtx_Dispose(OtPacketCtx* thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);\
    ByteBuf_Dispose(&thiz->buf);
    /*if (thiz->buf != NULL)
    	ByteBuf_Delete(thiz->buf);*/
    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

static TinyRet OtPacketCtx_Delete(OtPacketCtx* thiz)
{
    LOG_D(TAG, "OtPacketCtx_Delete");
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    OtPacketCtx_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet OtPacketCtx_Construct(OtPacketCtx* thiz, MiioCloudCtx * cloudCtx)
{
    TinyRet ret = TINY_RET_OK;
    LOG_D(TAG, "OtPacketCtx_Construct");

    memset(thiz, 0, sizeof(OtPacketCtx));

    ret = ByteBuf_Construct(&thiz->buf);
    if (RET_FAILED(ret))
    {
    	return ret;
    }
    /*thiz->buf = ByteBuf_New();
    if (thiz->buf == NULL)
    {
        return TINY_RET_E_OUT_OF_MEMORY;
    }*/
    thiz->state = PACKET_STATE_HEADER;
    thiz->cloudCtx = cloudCtx;

    return TINY_RET_OK;
}

static OtPacketCtx* OtPacketCtx_New(MiioCloudCtx * cloudCtx)
{
    OtPacketCtx *thiz = NULL;

    do
    {
        thiz = (OtPacketCtx *)tiny_malloc(sizeof(OtPacketCtx));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(OtPacketCtx_Construct(thiz, cloudCtx)))
        {
            OtPacketCtx_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;

}

static TinyRet OtPacketHandler_Construct(ChannelHandler *thiz, MiioCloudCtx * cloudCtx);
static TinyRet OtPacketHandler_Dispose(ChannelHandler *thiz);
static void OtPacketHandler_Delete(ChannelHandler *thiz);
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, void *data, uint32_t len);
static bool _channelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);
static void _channelEvent(ChannelHandler *thiz, Channel *channel, void *event);
static void _channelActive(ChannelHandler *thiz, Channel *channel);
static void _channelInActive(ChannelHandler *thiz, Channel *channel);


ChannelHandler * OtPacketHandler(MiioCloudCtx * cloudCtx)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(OtPacketHandler_Construct(thiz, cloudCtx)))
        {
            OtPacketHandler_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static void OtPacketHandler_Delete(ChannelHandler *thiz)
{
    LOG_D(TAG, "ExampleHandler_Delete");

    OtPacketHandler_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet OtPacketHandler_Construct(ChannelHandler *thiz, MiioCloudCtx * cloudCtx)
{
    LOG_D(TAG, "ExampleHandler_Construct");

    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, OtPacketHandler_Name, CHANNEL_HANDLER_NAME_LEN);
    thiz->onRemove = OtPacketHandler_Delete;
    thiz->inType = DATA_RAW;
    thiz->outType = DATA_RAW;
    thiz->channelActive = _channelActive;
    thiz->channelInactive = _channelInActive;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = _channelWrite;;
    thiz->channelEvent = _channelEvent;

    thiz->data = OtPacketCtx_New(cloudCtx);

    return TINY_RET_OK;
}

static TinyRet OtPacketHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    OtPacketCtx_Delete(thiz->data);
    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, void *data, uint32_t len)
{
    char* tmp = (char*)data;
    OtPacketCtx* ctx;
    OtPacketHeader* header;
    OtPacket* pack;
    int decLen;
    int ret;
    uint16_t length;
    uint16_t readable;

    
    if (thiz->data == NULL)
    {
        return false;
    }
    ctx = (OtPacketCtx*)thiz->data;
    ByteBuf_Write(&ctx->buf, tmp, len);
    LOG_D(TAG, "about to parse packet");
    while (true)
    {
        readable = ByteBuf_ReadableLen(&ctx->buf);
    	if (readable < sizeof(OtPacketHeader))
    	{
    	    LOG_W(TAG, "OT received (%d), less than size of OT header (32). %m, wait next\n", (int)readable);
    	    break;
    	}
        pack = (OtPacket*)ByteBuf_Buf(&ctx->buf);
    	header = (OtPacketHeader*)(ByteBuf_Buf(&ctx->buf));
    	length = ntohs(header->length);

        if (length > OT_MAX_PAYLOAD)
	{
	    LOG_E(TAG, "OT length (%d), bigger than max size of OT(%d).\n", length, OT_MAX_PAYLOAD);
	    ByteBuf_Clear(&ctx->buf);
	    break;
	}
    	    
    	if (length > readable)
    	{
    	    LOG_D(TAG, "OT received (%d), less than size of OT packet (%d). wait next\n", (int)readable, length);
    	    ctx->state = PACKET_STATE_BODY;
    	    break;
    	}
	ByteBuf_Handled(&ctx->buf, length);
    	decLen = OtPacket_Decode((char*)pack, length, &ctx->cloudCtx->miot->miio);
    	if (decLen < 0)
    	{
    	    LOG_E(TAG, "OT decode error");
    	    continue;
    	}
    	else if (decLen == 0)
    	{
    	    if (header->did == OT_DID_REQUEST) {
		/* time sync header */
		int ts = ntohl(header->timestamp);

		LOG_D(TAG, "Time sync msg from server\n");
		ctx->cloudCtx->miot->miio.ping_pong = true;
		report_synctime_ack(ctx->cloudCtx, ts);
		return 0;
	    } else {
		LOG_D(TAG, "Ping - Pong msg from server\n");
		ctx->cloudCtx->miot->miio.ping_pong = true;
	    }
    	}
    	else
    	{
    	    cloud_handle_msg(ctx->cloudCtx, pack->payload, decLen);
    	}
    }

    return true;
}

static bool _channelWrite(ChannelHandler * thiz,Channel * channel, ChannelDataType type,const void * data,uint32_t len)
{
	return false;
}

static void _channelEvent(ChannelHandler *thiz, Channel *channel, void *event)
{
	OtPacketCtx* ctx = (OtPacketCtx*)thiz->data;

    	LOG_D(TAG, "_channelEvent: %s, timeout", channel->id);

	report_keepalive(ctx->cloudCtx);
    //Channel_Close(channel);
}

void _channelActive(ChannelHandler *thiz, Channel *channel)
{
	OtPacketCtx* ctx;
	
	ctx = (OtPacketCtx*)thiz->data;
	ctx->cloudCtx->state = STATE_CLOUD_CONNECTED;

	report_otcinfo(ctx->cloudCtx);
}

void _channelInActive(ChannelHandler *thiz, Channel *channel)
{
	OtPacketCtx* ctx;
	
	ctx = (OtPacketCtx*)thiz->data;
	ctx->cloudCtx->state = STATE_CLOUD_DISCONNECTED;
}


