#include <TinyList.h>
#include <tiny_log.h>
#include <tiny_snprintf.h>
#include <tiny_malloc.h>

#define TAG     "test"

static void on_delete(void * data, void *ctx)
{
//    LOG_I(TAG, "on_delete: %s", (const char *)data);
    tiny_free(data);
}

static int test_add_head(TinyList *list)
{
    for (int i = 0; i < 10; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hello: %d", i);
        TinyList_AddHead(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 10, -1);

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    for (int i = 0; i < 20; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hi: %d", i);
        TinyList_AddHead(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 20, -1);

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    for (int i = 0; i < 100; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "aaa: %d", i);
        TinyList_AddHead(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 100, -1);

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    for (int j = 0; j < 10; ++j)
    {
        for (int i = 0; i < 10; ++i)
        {
            char *data = tiny_malloc(20);
            memset(data, 0, 20);
            tiny_snprintf(data, 20, "hello: %d", i);
            TinyList_AddHead(list, data);
        }

        RETURN_VAL_IF_FAIL(list->size == 10, -1);

        TinyList_RemoveAll(list);

        RETURN_VAL_IF_FAIL(list->size == 0, -1);
    }

#ifdef TINY_DEBUG
    for (int j = 0; j < 100; ++j)
    {
        int count = (j + 1) * 100;

        for (int i = 0; i < 100; ++i)
        {
            char *data = tiny_malloc(20);
            memset(data, 0, 20);
            tiny_snprintf(data, 20, "hello: %d", i);
            TinyList_AddHead(list, data);
        }

        RETURN_VAL_IF_FAIL(list->size == count, -1);
    }
#endif

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    return 0;
}

static int test_add_tail(TinyList *list)
{
    for (int i = 0; i < 10; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hello: %d", i);
        TinyList_AddTail(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 10, -1);

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    for (int i = 0; i < 20; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hi: %d", i);
        TinyList_AddTail(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 20, -1);

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    for (int i = 0; i < 100; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "aaa: %d", i);
        TinyList_AddTail(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 100, -1);

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    for (int j = 0; j < 10; ++j)
    {
        for (int i = 0; i < 10; ++i)
        {
            char *data = tiny_malloc(20);
            memset(data, 0, 20);
            tiny_snprintf(data, 20, "hello: %d", i);
            TinyList_AddTail(list, data);
        }

        RETURN_VAL_IF_FAIL(list->size == 10, -1);

        TinyList_RemoveAll(list);

        RETURN_VAL_IF_FAIL(list->size == 0, -1);
    }

    for (int j = 0; j < 100; ++j)
    {
#ifdef TINY_DEBUG
        int count = (j + 1) * 100;

        for (int i = 0; i < 100; ++i)
        {
            char *data = tiny_malloc(20);
            memset(data, 0, 20);
            tiny_snprintf(data, 20, "hello: %d", i);
            TinyList_AddTail(list, data);
        }

        RETURN_VAL_IF_FAIL(list->size == count, -1);
#endif
    }

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    return 0;
}

static int test_insert(TinyList *list)
{
    for (int i = 0; i < 10; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hello: %d", i);
        TinyList_AddHead(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 10, -1);

    for (int i = 0; i < 10; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hi: %d", i);
        TinyList_InsertBefore(list, 9, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 20, -1);

    for (int i = 0; i < 10; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "aaa: %d", i);
        TinyList_InsertAfter(list, 1, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 30, -1);

    for (uint32_t i = 0; i < list->size; ++i)
    {
    #ifdef TINY_DEBUG
        const char * data = TinyList_GetAt(list, i);
        LOG_D(TAG, "[%d] %s", i, data);
    #endif
    }

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    return 0;
}

static int test_remove_head(TinyList *list)
{
    for (int i = 0; i < 10; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hello: %d", i);
        TinyList_AddHead(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 10, -1);

    for (int i = 0; i < 5; ++i)
    {
        //TinyList_RemoveHead(list);
    }

    RETURN_VAL_IF_FAIL(list->size == 5, -1);

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    return 0;
}

static int test_remove_tail(TinyList *list)
{
    for (int i = 0; i < 100; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hello: %d", i);
        TinyList_AddTail(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 100, -1);

    for (int i = 0; i < 50; ++i)
    {
        //TinyList_RemoveTail(list);
    }

    RETURN_VAL_IF_FAIL(list->size == 50, -1);

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    return 0;
}

static int test_remove(TinyList *list)
{
    for (int i = 0; i < 100; ++i)
    {
        char *data = tiny_malloc(20);
        memset(data, 0, 20);
        tiny_snprintf(data, 20, "hello: %d", i);
        TinyList_AddTail(list, data);
    }

    RETURN_VAL_IF_FAIL(list->size == 100, -1);

    for (int i = 99; i >=0; i -= 2)
    {
        TinyList_RemoveAt(list, i);
    }

    RETURN_VAL_IF_FAIL(list->size == 50, -1);

    for (uint32_t i = 0; i < list->size; ++i)
    {
#ifdef TINY_DEBUG
        const char * data = TinyList_GetAt(list, i);
        LOG_D(TAG, "[%d] %s", i, data);
#endif
    }

    TinyList_RemoveAll(list);

    RETURN_VAL_IF_FAIL(list->size == 0, -1);

    return 0;
}

void test1(TinyList *list)
{
    LOG_D(TAG, "test start");

    RETURN_IF_FAIL(test_add_head(list) == 0);
    RETURN_IF_FAIL(test_add_tail(list) == 0);
    RETURN_IF_FAIL(test_insert(list) == 0);
    RETURN_IF_FAIL(test_remove_head(list) == 0);
    RETURN_IF_FAIL(test_remove_tail(list) == 0);
    RETURN_IF_FAIL(test_remove(list) == 0);

    LOG_D(TAG, "test finished");
}

int main(int argc, char *argv[])
{
    TinyList list;

    TinyList_Construct(&list, on_delete, NULL);

    test1(&list);

    TinyList_Dispose(&list);

    return 0;
}
