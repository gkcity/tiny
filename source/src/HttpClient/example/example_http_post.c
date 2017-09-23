#include <tiny_log.h>
#include <tiny_socket.h>
#include <JsonObject.h>
#include "../HttpClient.h"

#define API_INITIALIZE      "/crypto/srp/server/instance/initialize"
#define API_VERIFY          "/crypto/srp/server/instance/verify"
#define SERVER_IP           "127.0.0.1"
#define SERVER_PORT         9000

int main(void)
{
    HttpClient * client = NULL;
    HttpExchange * exchange = NULL;
    JsonObject * object = NULL;

    tiny_socket_initialize();

    do
    {
        client = HttpClient_New();
        if (client == NULL)
        {
            printf("HttpClient_New failed!\n");
            break;
        }

        object = JsonObject_New();
        if (object == NULL)
        {
            printf("JsonObject_New failed!\n");
            break;
        }

        if (RET_FAILED(JsonObject_PutString(object, "deviceId", "aabb"))
            || RET_FAILED(JsonObject_PutString(object, "username", "hello"))
            || RET_FAILED(JsonObject_PutString(object, "password", "hi")))
        {
            printf("JsonObject_PutString failed!\n");
            break;
        }

        if (RET_FAILED(JsonObject_Encode(object, false)))
        {
            printf("JsonObject_Encode failed!");
            break;
        }

        exchange = HttpExchange_New(SERVER_IP, SERVER_PORT, "POST", API_INITIALIZE, 5, (uint8_t *)object->string, object->size);
        if (exchange == NULL)
        {
            printf("HttpExchange_New failed!\n");
            break;
        }

        if (RET_FAILED(HttpHeader_Set(&exchange->request, "Content-Type", "application/json")))
        {
            printf("HttpHeader_Set failed!\n");
            break;
        }

        if (RET_FAILED(HttpClient_Send(client, exchange)))
        {
            printf("HttpClient_Send failed!\n");
            break;
        }

        printf("%d\n", exchange->status);

        if (exchange->status != HTTP_STATUS_OK)
        {
            break;
        }

        printf("%s\n", exchange->content);
    } while (false);

    if (object != NULL)
    {
        JsonObject_Delete(object);
    }

    if (exchange != NULL)
    {
        HttpExchange_Delete(exchange);
    }

    if (client != NULL)
    {
        HttpClient_Delete(client);
    }

    tiny_socket_initialize();

    return 0;
}