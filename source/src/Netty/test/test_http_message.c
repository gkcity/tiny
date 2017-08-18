#include <stdio.h>
#include <codec-http/HttpMessage.h>
#include <tiny_log.h>

#define TAG			"test"

static int test1(void)
{
	HttpMessage response;

	if (RET_FAILED(HttpMessage_Construct(&response)))
	{
		return 0;
	}

	HttpMessage_SetResponse(&response, 204, "No Content");
	HttpMessage_SetVersion(&response, 1, 1);

	HttpHeader_Set(&response.header, "Content-Type", "text/json");
	HttpHeader_SetInteger(&response.header, "Content-Length", 0);

	HttpMessage_Dispose(&response);

	return 0;
}

static int test2(void)
{
	HttpMessage response;

	if (RET_FAILED(HttpMessage_Construct(&response)))
	{
		return 0;
	}

	HttpMessage_SetResponse(&response, 204, "No Content");
    HttpMessage_SetVersion(&response, 1, 1);

    HttpHeader_Set(&response.header, "Content-Type", "text/json");
    HttpHeader_SetInteger(&response.header, "Content-Length", 0);

	LOG_D(TAG, "BYTES: \n%s", HttpMessage_GetBytesWithoutContent(&response));
	LOG_D(TAG, "SIZE: %d", HttpMessage_GetBytesSizeWithoutContent(&response));

	HttpMessage_Dispose(&response);

	return 0;
}

int main(int argc, char *argv[])
{
	for (int i = 0; i < 10000; ++i) 
	{
		test1();
		test2();
	}

	return 0;
}
