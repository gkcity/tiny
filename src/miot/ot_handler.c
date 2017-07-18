#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <tiny_base.h>
#include <tiny_log.h>
#include "json.h"
#include <tiny_socket.h>
#include "tiny_malloc.h"

#include "miio.h"
#include "miio_util.h"
#include "ot_handler.h"
#include "ot_cloud.h"

#define TAG "OtHandler"


ServerListNode* server_pick(MiioCloudCtx* cloud, ServerTypeEnum server_type)
{
	int tcpInitLen, tcpLen, udpInitLen, udpLen;
	RETURN_VAL_IF_FAIL(cloud, NULL);

	tcpInitLen = TinyList_GetCount(&cloud->tcpServerInit);
	tcpLen = TinyList_GetCount(&cloud->tcpServers);
	udpInitLen = TinyList_GetCount(&cloud->udpServerInit);
	udpLen = TinyList_GetCount(&cloud->udpServers);
	if (server_type == SERVER_UNKNOWN)
	{
		server_type = SERVER_TCP;
	}

	if (server_type == SERVER_TCP)
	{
		if (TinyList_IsEmpty(&cloud->tcpServerInit))
		{
			LOG_E(TAG, "you even do not get server list first");
			return NULL;
		}

		if (cloud->curServerList == NULL ||cloud->curServerList == &cloud->udpServerInit ||cloud->curServerList == &cloud->udpServers)
		{
			cloud->curServerList = &cloud->tcpServerInit;
			cloud->curServerPos = 0;
			//cloud->tcpServerInitPos++;
			return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
		}

		if (cloud->curServerList == &cloud->tcpServerInit)
		{
			if (cloud->curServerPos >= tcpInitLen -1)
			{
				if (TinyList_IsEmpty(&cloud->tcpServers))
				{
					cloud->curServerPos = 0;
					//cloud->tcpServerInitPos = 0;
					return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
				}
				else
				{
					cloud->curServerList = &cloud->tcpServers;
					cloud->curServerPos = 0;
					//cloud->tcpServerPos = 0;
					return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
				}
			}
			else
			{
				cloud->curServerPos++;
				//cloud->tcpServerInitPos++;
				return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
			}
		}
		else if (cloud->curServerList == &cloud->tcpServers)
		{
			if (cloud->curServerPos >= tcpLen - 1)
			{
				cloud->curServerList = &cloud->tcpServerInit;
				cloud->curServerPos = 0;
				//cloud->tcpServerInitPos = 0;
				return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
			}
			else
			{
				cloud->curServerPos++;
				//cloud->tcpServerPos++;
				return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
			}
		}
	}
	else
	{
		if (TinyList_IsEmpty(&cloud->udpServerInit))
		{
			LOG_E(TAG, "you even do not get udp server list first");
			return NULL;
		}

		if (cloud->curServerList == NULL ||cloud->curServerList == &cloud->tcpServerInit || cloud->curServerList == &cloud->tcpServers)
		{
			cloud->curServerList = &cloud->udpServerInit;
			cloud->curServerPos = 0;
			//cloud->udpServerInitPos++;
			return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
		}

		if (cloud->curServerList == &cloud->udpServerInit)
		{
			if (cloud->curServerPos >= udpInitLen -1)
			{
				if (TinyList_IsEmpty(&cloud->udpServers))
				{
					cloud->curServerPos = 0;
					//cloud->udpServerInitPos = 0;
					return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
				}
				else
				{
					cloud->curServerList = &cloud->udpServers;
					cloud->curServerPos = 0;
					//cloud->udpServerPos = 0;
					return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
				}
			}
			else
			{
				cloud->curServerPos++;
				//cloud->udpServerInitPos++;
				return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
			}
		}
		else if (cloud->curServerList == &cloud->udpServers)
		{
			if (cloud->curServerPos >= udpLen - 1)
			{
				cloud->curServerList = &cloud->udpServerInit;
				cloud->curServerPos = 0;
				//cloud->udpServerInitPos = 0;
				return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
			}
			else
			{
				cloud->curServerPos++;
				//cloud->udpServerPos++;
				return TinyList_GetAt(cloud->curServerList, cloud->curServerPos);
			}
		}
	}

	return NULL;
}

int get_server_list(MiioCloudCtx* cloud, ServerTypeEnum server_type)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sockfd, ret;
	char buf[MAX_BUFLEN];
	ServerListNode *server;
	MiioCtx* miio = &(cloud->miot->miio);

	if (server_type == SERVER_UNKNOWN)
	{
		server_type = SERVER_TCP;
	}

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = (server_type == SERVER_TCP) ? SOCK_STREAM : SOCK_DGRAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	if (server_type == SERVER_TCP)
		ret = getaddrinfo(OT_TCP_HOST, OT_TCP_PORT, &hints, &result);
	else
		ret = getaddrinfo(OT_UDP_HOST, OT_UDP_PORT, &hints, &result);

	if (ret != 0) {
		log_printf(LOG_INFO, "getaddrinfo: %d\n", ret);
		return ret;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		char ipstr[INET6_ADDRSTRLEN];
		uint16_t port;

		ret = getnameinfo(rp->ai_addr, rp->ai_addrlen, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
		if (ret != 0) {
			LOG_W(TAG, "error in getnameinfo: %d\n", ret);
			continue;
		}

		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1) {
			LOG_W(TAG, "error in create socket: %m\n");
			continue;
		}

		if (rp->ai_socktype == SOCK_STREAM &&
		    connect(sockfd, rp->ai_addr, rp->ai_addrlen) != 0) {
			LOG_W(TAG, "error in trying to connect socket: %m\n");
			continue;
		}

		close(sockfd);

		/* prepend to server_list if not already there */
		server = malloc(sizeof(ServerListNode));
		if (server == NULL) {
			LOG_E(TAG, "%s, %d: malloc fail\n", __FILE__, __LINE__);
			continue;
		}

		server->server_type = server_type;
		memcpy(&server->addr, rp->ai_addr, sizeof(struct sockaddr));
		server->addrlen = rp->ai_addrlen;

		get_ip_str(&server->addr, ipstr, INET6_ADDRSTRLEN);
		port = get_port(&server->addr);
		LOG_I(TAG, "Add %s server into list, host ip: %s, port: %d\n",
		       (server_type == SERVER_TCP ? "TCP" : "UDP"),
		       ipstr, port);

		if (server_type == SERVER_UDP) {
			TinyList_AddHead(&cloud->udpServerInit, server);
		} else {
			TinyList_AddHead(&cloud->tcpServerInit, server);
		}
	}

	freeaddrinfo(result);
	return 0;
}


int report_synctime_ack(MiioCloudCtx* cloud, int server_time)
{
	unsigned int mono = timestamp();
	int ret;
	MiioCtx* miio = &cloud->miot->miio;

	miio->timestamp_diff = server_time - mono;

	LOG_I(TAG, "sync time with server, server: %d, local(mono): %u, diff: %d\n",
		   server_time, mono, miio.timestamp_diff);

	if (miio->dev_info.did == MAGIC_DID_TYPE2) {
		/* register {did, key} */
		if (miio->didkeyreq_timestamp == 0 ||
		    timestamp() - miio->didkeyreq_timestamp > miio->didkeyreq_interval) {
			//ret = request_didkeymac2();
			LOG_I(TAG, "no {did, key, mac} 2, request from server and ret: %d.\n", ret);

			miio->didkeyreq_timestamp = timestamp();
		}

//		LOG_I(TAG, "STATE: (%d) -> STATE_DIDKEY_REQ2\n", miio.state);
//		miio->state = STATE_DIDKEY_REQ2;
	} else {
		/* start _otc.info process */
//		cmd_internal_get_info();
	}

	return 0;
}


/**
 * miIO.wifi_assoc_state
 */
static int method_miio_wifi_assoc_state(MiioCtx* miio, JsonObject* jo, char *buf, int buflen)
{
	char *status;
	int id = -1;
	int ret = 0;
	JsonElement* je;

	char *string_template =
		"{\"id\":%d,\"code\":0,\"message\":\"ok\",\"result\":"
		"{\"state\":\"%s\","
		"\"auth_fail_count\":0,"
		"\"conn_success_count\":1,"
		"\"conn_fail_count\":0,"
		"\"dhcp_fail_count\":0}}";

	if (miio->dev_info.wpa_state)
		status = "ONLINE";
	else
		status = "CONNECTING";
	je = json_object_get_value(jo, "id");
	if (je == NULL)
	{
	    LOG_E(TAG, "can not get id");
	} else {
	    id = json_value_get_number(je);
	}
	
	/* check buf enough or not */
	if (strlen(string_template) + 10 /* max int*/ +
	    strlen("CONNECTING")
	    >= buflen) {
		LOG_E(TAG, "json string too long. %s, line %d\n", __FILE__, __LINE__);
		return -1;
	}

	sprintf(buf, string_template, id, status);
	return 0;
}


/**
 * miIO.config
 */
static int method_miio_config(MiioCtx* miio, JsonObject* jo, char *buf, int buflen)
{
	JsonElement* je;
	JsonObject *params;
	int id, ret, enauth;
	FILE *fp;
	char path[128];

	params = json_object_get_object(jo, "params");
	if (NULL == params)
	{
	    LOG_E(TAG, "no params");
	    return -1;
	}

	je = json_object_get_value(params, "enauth");
	if (NULL == je)
	{
	    LOG_E(TAG, "no enauth");
	}
	else
	{
	    enauth = json_value_get_number(je);
	    LOG_D(TAG, "miIO.config, enauth: %d", enauth);

	    /* create the flag file */
	    ret = snprintf(path, sizeof(path), "%s%s", miio->datadir, OTD_DONOT_TRUST_PEER_FILE);
            if (ret < 0) {
		return ret;
	    } else if (ret >= sizeof(path)) {
		return ret;
	    } else {
		path[ret] = '\0';
            }
	    fp = fopen(path, "w+");
	    if (fp == NULL) {
		LOG_E(TAG, "can't create %s: %m", path);
		return -1;
            }
	    fclose(fp);
	}


	ret = json_object_get_number(jo, "id");
	if (ret <=  0) {
		ret = 0;
	} else {
		char *string_template = "{\"id\":%d,\"result\":[\"OK\"]}";

		/* check buf enough or not */
		if (strlen(string_template) + 10 /* max int*/
		    >= buflen)
			LOG_E(TAG, "json string too long. %s, line %d", __FILE__, __LINE__);
		else
			sprintf(buf, string_template, id);
	}


	return 0;
}


/**
 * miIO.info
 */
static int method_miio_info(MiioCtx* miio, JsonObject* jo,  char *buf, int buflen)
{
	char *newmsg;
	char tokenstr[32+1];
	int id = -1;
	int ret = 0;
	JsonElement* je;
	JsonObject* jinfo;
	JsonObject* jparams;
	JsonElement* jtmp;

	if (miio->info_str == NULL)
		return -1;

	je = json_object_get_value(jo, "id");
	if (je == NULL)
	{
		LOG_E(TAG, "can not get id");
	} else {
		id = json_value_get_number(je);
	}

	memset(tokenstr, 0, sizeof(tokenstr));
	str2hex(tokenstr, miio->dev_info.token, 16);

	je = json_parse_string(miio->info_str);
	if (NULL == je)
	{
		LOG_E(TAG, "info_str parse error, maybe not a json");
		return -1;
	}
	do
	{
		jinfo = json_value_get_object(je);
		json_object_set_number(jinfo, "id", id);
		json_object_set_number(jinfo, "code", 0);
		json_object_set_string(jinfo, "message", "ok");

		jparams = json_object_get_object(jinfo, "params");
		json_object_set_string(jparams, "model", miio->dev_info.model);
		json_object_set_string(jparams, "mac", miio->dev_info.mac);
		json_object_set_string(jparams, "token", tokenstr);
		json_object_set_number(jparams, "life", get_uptime());

		//json_object_add(jinfo, "result", jparams);
		json_object_set_value(jinfo, "result", json_object_get_wrapping_value(jparams));

		newmsg = json_serialize_to_string(je);
		if (newmsg == NULL)
		{
			LOG_E(TAG, "json build error");
			ret = -1;
			break;
		}
		if (strlen(newmsg) >= buflen)
		{
			LOG_E(TAG, "json string too long. %s, line %d:%s", __FILE__, __LINE__, newmsg);
			tiny_free(newmsg);
			ret = -1;
			break;
		}
		memcpy(buf, newmsg, strlen(newmsg));
		tiny_free(newmsg);
	} while(0);
	json_value_free(je);

	return ret;
}

static bool ackSelector(void* data, void* ctx)
{
	DataDesc* dd = (DataDesc*)data;
	int id = *((int*)ctx);

	if (dd->id == id)
	{
		return true;
	}
	return false;
}

static TinyRet queueAck(MiioCloudCtx* cloud, int id)
{
	TinyRet ret = TINY_RET_OK;
	int pos = -1;
	RETURN_VAL_IF_FAIL(cloud, TINY_RET_E_ARG_INVALID);

	do
	{
		pos = TinyList_Foreach(&cloud->dataQueueNoAck, ackSelector, &id);
		if (pos < 0)
		{
			LOG_E(TAG, "the id of msg from server is invalid: %d", id);
			ret = TINY_RET_E_ARG_INVALID;
			break;
		}
		ret = TinyList_RemoveAt(&cloud->dataQueueNoAck, pos);
	} while(0);
	return ret;
}

TinyRet cloud_handle_msg(MiioCloudCtx* cloud, char* msg, int len)
{
    TinyRet ret = TINY_RET_OK;
    char ackbuf[OT_MAX_PAYLOAD];
    JsonElement* je;
    JsonObject* jo;
    const char* method;
    int id;
    MiioCtx* miio = &cloud->miot->miio;

    LOG_D(TAG, "%s, msg: %s, strlen: %d, len: %d\n", __func__, msg, (int)strlen(msg), len);
	je = json_parse_string(msg);
	if (NULL == je)
	{
		LOG_E(TAG, "msg from server is not a json");
		return TINY_RET_E_INTERNAL;
	}

    do
    {

	        jo = json_value_get_object(je);
	        if (NULL == jo)
	        {
	            /* just send to so invoker */
	            LOG_D(TAG, "not an object, maybe array");
	            if (cloud->miot->cloudCallback != NULL)
	            {
	                cloud->miot->cloudCallback(msg, len);
	            }
	            break;
	        }
	        else
	        {
	        	id = json_object_get_number(jo, "id");
	        	if (id > 0)
	        	{
	        		if (RET_FAILED(queueAck(cloud, id)))
	        		{
	        			LOG_E(TAG, "remove data desc from dataNoAck queue error");
	        		}
	        	}
		        memset(ackbuf, 0, sizeof(ackbuf));
		        method = json_object_get_string(jo, "method");
		        if (method == NULL)
		        {
		                /* just send to so invoker */
		                LOG_D(TAG, "not a method cmd");
		                if (cloud->miot->cloudCallback != NULL)
		                {
		                    cloud->miot->cloudCallback(msg, len);
		                }
		            }
			    else if (STR_EQUAL(method, "miIO.wifi_assoc_state"))
			    {
			        LOG_D(TAG, "got miIO.wifi_assoc_state");
			        method_miio_wifi_assoc_state(miio, jo, ackbuf, sizeof(ackbuf));
			    }
			    else if (STR_EQUAL(method, "miIO.stop_diag_mode"))
			    {
			        LOG_D(TAG, "got miIO.stop_diag_mode");
			    }
			    else if (STR_EQUAL(method, "miIO.config"))
			    {
			    	LOG_I(TAG, "Got miIO.config");
			        if (method_miio_config(miio, jo, ackbuf, sizeof(ackbuf)) < 0)
				    LOG_W(TAG, "miIO.config fail");
			    }
			    else if (STR_EQUAL(method, "miIO.info"))
			    {
			    	LOG_I(TAG, "Got miIO.info");
			    	if (method_miio_info(miio, jo, ackbuf, sizeof(ackbuf) < 0))
			    		LOG_W(TAG, "miIO.info failed");
			    }
			    else if (STR_EQUAL(method, "miIO.restore"))
			    {
			    	LOG_I(TAG, "Got miIO.restore");
			    	if (cloud->miot->cloudCallback != NULL)
		                {
		                    cloud->miot->cloudCallback(msg, len);
		                }
		                break;
			    }
			    else if (STR_EQUAL(method, "miIO.reboot"))
			    {
			    	LOG_I(TAG, "Got miIO.reboot");
			    	if (cloud->miot->cloudCallback != NULL)
		                {
		                    cloud->miot->cloudCallback(msg, len);
		                }
		                break;
			    }
			    else if (STR_EQUAL(method, "_otc.info"))
			    {
			    	
			    }
			    else
			    {
			        LOG_D(TAG, "Got %s unhandled", method);
		                if (cloud->miot->cloudCallback != NULL)
		                {
		                    cloud->miot->cloudCallback(msg, len);
		                }
		                break;
			    }
	            
	        }
        }while(0);
        
        json_value_free(je);

    return ret;

}


/*
 * Keep alive heartbeat packet.
 *
 * Also use the miio.ping_pong flag to detect network disconnection, the
 * strategy is like this:
 *
 * On every keepalive ping packet we set ping_pong to false, and when
 * server response pong packet, ping_pong is set to true; so on next
 * keepalive ping packet we check and see if ping_pong is true or not to
 * detect whether the server had responsed.
 */

TinyRet report_keepalive(MiioCloudCtx* cloud)
{
	MiioCtx* miio = &cloud->miot->miio;
	TinyRet ret = TINY_RET_OK;

	RETURN_VAL_IF_FAIL(cloud, TINY_RET_E_ARG_INVALID);

	do
	{
		if (!miio->ping_pong) {
			miio->server_noack++;
			LOG_D(TAG, "%s,%d: server_noack: %d\n", __func__, __LINE__, miio->server_noack);
			if (miio->server_noack >= SERVER_NOACK_MAX) {
				miio->server_noack = 0;
				LOG_D(TAG, "%s,%d: otc_close_and_retry\n", __func__, __LINE__);
				ret = OtCloud_Reconnect(cloud);
				break;
			}
		}

		ret = OtCloud_SendMessage(cloud, NULL, -1, 0, false);
		if (RET_FAILED(ret))
		{
			miio->ping_pong = false;
		}
	} while(0);

	return ret;
}


TinyRet report_synctime(MiioCloudCtx* cloud)
{
	MiioCtx* miio = &cloud->miot->miio;
	TinyRet ret = TINY_RET_OK;
	uint64_t didsave = miio->dev_info.did;

	RETURN_VAL_IF_FAIL(cloud, TINY_RET_E_ARG_INVALID);

	do
	{
		if (!miio->ping_pong) {
			miio->server_noack++;
			LOG_D(TAG, "%s,%d: server_noack: %d\n", __func__, __LINE__, miio->server_noack);
			if (miio->server_noack >= SERVER_NOACK_MAX) {
				miio->server_noack = 0;
				LOG_D(TAG, "%s,%d: otc_close_and_retry\n", __func__, __LINE__);
				ret = OtCloud_Reconnect(cloud);
				break;
			}
		}

		miio->dev_info.did = OT_DID_REQUEST;
		ret = OtCloud_SendMessage(cloud, NULL, -1, 0, false);
		miio->dev_info.did = didsave;
		if (RET_FAILED(ret))
		{
			miio->ping_pong = false;
		}
	} while(0);

	return ret;
}

static bool printDataDesc(void* data, void* ctx)
{
	DataDesc* dd = (DataDesc*)data;
	LOG_D(TAG, "data desc: id:%d, size:%d, retry:%d, ack_timeout:%llu", dd->id, dd->size, dd->retry, (unsigned long long)dd->ack_timeout);
	return true;
}

TinyRet report_otcinfo(MiioCloudCtx* cloud)
{
	char tokenstr[32+1];
	struct callback *node;
	int id = generate_random_id();
	JsonElement* je;
	JsonElement* jtmp;
	JsonObject* jo;
	char *newmsg;
	MiioCtx* miio = &cloud->miot->miio;
	TinyRet ret = TINY_RET_OK;

	LOG_I(TAG, "report_otcinfo_flag: %u\n", cloud->report_otcinfo_flag);
	if (cloud->report_otcinfo_flag > 0) {
		/* should be in data_queue_noack? */
		TinyList_Foreach(&cloud->dataQueueNoAck, printDataDesc, NULL);
		return 0;
	}

	do
	{
		memset(tokenstr, 0, sizeof(tokenstr));
		str2hex(tokenstr, miio->dev_info.token, 16);

		je = json_parse_string(miio->info_str);
		if (je == NULL)
		{
			LOG_E(TAG, "info_str parse error, maybe not a json");
			ret = TINY_RET_E_INTERNAL;
			break;
		}

		jo = json_value_get_object(je);
		json_object_set_number(jo, "id", id);
		json_object_set_string(jo, "method", "_otc.info");

		jo = json_object_get_object(jo, "params");
		json_object_set_string(jo, "model", miio->dev_info.model);
		json_object_set_string(jo, "mac", miio->dev_info.mac);
		json_object_set_string(jo, "token", tokenstr);
		json_object_set_number(jo, "life", get_uptime());

		newmsg = json_serialize_to_string(je);
		if (newmsg == NULL)
		{
			LOG_E(TAG, "json build error");
			ret = TINY_RET_E_INTERNAL;
			break;
		}
		if (strlen(newmsg) >= OT_MAX_PAYLOAD)
		{
			LOG_E(TAG, "json string too long. %s, line %d:%s", __FILE__, __LINE__, newmsg);
			ret = TINY_RET_E_INTERNAL;
			break;
		}
		ret = OtCloud_SendMessage(cloud, NULL, -1, 0, false);
		if (RET_FAILED(ret))
		{
			LOG_E(TAG, "report _otc.info error while sending");
			break;
		}

		LOG_D(TAG, "report to cloud _otc.info: %s", newmsg);
		cloud->report_otcinfo_flag = id;
	}while(0);
	if (newmsg != NULL) tiny_free(newmsg);
	if (je != NULL) json_value_free(je);
	return ret;
}

