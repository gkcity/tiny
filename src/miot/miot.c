#include <tiny_log.h>
#include "tiny_base.h"
#include "tiny_malloc.h"

#include <channel/SocketChannel.h>
#include <channel/stream/StreamServerChannel.h>
#include <channel/stream/StreamClientChannel.h>
#include <channel/ChannelIdleStateHandler.h>

#include "miot.h"
#include "json.h"
#include "ot_cloud.h"
#include "miio_util.h"

#define TAG "MIOT"

static TinyRet my_socket_init(void)
{
	TinyRet ret = TINY_RET_OK;
	static bool is_init = false;

	do
	{
#ifdef _WIN32
		WORD wVersionRequested;
		WSADATA wsaData;
#endif

#ifdef _WIN32
		wVersionRequested = MAKEWORD(2, 0);
		if (WSAStartup(wVersionRequested, &wsaData) != 0)
		{
			LOG_W(TAG, "WSAStartup failed");
			ret = TINY_RET_E_INTERNAL;
			break;
		}
#else
		// Ignore SIGPIPE signal, so if browser cancels the request, it won't kill the whole process.
		//(void)signal(SIGPIPE, SIG_IGN);
#endif

		ret = TINY_RET_OK;
	} while (0);

	return ret;
}

/*
static void OtServerInitializer(Channel *channel, void *ctx)
{
    LOG_D(TAG, "HttpServerInitializer: %s", channel->id);
    SocketChannel_AddLast(channel, ChannelIdleStateHandler(0, 0, 3));
    //SocketChannel_AddLast(channel, ExampleHandler());
    //SocketChannel_AddBefore(channel, ExampleHandler_Name, HttpMessageCodec());
}

static void OtCloudInitializer(Channel *channel, void *ctx)
{
    LOG_D(TAG, "OtCloudInitializer: %s", channel->id);
    SocketChannel_AddLast(channel, ChannelIdleStateHandler(0, 0, 3));
    SocketChannel_AddLast(channel, OtPacketHandler());
}
*/
void Miot_Dispose(Miot* thiz)
{
    if (thiz->cloud != NULL)
    {
        OtCloud_Delete(thiz->cloud);
        thiz->cloud = NULL;
    }
    Bootstrap_Dispose(&thiz->bootstrap);
}

void Miot_Delete(Miot* thiz)
{
    Miot_Dispose(thiz);
    tiny_free(thiz);
}

 TinyRet Miot_Construct(Miot* thiz)
 {
     TinyRet ret = TINY_RET_OK;
     thiz->cloud = OtCloud_New(thiz);
     if (NULL == thiz->cloud)
     {
         return TINY_RET_E_INTERNAL;
     }
     ret = Bootstrap_Construct(&thiz->bootstrap);
     return ret;
 }


 Miot* Miot_New()
{

    my_socket_init();

    Miot* thiz = NULL;
    do
    {
        thiz = (Miot*)tiny_malloc(sizeof(Miot));
        if (thiz == NULL)
        {
            LOG_E(TAG, "malloc for miot error");
            break;
        }
        if (RET_FAILED(Miot_Construct(thiz)))
        {
            Miot_Delete(thiz);
            LOG_E(TAG, "construct for miot error");
            break;
        }
    } while(0);
    return thiz;
}

TinyRet Miot_SetDInfo(Miot* thiz, uint64_t did, const char* model, const char* vendor, const char* key)
{
	MiioCtx* miio = &thiz->miio;
	MiioDevInfo* devinfo = &miio ->dev_info;

	RETURN_VAL_IF_FAIL(did, TINY_RET_E_ARG_INVALID);
	RETURN_VAL_IF_FAIL(model, TINY_RET_E_ARG_INVALID);
	RETURN_VAL_IF_FAIL(vendor, TINY_RET_E_ARG_INVALID);
	RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_INVALID);

	miio->dev_info.did = did;
	memcpy(devinfo->model, model, sizeof(devinfo->model));
	memcpy(devinfo->vendor, vendor, sizeof(devinfo->vendor));
	memcpy(devinfo->key, key, sizeof(devinfo->key));
	return TINY_RET_OK;
}

TinyRet Miot_SetDToken(Miot* thiz, const char* token)
{
	MiioDevInfo* devinfo = &thiz->miio.dev_info;

	if (token == NULL)
	{
		rand_str(devinfo->token, 16);
	}
	else
	{
		memcpy(devinfo->token, token, sizeof(devinfo->token));
	}
	return TINY_RET_OK;
}

TinyRet Miot_SetNetInfo(Miot* thiz, const char* info)
{
	JsonElement* je;
	JsonObject* jo;
	JsonObject* jtmp;
	const char* tmp = NULL;
	MiioDevInfo* devinfo = &thiz->miio.dev_info;
	TinyRet ret = TINY_RET_E_INTERNAL;

	RETURN_VAL_IF_FAIL(info, TINY_RET_E_ARG_INVALID);

	je = json_parse_string(info);
	if (NULL == je)
	{
		LOG_E(TAG, "Net info parse error, maybe not a json");
		return ret;
	}

	do
	{
		jo = json_value_get_object(je);
		if (NULL == jo)
		{
			LOG_E(TAG, "Net info format invalid, not a json object");
			break;
		}
		
		jtmp = json_object_get_object(jo, "params");
		if (NULL == jtmp)
		{
			LOG_E(TAG, "Net info has no params");
			break;
		}
		jo = jtmp;
		jtmp = json_object_get_object(jo, "netif");
		if (NULL == jtmp)
		{
			LOG_E(TAG, "Net info has no netif");
			break;
		}
		tmp = json_object_get_string(jtmp, "localIp");
		if (tmp == NULL)
		{
			LOG_E(TAG, "Net info has no local ip");
			break;
		}
		memcpy(devinfo->ip, tmp, sizeof(devinfo->ip));
		if (thiz->miio.info_str != NULL)
		{
			tiny_free(thiz->miio.info_str);
		}
		thiz->miio.info_str = strdup(info);
		if (thiz->miio.info_str == NULL)
		{
			LOG_E(TAG, "duplicate net info error");
			break;
		}
		ret = TINY_RET_OK;
	} while(0);
	json_value_free(je);
	return ret;
}

TinyRet Miot_Start(Miot* thiz)
{
	OtCloud_Start(thiz->cloud);
    	//Bootstrap_AddChannel(&thiz->bootstrap, thiz->cloud->cloudChannel);
    	Bootstrap_Sync(&thiz->bootstrap);
}

TinyRet Miot_Stop(Miot* thiz)
{
    OtCloud_Stop(thiz->cloud);
    Bootstrap_Shutdown(&thiz->bootstrap);
}


