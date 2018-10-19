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
#include <value/JsonString.h>
#include <value/JsonNumber.h>
#include <value/JsonBoolean.h>

#define TAG			"test"

#define JSON_SAMPLE_0 \
"{}"

#define JSON_SAMPLE_1 \
"{\"name\":null}"

#define JSON_SAMPLE_2 \
"{\"online\":true,\"offline\":false}"

#define JSON_SAMPLE_3 \
"{\"name\":\"tom\",\"age\":1234,\"pi\":3.1415926}"

#define JSON_SAMPLE_4 \
"{\"type\":\"urn:xiot-spec:device:fan:00000A04:zhimi\",\"description\":\"Zhimi Fan\"}"

#define JSON_SAMPLE_5 \
"{\"a1\": {\"a2\": {\"a3\": {\"a4\": \"hello\"}}}}"

#define JSON_SAMPLE_6 \
"{\"type\":\"urn:xiot-spec:device:fan:00000A04:zhimi\",\"description\":\"Zhimi Fan\",\"services\":[{\"iid\":1,\"type\":\"urn:xiot-spec:service:device-information:00000800\",\"description\":\"Device Information\",\"properties\":[{\"iid\":1,\"type\":\"urn:xiot-spec:property:manufacturer:00000024\",\"description\":\"Device Manufacturer\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":2,\"type\":\"urn:xiot-spec:property:model:00000025\",\"description\":\"Device Model\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":3,\"type\":\"urn:xiot-spec:property:serial-number:00000026\",\"description\":\"Device Serial Number\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":4,\"type\":\"urn:xiot-spec:property:name:00000001\",\"description\":\"Device Name\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":5,\"type\":\"urn:xiot-spec:property:firmware-revision:0000002D\",\"description\":\"Current Firmware Version\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":6,\"type\":\"urn:xiot-spec:property:firmware-revision:0000002D\",\"description\":\"Latest Firmware Version\",\"format\":\"string\",\"access\":[\"read\"]}]},{\"iid\":2,\"type\":\"urn:xiot-spec:service:fan:00000802\",\"description\":\"Fan\",\"properties\":[{\"iid\":1,\"type\":\"urn:xiot-spec:property:on:00000002\",\"description\":\"Switch Status\",\"format\":\"bool\",\"access\":[\"read\",\"write\",\"notify\"]},{\"iid\":2,\"type\":\"urn:xiot-spec:property:speed-level:00000003\",\"description\":\"Speed Level\",\"format\":\"uint8\",\"access\":[\"read\",\"write\",\"notify\"],\"value-range\":[1,5,1]},{\"iid\":3,\"type\":\"urn:xiot-spec:property:swing:00000005\",\"description\":\"Swing Mode Status\",\"format\":\"bool\",\"access\":[\"read\",\"write\",\"notify\"]},{\"iid\":4,\"type\":\"urn:xiot-spec:property:swing-angle:00000006\",\"description\":\"Swing Angle\",\"format\":\"int32\",\"access\":[\"read\",\"write\",\"notify\"],\"value-range\":[30,120,1]},{\"iid\":5,\"type\":\"urn:xiot-spec:property:physical-controls-locked:00000004\",\"description\":\"Physical Controls Lock Mode Status\",\"format\":\"bool\",\"access\":[\"read\",\"write\",\"notify\"]}]}]}"

#define JSON_SAMPLE_7 \
"{\"name\":\"\"}"

const char * samples[8] =
        {
                JSON_SAMPLE_0,
                JSON_SAMPLE_1,
                JSON_SAMPLE_2,
                JSON_SAMPLE_3,
                JSON_SAMPLE_4,
                JSON_SAMPLE_5,
                JSON_SAMPLE_6,
                JSON_SAMPLE_7,
        };

static int test1(void)
{
    for (int i = 0; i < 8; ++i)
    {
        printf("JSON [%d]: %s\n", i, samples[i]);
        JsonObject *object = JsonObject_NewString(samples[i]);
        if (object == NULL)
        {
            printf("JsonObject_NewString FAILED!\n");
            break;
        }

        JsonObject_Encode(object, true);
        printf("------------ JSON ENCODE START ----------------------\n");
        printf("%s\n", object->string);
        printf("------------ JSON ENCODE END ------------------------\n");

        JsonObject_Delete(object);
    }

	return 1;
}

#define JSON_SAMPLE_E0 \
"{"

#define JSON_SAMPLE_E1 \
"{\"name\"null}"

#define JSON_SAMPLE_E2 \
"{\"online:true,\"offline\":false}"

#define JSON_SAMPLE_E3 \
"{\"name\":\"tom\"\"age\":1234,\"pi\":3.1415926}"

#define JSON_SAMPLE_E4 \
"{\"type\"\"urn:xiot-spec:device:fan:00000A04:zhimi\",\"description\":\"Zhimi Fan\"}"

#define JSON_SAMPLE_E5 \
"{\"a1\": {\"a2\": {\"a3\": {\"a4\": \"hello\"}}}"

#define JSON_SAMPLE_E6 \
"{\"type\":\"urn:xiot-spec:device:fan:00000A04:zhimi\",\"description\":\"Zhimi Fan\",\"services\":[{\"iid\":1,\"type\":\"urn:xiot-spec:service:device-information:00000800\",\"description\":\"Device Information\",\"properties\":[{\"iid\":1,\"type\":\"urn:xiot-spec:property:manufacturer:00000024\",\"description\":\"Device Manufacturer\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":2,\"type\":\"urn:xiot-spec:property:model:00000025\",\"description\":\"Device Model\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":3,\"type\":\"urn:xiot-spec:property:serial-number:00000026\",\"description\":\"Device Serial Number\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":4,\"type\":\"urn:xiot-spec:property:name:00000001\",\"description\":\"Device Name\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":5,\"type\":\"urn:xiot-spec:property:firmware-revision:0000002D\",\"description\":\"Current Firmware Version\",\"format\":\"string\",\"access\":[\"read\"]},{\"iid\":6,\"type\":\"urn:xiot-spec:property:firmware-revision:0000002D\",\"description\":\"Latest Firmware Version\",\"format\":\"string\",\"access\":[\"read\"]}]},{\"iid\":2,\"type\":\"urn:xiot-spec:service:fan:00000802\",\"description\":\"Fan\",\"properties\":[{\"iid\":1,\"type\":\"urn:xiot-spec:property:on:00000002\",\"description\":\"Switch Status\",\"format\":\"bool\",\"access\":[\"read\",\"write\",\"notify\"]},{\"iid\":2,\"type\":\"urn:xiot-spec:property:speed-level:00000003\",\"description\":\"Speed Level\",\"format\":\"uint8\",\"access\":[\"read\",\"write\",\"notify\"],\"value-range\":[1,5,1]},{\"iid\":3,\"type\":\"urn:xiot-spec:property:swing:00000005\",\"description\":\"Swing Mode Status\",\"format\":\"bool\",\"access\":[\"read\",\"write\",\"notify\"]},{\"iid\":4,\"type\":\"urn:xiot-spec:property:swing-angle:00000006\",\"description\":\"Swing Angle\",\"format\":\"int32\",\"access\":[\"read\",\"write\",\"notify\"],\"value-range\":[30,120,1]},{\"iid\":5,\"type\":\"urn:xiot-spec:property:physical-controls-locked:00000004\",\"description\":\"Physical Controls Lock Mode Status\",\"format\":\"bool\",\"access\":\"read\",\"write\",\"notify\"]}]}]}"

const char * error_samples[7] =
        {
                JSON_SAMPLE_E0,
                JSON_SAMPLE_E1,
                JSON_SAMPLE_E2,
                JSON_SAMPLE_E3,
                JSON_SAMPLE_E4,
                JSON_SAMPLE_E5,
                JSON_SAMPLE_E6,
        };

static int test2(void)
{
    for (int i = 0; i < 7; ++i)
    {
        printf("[%d]: %s\n", i, error_samples[i]);
        JsonObject *object = JsonObject_NewString(error_samples[i]);
        if (object != NULL)
        {
            if (RET_SUCCEEDED(JsonObject_Encode(object, true)))
            {
                printf("------------ JSON ENCODE START ----------------------\n");
                printf("%s\n", object->string);
                printf("------------ JSON ENCODE END ------------------------\n");
            }

            JsonObject_Delete(object);
        }
    }

    return 1;
}

#define JSON_SAMPLE_003 \
"{\"characteristics\":[{\"aid\":1,\"iid\":9,\"ev\":true}]}"

static int test3(void)
{
    printf("json: %s\n", JSON_SAMPLE_003);
    JsonObject *object = JsonObject_NewString(JSON_SAMPLE_003);
    if (object != NULL)
    {
        if (RET_SUCCEEDED(JsonObject_Encode(object, true)))
        {
            printf("------------ JSON ENCODE START ----------------------\n");
            printf("%s\n", object->string);
            printf("------------ JSON ENCODE END ------------------------\n");
        }

        JsonValue *characteristics = JsonObject_GetValue(object, "characteristics");
        if (characteristics == NULL) 
        {
            printf("JsonObject_Get: %s FAILED\n", "characteristics");
            return 1;
        }

        if (characteristics->type != JSON_ARRAY) 
        {
            printf("characteristics type invalid: %d\n", characteristics->type);
            return 1;
        }

        if (characteristics->data.array->values.size != 1) 
        {
            printf("characteristics values size: %d\n", characteristics->data.array->values.size);
            return 1;
        }

        JsonObject *property = ((JsonValue *)TinyList_GetAt(&characteristics->data.array->values, 0))->data.object;
        JsonValue *jaid = JsonObject_GetValue(property, "aid");
        JsonValue *jiid = JsonObject_GetValue(property, "iid");
        JsonBoolean *ev = JsonObject_GetBoolean(property, "ev");

        printf("aid: %ld\n", jaid->data.number->value.intValue);
        printf("iid: %ld\n", jiid->data.number->value.intValue);
        printf("ev: %s\n", ev->value ? "true" : "false");

        JsonObject_Delete(object);
    }

    return 0;
}

#define JSON_SAMPLE_004 \
"{\"name\":\"ouyang\"}"

static int test4(void)
{
    printf("json: %s\n", JSON_SAMPLE_004);
    JsonObject *object = JsonObject_NewString(JSON_SAMPLE_004);
    if (object != NULL)
    {
        if (RET_SUCCEEDED(JsonObject_Encode(object, true)))
        {
            printf("------------ JSON ENCODE START ----------------------\n");
            printf("%s\n", object->string);
            printf("------------ JSON ENCODE END ------------------------\n");
        }

        JsonValue *value = JsonObject_GetValue(object, "name");
        if (value == NULL) 
        {
            printf("JsonObject_GetValue failed: %s\n", "name");
            return 1;
        }

        if (value->type != JSON_STRING) 
        {
            printf("JsonValue.type is not a string\n");
            return 1;
        }

        printf("name: %s\n", value->data.string->value);

        JsonObject_Delete(object);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    test1();
    test2();
    test3();
    test4();

#ifdef _WIN32
    _CrtDumpMemoryLeaks();
#endif

	return 0;
}