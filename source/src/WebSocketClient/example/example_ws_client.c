#include <tiny_log.h>
#include <tiny_socket.h>
#include "../WsClient.h"

#define FAN    "/dd/instance/urn:homekit-spec:device:fan:00000000:geekcity-ds:1"

/**
 * NO ACTION
 */
#define ZHIMI_FAN "/dd/instance/urn:miot-spec:device:fan:00000A04:zhimi"

/**
 * Contains Action without arguments
 */
#define ROCK_SWEEPER "/dd/instance/urn:miot-spec:device:sweeper:00000A0A:rock"

/**
 * Contains Action with arguments
 */
#define CHUNMI_COOKER "/instance/urn:miot-spec:device:cooker:00000A08:chunmi"

void tiny_print_mem(const char *tag, const char *function)
{
}

void tiny_sleep(int ms)
{
    printf("tiny_sleep: %d\n", ms);
}

int main(void)
{
    tiny_socket_initialize();

    do
    {
        WsClient *client = WsClient_New();

        HttpExchange *exchange = HttpExchange_New("39.106.171.204", 80, "GET", FAN, 5, NULL, 0);

        if (RET_SUCCEEDED(WsClient_Send(client, exchange)))
        {
            printf("HTTP/1.1 %d\n\n", exchange->status);

            if (exchange->status == HTTP_STATUS_OK)
            {
                printf("%s\n", exchange->content);
            }
        }

        HttpExchange_Delete(exchange);
        WsClient_Delete(client);
    } while (false);

    tiny_socket_initialize();

    return 0;
}