
#include "tiny_base.h"
#include "tiny_log.h"
#include "tiny_malloc.h"
#include "TinyContainerListener.h"

#include "ot_cloud.h"
#include "ot_packet.h"
#include "ot_packet_handler.h"
#include "ot_handler.h"

#include "timer/Timer.h"
#include "channel/SocketChannel.h"
#include "channel/stream/StreamClientChannel.h"
#include "channel/ChannelIdleStateHandler.h"

#include "miio_util.h"

#define TAG "OtCloud"

static void OtCloudInitializer(Channel *channel, void *ctx)
{
    LOG_D(TAG, "OtCloudInitializer: %s", channel->id);
    SocketChannel_AddLast(channel, ChannelIdleStateHandler(0, 0, 3));
    SocketChannel_AddLast(channel, OtPacketHandler(ctx));
}


static void _onServerRemove(void* data, void* ctx) {
    ServerListNode* node = (ServerListNode*)data;
    //tiny_free(node->data);
}

static void _onDataRemove(void* data, void* ctx) {
    DataDesc* desc = (DataDesc*)data;
    tiny_free(desc->data);
}

static TinyRet OtCloud_Dispose(MiioCloudCtx* thiz) {
    thiz->miot = NULL;
    TinyList_Delete(&thiz->udpServerInit);
    TinyList_Delete(&thiz->tcpServerInit);
    TinyList_Delete(&thiz->udpServers);
    TinyList_Delete(&thiz->tcpServers);
    thiz->curServerList = NULL;
    thiz->curServerPos = -1;
    TinyList_Delete(&thiz->dataQueue);
    TinyList_Delete(&thiz->dataQueueNoAck);
    memset(thiz->udpServersMd5, 0, sizeof(thiz->udpServersMd5));
    memset(thiz->tcpServersMd5, 0, sizeof(thiz->tcpServersMd5));
    StreamClientChannel_Delete(thiz->cloudChannel);
    Timer_Delete(thiz->timer);
    
    return TINY_RET_OK;
}

void OtCloud_Delete(MiioCloudCtx * thiz) {
    OtCloud_Dispose(thiz);
    tiny_free(thiz);
}

static void timerHandler(Timer* t, void* ctx)
{
	MiioCloudCtx* thiz = (MiioCloudCtx*)ctx;
	if (thiz->state == STATE_CLOUD_DISCONNECTED)
	{
		OtCloud_Reconnect(thiz);
	}
	else if (thiz->state == STATE_CLOUD_CONNECTED)
	{
		report_synctime(thiz);
	}
}

static TinyRet OtCloud_InitList(MiioCloudCtx* thiz, TinyList* list) {
    TinyRet ret = TINY_RET_OK;
    RETURN_VAL_IF_FAIL(list, TINY_RET_E_ARG_INVALID);
    TinyList_Construct(list);
/*    if (*list == NULL) {
        ret = TINY_RET_E_INTERNAL;
        OtCloud_Dispose(thiz);
    }*/
    return ret;
};


TinyRet OtCloud_Construct(MiioCloudCtx * thiz, Miot* miot) {
    TinyRet ret = TINY_RET_OK;
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->miot = miot;

	ret = OtCloud_InitList(thiz, &thiz->udpServerInit);
	if (RET_FAILED(ret)) break;

	ret = OtCloud_InitList(thiz, &thiz->tcpServerInit);
	if (RET_FAILED(ret)) break;
        
	ret = OtCloud_InitList(thiz, &thiz->udpServers);
	if (RET_FAILED(ret)) break;

	ret = OtCloud_InitList(thiz, &thiz->tcpServers);
	if (RET_FAILED(ret)) break;

	ret = OtCloud_InitList(thiz, &thiz->dataQueue);
	if (RET_FAILED(ret)) break;

	ret = OtCloud_InitList(thiz, &thiz->dataQueueNoAck);
	if (RET_FAILED(ret)) break;

	TinyList_SetDeleteListener(&thiz->udpServerInit, _onServerRemove, NULL);
	TinyList_SetDeleteListener(&thiz->tcpServerInit, _onServerRemove, NULL);
        TinyList_SetDeleteListener(&thiz->udpServers, _onServerRemove,  NULL);
        TinyList_SetDeleteListener(&thiz->tcpServers, _onServerRemove, NULL);
        TinyList_SetDeleteListener(&thiz->dataQueue, _onDataRemove, NULL);
        TinyList_SetDeleteListener(&thiz->dataQueueNoAck, _onDataRemove, NULL);

        thiz->curServerList = NULL;
        thiz->curServerPos = -1;
        //thiz->udpServerInitPos = -1;
        //thiz->udpServerPos = -1;
        //thiz->tcpServerInitPos = -1;
        //thiz->tcpServerPos = -1;
        thiz->retryCount = 0;
        thiz->serverType = SERVER_TCP;
        
        thiz->cloudChannel = StreamClientChannel_New();
        if (thiz->cloudChannel == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
        StreamClientChannel_Initialize(thiz->cloudChannel, OtCloudInitializer, thiz);
        thiz->state = STATE_CLOUD_DISCONNECTED;

        thiz->timer = Timer_New(TAG, TIMER_INTERVAL, thiz, timerHandler);
        if (thiz->timer == NULL)
        {
        	ret = TINY_RET_E_OUT_OF_MEMORY;
        	break;
        }
    } while (0);

    return ret;
}


MiioCloudCtx* OtCloud_New(Miot* miot) {
	MiioCloudCtx * thiz = NULL;
	
	do
	{
		thiz = (MiioCloudCtx *)tiny_malloc(sizeof(MiioCloudCtx));
		if (thiz == NULL)
		{
			break;
		}

		if (RET_FAILED(OtCloud_Construct(thiz, miot)))
		{
			OtCloud_Delete(thiz);
			thiz = NULL;
			break;
		}
	} while (0);
	
	return thiz;
}

TinyRet OtCloud_Start(MiioCloudCtx* thiz) {
    	TinyRet ret = TINY_RET_OK;
    	RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

	get_server_list(thiz, SERVER_TCP);
	Bootstrap_AddTimer(&thiz->miot->bootstrap, thiz->timer);
    	Bootstrap_AddChannel(&thiz->miot->bootstrap, thiz->cloudChannel);
    	OtCloud_Connect(thiz);
}

TinyRet OtCloud_Stop(MiioCloudCtx* thiz)
{
	Bootstrap_RemoveChannel(&thiz->miot->bootstrap, thiz->cloudChannel);
	Bootstrap_RemoveTimer(&thiz->miot->bootstrap, thiz->timer);
	OtCloud_Disconnect(thiz);
}


static DataDesc* DataDesc_New(char* buf, int len, int id, bool retry) {
    DataDesc* dd = NULL;

    dd = tiny_malloc(sizeof(DataDesc));
    if (dd == NULL) {
    	LOG_E(TAG, "malloc DataDesc error while sending to cloud");
    	return NULL;
    }
    memset(dd, 0, sizeof(DataDesc));
    dd->data = buf;
    dd->id = id;
    dd->size = len;
    if (retry) {
        dd->retry = 0;
        dd->ack_timeout = get_micro_second() / 1000 + ACK_TIMEOUT;
    } else {
        dd->retry = -1;
        dd->ack_timeout = 0;
    }
    return dd;
}

TinyRet _sendMessage(MiioCloudCtx* thiz, char* buf, int len, int id, bool retry) {
    int ret = TINY_RET_OK;
    DataDesc* dd = NULL;
    DataDesc* tmp = NULL;

    dd = DataDesc_New(buf, len, id, retry);
    if (dd = NULL) {
        return TINY_RET_E_OUT_OF_MEMORY;
    }
    TinyList_AddTail(&thiz->dataQueueNoAck, dd);
    SocketChannel_StartWrite(thiz->cloudChannel, DATA_RAW, buf, len); 
}

TinyRet OtCloud_SendMessage(MiioCloudCtx* thiz, char* payload, int id, int payloadLen, bool retry) {
    Channel* cloudChannel = thiz->cloudChannel;
    char* msg;
    int len = payloadLen;
    
    if (!StreamClientChannel_isConnected(cloudChannel)) {
    	LOG_E(TAG, "cloud socket not connected");
    	return TINY_RET_E_NOT_INITIALIZED;
    }

    msg = OtPacket_Encode(payload, &len, &thiz->miot->miio);
    if (msg == NULL) {
    	LOG_E(TAG, "construct packet error");
    	return TINY_RET_E_INTERNAL;
    }
    if (len >= OT_MAX_PAYLOAD) {
    	LOG_E(TAG, "packet too long");
    	tiny_free(msg);
    	return TINY_RET_E_INTERNAL;
    }
    return _sendMessage(thiz, msg, len, id, retry);
}

TinyRet OtCloud_Connect(MiioCloudCtx* thiz)
{
	ServerListNode* server;
	ServerListNode* oldServer;
	char ipstr[INET6_ADDRSTRLEN];
	uint16_t port;


	oldServer = TinyList_GetAt(thiz->curServerList, thiz->curServerPos);
	server = server_pick(thiz, thiz->serverType);
	if (server == NULL)
	{
		LOG_E(TAG, "server pick null");
		server = oldServer;
	}
	if (server == NULL)
	{
		LOG_E(TAG, "server is null, return error");
		return TINY_RET_E_INTERNAL;
	}

	thiz->state = STATE_CLOUD_CONNECTING;
	thiz->retryCount++;
	ipstr[0] = '\0';
	port = get_port(&server->addr);
	get_ip_str(&server->addr, ipstr, INET6_ADDRSTRLEN);
	LOG_D(TAG, "Retry (%d), host ip: %s, port: %d\n", thiz->retryCount, ipstr, port);

	StreamClientChannel_Connect(thiz->cloudChannel, ipstr, port);

}

TinyRet OtCloud_Reconnect(MiioCloudCtx* thiz)
{
	Channel* cloudChannel = thiz->cloudChannel;


	if (StreamClientChannel_isConnected(cloudChannel))
	{
		StreamClientChannel_Close(cloudChannel);
	}
	OtCloud_Connect(thiz);
}

TinyRet OtCloud_Disconnect(MiioCloudCtx* thiz)
{
	return StreamClientChannel_Close(thiz->cloudChannel);
}

