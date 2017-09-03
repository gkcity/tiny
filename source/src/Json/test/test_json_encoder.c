#include <tiny_debug.h>
#include <JsonObject.h>
#include <JsonArray.h>

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

    ret = JsonObject_PutString(home, "name", "hello");
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

int main(int argc, char *argv[])
{
	test1();
    test2();
    test3();
    test4();

	return 0;
}