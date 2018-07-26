#include <tiny_log.h>
#include <tiny_socket.h>
#include "../TcpClient.h"

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
        TcpClient *client = TcpClient_New();
        TcpExchange *exchange = TcpExchange_New("39.106.171.204", 80, "GET", FAN, 5, NULL, 0);

        if (RET_SUCCEEDED(TcpClient_Send(client, exchange)))
        {
            printf("HTTP/1.1 %d\n\n", exchange->status);

            if (exchange->status == HTTP_STATUS_OK)
            {
                printf("%s\n", exchange->content);
            }
        }

        TcpExchange_Delete(exchange);
        TcpClient_Delete(client);
    } while (false);

    tiny_socket_initialize();

    return 0;
}