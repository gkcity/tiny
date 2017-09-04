#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <winsock2.h>
#include <windows.h>
#endif

#include <tiny_debug.h>
#include <JsonObject.h>
#include <JsonArray.h>
#include <JsonTokenizer.h>
#include <JsonToken.h>

#define TAG			"test"

#define JSON_SAMPLE_0 \
"{}"

#define JSON_SAMPLE_1 \
"{\"name\":null}"

#define JSON_SAMPLE_2 \
"{\"online\":true,\"offline\":false}"

#define JSON_SAMPLE_3 \
"{\"name\":\"tom\"}"

#define JSON_SAMPLE_4 \
"{\"type\":\"urn:miot-spec:device:fan:00000A04:zhimi\",\"description\":\"Zhimi Fan\"}"

#define JSON_SAMPLE_5 \
"{\"type\":\"urn:miot-spec:device:fan:00000A04:zhimi\",\"description\":\"Zhimi Fan\",\"services\":[{\"iid\":1,\"type\":\"urn:miot-spec:service:device-information:00000800\",\"description\":\"Device Information\",\"properties\":[{\"iid\":1,\"type\":\"urn:miot-spec:property:manufacturer:00000024\",\"description\":\"Device Manufacturer\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":2,\"type\":\"urn:miot-spec:property:model:00000025\",\"description\":\"Device Model\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":3,\"type\":\"urn:miot-spec:property:serial-number:00000026\",\"description\":\"Device Serial Number\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":4,\"type\":\"urn:miot-spec:property:name:00000001\",\"description\":\"Device Name\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":5,\"type\":\"urn:miot-spec:property:firmware-revision:0000002D\",\"description\":\"Current Firmware Version\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":6,\"type\":\"urn:miot-spec:property:firmware-revision:0000002D\",\"description\":\"Latest Firmware Version\",\"format\":\"string\",\"access\":[\"read\"]}]},{\"iid\":2,\"type\":\"urn:miot-spec:service:fan:00000802\",\"description\":\"Fan\",\"properties\":[{\"iid\":1,\"type\":\"urn:miot-spec:property:on:00000002\",\"description\":\"Switch Status\",\"format\":\"bool\",\"access\":[\"read\",\"write\",\"notify\"]},{\"iid\":2,\"type\":\"urn:miot-spec:property:speed-level:00000003\",\"description\":\"Speed Level\",\"format\":\"uint8\",\"access\":[\"read\",\"write\",\"notify\"],\"value-range\":[1,5,1]},{\"iid\":3,\"type\":\"urn:miot-spec:property:swing:00000005\",\"description\":\"Swing Mode Status\",\"format\":\"bool\",\"access\":[\"read\",\"write\",\"notify\"]},{\"iid\":4,\"type\":\"urn:miot-spec:property:swing-angle:00000006\",\"description\":\"Swing Angle\",\"format\":\"int32\",\"access\":[\"read\",\"write\",\"notify\"],\"value-range\":[30,120,1]},{\"iid\":5,\"type\":\"urn:miot-spec:property:physical-controls-locked:00000004\",\"description\":\"Physical Controls Lock Mode Status\",\"format\":\"bool\",\"access\":[\"read\",\"write\",\"notify\"]}]}]}"

const char * samples[6] =
        {
                JSON_SAMPLE_0,
                JSON_SAMPLE_1,
                JSON_SAMPLE_2,
                JSON_SAMPLE_3,
                JSON_SAMPLE_4,
                JSON_SAMPLE_5,
        };

static int test1(void)
{
    for (int i = 0; i < 6; ++i)
    {
        JsonTokenizer tokenizer;

        if (RET_FAILED(JsonTokenizer_Construct(&tokenizer)))
        {
            return 1;
        }

        if (RET_FAILED(JsonTokenizer_Parse(&tokenizer, samples[i])))
        {
            JsonTokenizer_Print(&tokenizer);
            JsonTokenizer_Dispose(&tokenizer);
            return 1;
        }

        JsonTokenizer_Print(&tokenizer);
        printf("\n");

        JsonTokenizer_Dispose(&tokenizer);
    }

	return 0;
}

int main(int argc, char *argv[])
{
	test1();

#ifdef _WIN32
    _CrtDumpMemoryLeaks();
#endif

	return 0;
}