
#ifndef __OT_CLOUD_H__
#define __OT_CLOUD_H__

#include <TinyList.h>
#include <channel/stream/StreamClientChannel.h>
#include "miio.h"
#include "miot.h"
#include "timer/Timer.h"
#include "TinyList.h"
#include "tiny_socket.h"
#include "tiny_base.h"

/* max retry before we give up a packet, for TCP, we do not actually need any retry */
#define MAX_RETRY_TCP			0
#define MAX_RETRY_UDP			3

/* MAX tolerate count before we switch to next server */
#define SERVER_NOACK_MAX		3

#define POLL_TIMEOUT			(100)	/* 100ms */

#define OT_DID_REQUEST			0xFFFFFFFFFFFFFFFFLL
#define OT_TOKEN_REQUEST		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define OT_TS_REQUEST			"\xFF\xFF\xFF\xFF"



TINY_BEGIN_DECLS

typedef enum _ServerTypeEnum {
	SERVER_UNKNOWN,
	SERVER_TCP,
	SERVER_UDP
} ServerTypeEnum;

typedef enum _CloudStateEnum {
	STATE_CLOUD_DISCONNECTED,
	STATE_CLOUD_CONNECTING,
	STATE_CLOUD_CONNECTED,
} CloudStateEnum;

typedef struct _ServerListNode {
	ServerTypeEnum server_type;
	struct sockaddr addr;
	socklen_t addrlen;
} ServerListNode;

typedef struct _DataDesc {
	int id;		/* json id to identify this desc */
	char *data; /* porint to data */
	int size; /* data size */
	int retry; /* num of retries; -1 indicates no retry needed; */
	uint64_t ack_timeout; /* timeout (ms) for retry; MONOTONIC time */
} DataDesc;

typedef struct _MiioCloudCtx {
	TinyList udpServerInit;
	TinyList tcpServerInit;
	TinyList udpServers;
	uint8_t udpServersMd5[AES_BLOCK_SIZE];
	TinyList tcpServers;
	uint8_t tcpServersMd5[AES_BLOCK_SIZE];

	TinyList* curServerList;
	uint32_t curServerPos;
	
	/*uint32_t udpServerInitPos;
	uint32_t udpServerPos;
	uint32_t tcpServerInitPos;
	uint32_t tcpServerPos;*/
	//ServerListNode *server_save;

	ServerTypeEnum serverType;

	unsigned int report_otcinfo_flag;
	int report_ota_state_idle;

	TinyList dataQueue; /* data queue to cloud */
	TinyList dataQueueNoAck; /* data queue waiting for ack from cloud */

	Channel* cloudChannel; 
	CloudStateEnum state;
	uint32_t retryCount;

	Timer* timer;

	Miot* miot;
} MiioCloudCtx;

MiioCloudCtx* OtCloud_New(Miot* miot);
TinyRet OtCloud_Start(MiioCloudCtx* thiz);
TinyRet OtCloud_SendMessage(MiioCloudCtx* thiz, char* buf, int id, int len, bool retry);
TinyRet OtCloud_Connect(MiioCloudCtx* thiz);
TinyRet OtCloud_Reconnect(MiioCloudCtx* thiz);
TinyRet OtCloud_Disconnect(MiioCloudCtx* thiz);
TinyRet OtCloud_Stop(MiioCloudCtx* thiz);
void OtCloud_Delete(MiioCloudCtx* thiz);

TINY_END_DECLS

#endif
