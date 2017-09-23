#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <winsock2.h>
#include <windows.h>
#endif

#include <tiny_debug.h>
#include <BsonObject.h>

/**
 * BSON: {"hello": "world"} => BSON: \x16\x00\x00\x00\x02hello\x00\x06\x00\x00\x00world\x00\x00
 *
 * [\x16\x00\x00\x00]   = total length                      4
 * [\x02]               = type of value: string,            1
 * [hello\x00]          = key.                              2
 * [\x06\x00\x00\x00]   = length of value.                  4
 * [world\x00]          = value                             2
 * [\x00]               = end                               1
 *
 */
uint8_t binary[22] = {
        0x16, 0x00, 0x00, 0x00,
        0x02,
        'h', 'e', 'l', 'l', 'o', 0,
        0x06, 0x00, 0x00, 0x00,
        'w', 'o', 'r', 'l', 'd', 0,
        0
};

static int test1(void)
{
    TinyRet ret = TINY_RET_OK;

    printf("binary: 22\n");
    for (uint32_t i = 0; i < 22; ++i)
    {
        printf("0x%02x ", binary[i]);
    }
    printf("\n");

    BsonObject * object = BsonObject_New();
    RETURN_VAL_IF_FAIL(object != NULL, -1);

    ret = BsonObject_PutString(object, "hello", "world");
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    BsonObject_Encode(object);

    printf("BsonObject_Encode length: %d\n", object->size);

    for (uint32_t i = 0; i < object->size; ++i)
    {
        printf("0x%02x ", object->binary[i]);
    }
    printf("\n");

    ret = BsonObject_PutString(object, "iid", "1");
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = BsonObject_PutInt32(object, "int32", 12);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    ret = BsonObject_PutFloat(object, "float", 44.0);
    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);

    BsonObject_ToString(object);
    printf("String: \n%s\n", object->binary);

    BsonObject_Delete(object);

	return 1;
}

//static int test1(void)
//{
//    TinyRet ret = TINY_RET_OK;
//
//    printf("binary: 22\n");
//    for (uint32_t i = 0; i < 22; ++i)
//    {
//        printf("0x%02x ", binary[i]);
//    }
//    printf("\n");
//
//    BsonObject * object = BsonObject_New();
//    RETURN_VAL_IF_FAIL(object != NULL, -1);
//
//    ret = BsonObject_PutString(object, "hello", "world");
//    RETURN_VAL_IF_FAIL(ret == TINY_RET_OK, -1);
//
//    BsonObject_Encode(object);
//
//    printf("BsonObject_Encode length: %d\n", object->size);
//
//    for (uint32_t i = 0; i < object->size; ++i)
//    {
//        printf("0x%02x ", object->binary[i]);
//    }
//    printf("\n");
//
//    BsonObject_Delete(object);
//
//    return 1;
//}

int main(int argc, char *argv[])
{
	test1();

#ifdef _WIN32
    _CrtDumpMemoryLeaks();
#endif

	return 0;
}