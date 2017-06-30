#include "ScUuid.h"
#include "sc_log.h"

static void test_uuid(const char * str, bool valid)
{
    ScUuid *uuid = ScUuid_New();
    bool uuid_is_valid = false;

    if (ScUuid_ParseFromBuffer(uuid, str, strlen(str)) == SC_RET_OK)
    {
        uuid_is_valid = true;
    }

    LOG_I("test", 
        "%s is %s -> %s",
        str,
        uuid_is_valid ? "valid" : "invalid",
        uuid_is_valid == valid ? "OK" : "ERROR");

    ScUuid_Delete(uuid);
}

int main()
{
    ScUuid * uuid1 = NULL;
    ScUuid * uuid2 = NULL;
    ScUuid * uuid3 = NULL;
    //char p1[SC_UUID_LEN] = { 0 };
    //char p2[SC_UUID_LEN] = { 0 };
    //char p3[SC_UUID_LEN] = { 0 };

    LOG_OPEN(NULL);

    uuid1 = ScUuid_New();
    ScUuid_GenerateRandom(uuid1);
    LOG_I("test", "UUID 1: %s", ScUuid_ToString(uuid1, true));

    uuid2 = ScUuid_New();
    ScUuid_GenerateRandom(uuid2);
    LOG_I("test", "UUID 2: %s", ScUuid_ToString(uuid2, true));

    LOG_I("test", "UUID 2: %s", ScUuid_ToString(uuid2, false));

    LOG_I("test", "UUID1 and UUID2 is %s", ScUuid_Equal(uuid1, uuid2) ? "equal" : "not equal");

    ScUuid_Clear(uuid1);
    LOG_I("test", "UUID1 clear, UUID1 : %s", ScUuid_IsNull(uuid1) ? "is NULL" : "is Not NULL");

    LOG_I("test", "UUID1 and UUID2 is %s", ScUuid_Equal(uuid1, uuid2) ? "equal" : "not equal");

    uuid3 = ScUuid_New();
    ScUuid_Copy(uuid3, uuid2);

    LOG_I("test", "clone from UUID2, UUID3: %s", ScUuid_ToString(uuid3, true));

    LOG_I("test", "UUID2 and UUID3 is %s", ScUuid_Equal(uuid2, uuid3) ? "equal" : "not equal");
    
    test_uuid("4A5BFD77-D452-4492-B973-69198200339", false);
	test_uuid("84949cc5-4701-4a84-895b-354c584a981b", true);
	test_uuid("84949SC5-4701-4A84-895B-354C584A981B", true);
    test_uuid("84949cc5-4701-4a84-895b-354c584a981bc", false);
    test_uuid("84949cc5-4701-4a84-895b-354c584a981", false);
    test_uuid("84949cc5x4701-4a84-895b-354c584a981b", false);
    test_uuid("84949cc504701-4a84-895b-354c584a981b", false);
    test_uuid("84949cc5-470104a84-895b-354c584a981b", false);
    test_uuid("84949cc5-4701-4a840895b-354c584a981b", false);
    test_uuid("84949cc5-4701-4a84-895b0354c584a981b", false);
    test_uuid("g4949cc5-4701-4a84-895b-354c584a981b", false);
    test_uuid("84949cc5-4701-4a84-895b-354c584a981g", false);

    ScUuid_Delete(uuid1);
    ScUuid_Delete(uuid2);
    ScUuid_Delete(uuid3);

    LOG_CLOSE();

    return 0;
}
