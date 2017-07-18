#include <stdio.h>
#include <codec-http/HttpMessage.h>
#include <tiny_log.h>

#include "miot.h"

#define TAG			"test"

bool run = true;

static void sighandler(int sig)
{
	run = false;
}

int main()
{
	//signal(SIGINT, sighandler);

	Miot* miot = Miot_New();
	Miot_SetDInfo(miot, 9994, "nuwa.robot.kiwi1", "nuwa", "yIJE4AWbuzB5adxG");
	Miot_SetDToken(miot, NULL);
	Miot_SetNetInfo(miot, "{\"partner_id\":\"\",\"method\":\"_internal.info\",\"params\":{\"uid\":\"0\",\"netif\":{\"mask\":\"255.255.255.0\",\"gw\":\"192.168.31.1\",\"localIp\":\"192.168.31.214\"},\"hw_ver\":\"Linux\",\"fw_ver\":\"unknown\",\"ap\":{\"bssid\":\"28:6c:07:34:6c:48\",\"ssid\":\"milink\"}}}");
	Miot_Start(miot);
	Miot_Stop(miot);
	Miot_Delete(miot);
/*
    Channel *cloudChannel = NULL;
    Channel *server2 = NULL;
    Bootstrap sb;

    my_socket_init();

    // new TCP Server
    cloudChannel = StreamClientChannel_New();
    StreamClientChannel_Initialize(thiz, OtCloudInitializer, NULL);
    
    //SocketChannel_Initialize(cloudChannel, ,int fd,uint16_t port,ChannelInitializer initializer,void * ctx)

    // new TCP Server
    server2 = StreamServerChannel_New(2);
    StreamServerChannel_Initialize(server2, HttpServerInitializer, NULL);
    StreamServerChannel_Bind(server2, 9092);

    // Starting Bootstrap
    Bootstrap_Construct(&sb);
    Bootstrap_AddChannel(&sb, server1);
    Bootstrap_AddChannel(&sb, server2);
    Bootstrap_Sync(&sb);
    Bootstrap_Shutdown(&sb);
    Bootstrap_Dispose(&sb);
*/
    return 0;
}

