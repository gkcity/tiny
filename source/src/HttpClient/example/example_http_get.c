#include <tiny_log.h>
#include "../HttpClient.h"

#define URI "/homekit/instance/device?type=urn:homtkit-spec:device:lightbulb:00000000:arrizo-v1"

int main(void)
{
    do
    {
        HttpClient *client = HttpClient_New();

        HttpExchange *exchange = HttpExchange_New("47.93.60.147", 8080, "GET", URI, 5);

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

    return 0;
}