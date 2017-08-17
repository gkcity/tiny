#include <tiny_log.h>
#include "bootstrap/Bootstrap.h"

#define TAG "SampleTimer"

static void timeout(Timer *thiz, void *ctx)
{
    LOG_I(TAG, "timeout");
}

int main()
{
    Timer *timer = NULL;
    Bootstrap sb;

    // new Timer
    timer = Timer_New("mytimer", 1 * 1000, NULL, timeout);

    // Starting Bootstrap
    Bootstrap_Construct(&sb);
    Bootstrap_AddTimer(&sb, timer);
    Bootstrap_Sync(&sb);
    Bootstrap_Shutdown(&sb);
    Bootstrap_Dispose(&sb);

    return 0;
}