#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <winsock2.h>
#include <windows.h>
#endif

#include <tiny_debug.h>
#include <BsonObject.h>
#include <BsonArray.h>

#define TAG			"test"

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
    BsonObject *object = BsonObject_NewBinary(binary, 22);
    if (object == NULL)
    {
        printf("BsonObject_NewBinary FAILED!\n");
        return 1;
    }

    //        BsonObject_Encode(object, true);
    //        printf("------------ BSON ENCODE START ----------------------\n");
    //        printf("%s\n", object->string);
    //        printf("------------ BSON ENCODE END ------------------------\n");
    //
    BsonObject_Delete(object);

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