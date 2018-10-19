#include <tiny_log.h>
#include <tiny_socket.h>
#include "../TcpClient.h"

static void _printHex(const uint8_t *data, uint32_t length)
{
    for (uint32_t  i = 0; i < length; ++i)
    {
        printf("%02x ", data[i]);
    }
    printf("\n");
}

int main(void)
{
    tiny_socket_initialize();

    do
    {
//        uint32_t length = 8;
//        uint8_t data[8] = {0x01, 0x4, 0x00, 0x02, 0x00, 0x02, 0xD0, 0x0B};
//        _printHex(data, length);

        uint32_t length = 12;
        uint8_t data[12] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0x04, 0x00, 0x00, 0x00, 0x02};
        _printHex(data, length);

        TcpClient *client = TcpClient_New();
        TcpExchange *exchange = TcpExchange_New("10.0.1.47", 502, 10, data, length);

        if (RET_SUCCEEDED(TcpClient_Send(client, exchange)))
        {
            _printHex((const uint8_t *)(exchange->content), exchange->length);
        }

        TcpExchange_Delete(exchange);
        TcpClient_Delete(client);
    } while (false);

    tiny_socket_initialize();

    return 0;
}