#include <stdio.h>
#include <codec-http/HttpMessage.h>
#include <tiny_log.h>
#include <codec-http/HttpMessageEncoder.h>

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

static void _Output (const uint8_t *data, uint32_t size, void *ctx)
{
	printf("%s", (const char *)data);
}

static int test2(void)
{
	HttpMessage response;
	HttpMessageEncoder encoder;

	if (RET_FAILED(HttpMessage_Construct(&response)))
	{
		return 0;
	}

	HttpMessage_SetResponse(&response, 204, "No Content");
    HttpMessage_SetVersion(&response, 1, 1);

    HttpHeader_Set(&response.header, "Content-Type", "text/json");
    HttpHeader_SetInteger(&response.header, "Content-Length", 0);

	HttpMessageEncoder_Construct(&encoder, &response);

	printf("size: %d\n", encoder.size);

	HttpMessageEncoder_Encode(&encoder, NULL, _Output, NULL);

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
