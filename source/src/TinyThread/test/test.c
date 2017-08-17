
#include <stdio.h>

#include "ScThread.h"
#include "ScMutex.h"
#include "ScCondition.h"
#include "sc_log.h"

void worker1(void *param)
{
    int i = 0;

    ScMutex *mutex = (ScMutex *)param;
    ScMutex_Lock(mutex);

    while (i < 10)
    {
        LOG_I("worker1", "i = %d", i);
        i++;
        
        sc_usleep(100);
    }

    ScMutex_Unlock(mutex);
}

void worker2(void *param)
{
    int i = 0;
    
    ScMutex *mutex = (ScMutex *)param;
    ScMutex_Lock(mutex);

    while (i < 10)
    {
        LOG_I("worker2", "i = %d", i);
        i++;
        sc_usleep(100);
    }

    ScMutex_Unlock(mutex);
}

int test_mutex()
{
    ScMutex *mutex = ScMutex_New();
    ScThread *work1_thread = ScThread_New();
    ScThread *work2_thread = ScThread_New();

    ScThread_Initialize(work1_thread, worker1, mutex, "work1");
    ScThread_Initialize(work2_thread, worker2, mutex, "work2");

    LOG_I("test", "test_mutex");

    ScThread_Start(work1_thread);
    ScThread_Start(work2_thread);

    ScThread_Join(work1_thread);
    ScThread_Join(work2_thread);

    ScThread_Delete(work1_thread);
    ScThread_Delete(work2_thread);

    ScMutex_Delete(mutex);

    return 0;
}

void worker3(void *param)
{
    int i = 0;

    ScCondition *condition = (ScCondition *)param;

    while (i < 10)
    {
        LOG_I("worker3", "i = %d", i);
        i++;
        sc_usleep(1000);
    }

    ScCondition_NotifyOne(condition);
}

void worker4(void *param)
{
    ScCondition *condition = (ScCondition *)param;

    LOG_I("worker4", "wait before");

    ScCondition_Wait(condition);

    LOG_I("worker4", "wait after");
}

int test_condition()
{
    ScCondition *condition = ScCondition_New();
    ScThread *work3_thread = ScThread_New();
    ScThread *work4_thread = ScThread_New();

    ScThread_Initialize(work3_thread, worker3, condition, "work3");
    ScThread_Initialize(work4_thread, worker4, condition, "work4");

    LOG_I("test", "test_condition");

    ScThread_Start(work3_thread);
    ScThread_Start(work4_thread);

    ScThread_Join(work3_thread);
    ScThread_Join(work4_thread);

    ScThread_Delete(work3_thread);
    ScThread_Delete(work4_thread);

    ScCondition_Delete(condition);

    return 0;
}

void worker0(void *param)
{
    int i = 0;

    while (i < 10)
    {
        LOG_I("worker0", "i = %d", i);
        i++;

        sc_usleep(1000 * 1000);
    }
}

int test_thread()
{
    ScThread *thread = ScThread_New();
    ScThread_Initialize(thread, worker0, NULL, "work0");

    LOG_I("test", "test_thread");

    ScThread_Start(thread);

    ScThread_Join(thread);

    ScThread_Delete(thread);

    return 0;
}

int main()
{
    test_thread();

    test_mutex();

    test_condition();

    return 0;
}
