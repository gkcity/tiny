#include <tiny_log.h>
#include <channel/stream/StreamServerChannel.h>
#include <channel/SocketChannel.h>
#include <channel/ChannelIdleStateHandler.h>
#include <channel/multicast/MulticastChannel.h>
#include <bootstrap/Bootstrap.h>
#include <codec-http/HttpMessageCodec.h>
#include "ExampleSsdpHandler.h"

#define TAG "SsdpExample"

static void SsdpInitializer(Channel *channel, void *ctx)
{
    LOG_D(TAG, "SsdpInitializer: %s", channel->id);
//    SocketChannel_AddLast(channel, ChannelIdleStateHandler(0, 0, 3));
    SocketChannel_AddLast(channel, ExampleSsdpHandler());
    SocketChannel_AddBefore(channel, ExampleSsdpHandler_Name, HttpMessageCodec());
}

//static void timeout(Timer *timer, void *ctx)
//{
//    Channel *ssdp = (Channel *)ctx;
//    const char *data = "hello, world";
//    size_t len = strlen(data);
//    LOG_I(TAG, "timeout");
//
//    MulticastChannel_Write(ssdp, data, (uint32_t)len);
//}

int main()
{
    Channel *ssdp = NULL;
//    Timer *timer = NULL;
    Bootstrap sb;

    tiny_socket_initialize();

    // SSDP
    ssdp = MulticastChannel_New();
    MulticastChannel_Initialize(ssdp, SsdpInitializer, NULL);

    if (RET_FAILED(MulticastChannel_Join(ssdp, "10.0.1.9", "239.255.255.250", 1900, false)))
    {
        LOG_D(TAG, "MulticastChannel_Join failed");
        return 0;
    }

    // new Timer
//    timer = Timer_New("mytimer", 3 * 1000000, ssdp, timeout);

    // Bootstrap
    if (RET_FAILED(Bootstrap_Construct(&sb)))
    {
        LOG_D(TAG, "Bootstrap_Construct failed");
        return 0;
    }

    if (RET_FAILED(Bootstrap_AddChannel(&sb, ssdp)))
    {
        LOG_D(TAG, "Bootstrap_AddChannel failed");
        return 0;
    }

//    Bootstrap_AddTimer(&sb, timer);

    if (RET_FAILED(Bootstrap_Sync(&sb)))
    {
        LOG_D(TAG, "Bootstrap_Sync failed");
        return 0;
    }

    if (RET_FAILED(Bootstrap_Shutdown(&sb)))
    {
        LOG_D(TAG, "Bootstrap_Shutdown failed");
        return 0;
    }

    Bootstrap_Dispose(&sb);

    tiny_socket_finalize();

    return 0;
}