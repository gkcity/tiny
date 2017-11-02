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
#include <codec/JsonEncoder.h>

#define TAG			"test"

static int test1(void)
{
    TinyRet ret = TINY_RET_OK;

    JsonObject * object = JsonObject_New();
    RETURN_VAL_IF_FAIL(object != NULL, -1);

    ret = JsonObject_PutInteger(object, "a", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(object, "b", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(object, "b", 10);
    RETURN_VAL_IF_FAIL(ret != TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(object, "c", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(object, "d", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    JsonObject_Encode(object, true);
    printf("json encode ->\n%s\n", object->string);

    JsonObject_Delete(object);

	return 1;
}

static int test2(void)
{
    TinyRet ret = TINY_RET_OK;

    JsonObject * object = JsonObject_New();
    RETURN_VAL_IF_FAIL(object != NULL, -1);

    ret = JsonObject_PutInteger(object, "age", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutFloat(object, "temp", 10.0);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutString(object, "name", "hello");
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(object, "c", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(object, "d", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    JsonObject_Encode(object, true);
    printf("json encode ->\n%s\n", object->string);

    JsonObject_Delete(object);

    return 1;
}

static int test3(void)
{
    TinyRet ret = TINY_RET_OK;

    JsonObject * root = JsonObject_New();
    RETURN_VAL_IF_FAIL(root != NULL, -1);

    ret = JsonObject_PutInteger(root, "age", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutFloat(root, "temp", 10.0);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutString(root, "name", "hello");
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(root, "c", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutBoolean(root, "online", false);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    JsonObject *home = JsonObject_New();

    ret = JsonObject_PutInteger(home, "age", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutFloat(home, "temp", 10.0);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutString(home, "name", "ouyangchengfeng");
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutBoolean(home, "married", true);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutObject(root, "myhome", home);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    JsonObject_Encode(root, true);
    printf("json encode ->\n%s\n", root->string);

    JsonObject_Encode(root, false);
    printf("json encode ->\n%s\n", root->string);

    JsonObject_Delete(root);

    return 1;
}

static int test4(void)
{
    TinyRet ret = TINY_RET_OK;

    JsonObject * root = JsonObject_New();
    RETURN_VAL_IF_FAIL(root != NULL, -1);

    ret = JsonObject_PutInteger(root, "age", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutFloat(root, "temp", 10.0);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutString(root, "name", "hello");
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(root, "c", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutBoolean(root, "online", false);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    JsonArray *array = JsonArray_New(JSON_STRING);
    RETURN_VAL_IF_FAIL(array != NULL, -1);

    JsonArray_AddString(array, "Monday");
    JsonArray_AddString(array, "Tuesday");
    JsonArray_AddString(array, "Wednesday");
    JsonArray_AddString(array, "Thursday");
    JsonArray_AddString(array, "Friday");
    JsonArray_AddString(array, "Saturday");
    JsonArray_AddString(array, "Sunday");

    ret = JsonObject_PutArray(root, "week", array);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    JsonObject_Encode(root, true);
    printf("json encode ->\n%s\n", root->string);

    JsonObject_Encode(root, false);
    printf("json encode ->\n%s\n", root->string);

    JsonObject_Delete(root);

    return 1;
}

static int test5(void)
{
    TinyRet ret = TINY_RET_OK;

    JsonObject *root = JsonObject_New();
    RETURN_VAL_IF_FAIL(root != NULL, -1);

    JsonObject_PutString(root, "s", "f0a8f3c5c449dd6d62e573f062b4cc99");
    JsonObject_PutString(root, "B", "da1f63ef0d5f77272d547d3545b04ea2a7a029410a410c041a9f0d593dda22692d72bb1d1b03edfb43f94ea4aced8a99fe2ee2620712a981ce758260c5769f2966385524d50f20a78ee69710298c0b512a0a85d9718c9fcaa4212ce979eb2e6487b1ecaae0804e29d6cbd84f06be903af7cb25261c315a109e425bec69712caedfa34d04628f83fbc21c5cf0899dd9355967d51e70f9bb3ce6414f2b9912bc7a8a63700dd6ec700c0548430af1c910d4da3bc37b3a3861da8afaa9818bbc4e3aac7cf0fa355da0e6f93aa19ff61daa93d717ce675eb143d4d8abe68be1c43719d1364a1d55f25a7c9873aa0bfeb41f82a80f5472b1cbaccb814b0e1b6eb6cf6577620def6f5d6f07575993b4749c81e7e35b5af1b84b59bd6020650d531218803bce4825e7a3a106f6abbc50284c109ae5921e655e527f17d9b881171969be1ec8b915d5c08faaff72e9495ebdfc9f55940e8bb90da364f1d439869cb066bca67f3b2c5534554d38e8fc34a53660e94cd96fc5de4864f3f3d14a08af081bd6fd");

    JsonObject_Encode(root, true);
    printf("json encode ->\n%s\n", root->string);

    JsonObject_Delete(root);

    return 0;
}

static void _Output (const char *string, void *ctx)
{
    printf("%s", string);
//    printf("test: [%s]\n", string);
}

static int test6(void)
{
    TinyRet ret = TINY_RET_OK;

    JsonObject * root = JsonObject_New();
    RETURN_VAL_IF_FAIL(root != NULL, -1);

    ret = JsonObject_PutInteger(root, "age", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutFloat(root, "temp", 10.0);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutString(root, "name", "hello");
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutInteger(root, "c", 10);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = JsonObject_PutBoolean(root, "online", false);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    JsonArray *array = JsonArray_New(JSON_STRING);
    RETURN_VAL_IF_FAIL(array != NULL, -1);

    JsonArray_AddString(array, "Monday");
    JsonArray_AddString(array, "Tuesday");
    JsonArray_AddString(array, "Wednesday");
    JsonArray_AddString(array, "Thursday");
    JsonArray_AddString(array, "Friday");
    JsonArray_AddString(array, "Saturday");
    JsonArray_AddString(array, "Sunday");

    ret = JsonObject_PutArray(root, "week", array);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    JsonObject_Encode(root, true);
    printf("json encode (%d) ->\n%s\n", root->size, root->string);

    JsonObject_Encode(root, false);
    printf("json encode (%d) ->\n%s\n", root->size, root->string);

    TinyBuffer *buffer = TinyBuffer_New(20);
    RETURN_VAL_IF_FAIL(buffer != NULL, -1);

    JsonEncoder encoder;
    ret = JsonEncoder_Construct(&encoder, root, true);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    printf("\nencode petty (%d):\n", encoder.size);
    JsonEncoder_EncodeObject(&encoder, buffer, _Output, NULL);

    JsonObject_Delete(root);

    return 1;
}
int main(int argc, char *argv[])
{
//	test1();
//    test2();
//    test3();
//    test4();
//    test5();
    test6();

#ifdef _WIN32
    _CrtDumpMemoryLeaks();
#endif

	return 0;
}

void tiny_print_mem(const char *tag, const char *function)
{
}

void tiny_sleep(int ms)
{
    printf("tiny_sleep: %d\n", ms);
}
