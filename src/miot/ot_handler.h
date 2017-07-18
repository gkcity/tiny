#ifndef __OT_HANDLER__
#define __OT_HANDLER__

#include "ot_cloud.h"

int get_server_list(MiioCloudCtx* miio, ServerTypeEnum server_type);

ServerListNode* server_pick(MiioCloudCtx* cloud, ServerTypeEnum server_type);

TinyRet report_synctime(MiioCloudCtx* cloud);
int report_synctime_ack(MiioCloudCtx* cloud, int server_time);


int reportSynctimeAck(MiioCtx* miio, int serverTime);

TinyRet cloud_handle_msg(MiioCloudCtx* cloud, char* msg, int len);

int report_keepalive(MiioCloudCtx* cloud);

TinyRet report_otcinfo(MiioCloudCtx* cloud);


#endif
