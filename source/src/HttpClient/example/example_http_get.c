#include <tiny_log.h>
#include <tiny_socket.h>
#include "../HttpClient.h"

#define URI "/homekit/instance/device?type=urn:homtkit-spec:device:lightbulb:00000000:arrizo-v1"

/**
 * NO ACTION
 */
#define ZHIMI_FAN "/instance/device?type=urn:miot-spec:device:fan:00000A04:zhimi"

/**
 * Contains Action without arguments
 */
#define ROCK_SWEEPER "/instance/device?type=urn:miot-spec:device:sweeper:00000A0A:rock"

/**
 * Contains Action with arguments
 */
#define CHUNMI_COOKER "/instance/device?type=urn:miot-spec:device:cooker:00000A08:chunmi"

int main(void)
{
    tiny_socket_initialize();

    do
    {
        HttpClient *client = HttpClient_New();

        HttpExchange *exchange = HttpExchange_New("47.93.60.147", 8080, "GET", CHUNMI_COOKER, 5);

        if (RET_SUCCEEDED(HttpClient_Send(client, exchange)))
        {
            printf("%d\n", exchange->status);

            if (exchange->status == HTTP_STATUS_OK)
            {
                printf("%s\n", exchange->content);
            }
        }

        HttpExchange_Delete(exchange);
        HttpClient_Delete(client);
    } while (false);

    tiny_socket_initialize();

    return 0;
}