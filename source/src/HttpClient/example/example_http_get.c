#include <tiny_log.h>
#include <tiny_socket.h>
#include "../HttpClient.h"

#define FAN    "/dd/instance/urn:homekit-spec:device:fan:00000000:geekcity-ds:1"

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
        HttpClient *client = HttpClient_New();

        HttpExchange *exchange = HttpExchange_New("39.106.171.204", 80, "GET", FAN, 5, NULL, 0);

        if (RET_SUCCEEDED(HttpClient_Send(client, exchange)))
        {
            printf("HTTP/1.1 %d\n\n", exchange->status);

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