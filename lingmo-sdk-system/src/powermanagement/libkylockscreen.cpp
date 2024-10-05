#include "lockscreen.h"
#include "libkylockscreen.h"

uint32_t kdk_set_inhibit_lockscreen(const char *appName , const char *reason)
{
    kdk::LockScreen lockScreen;
    return lockScreen.setInhibitLockScreen(appName , reason);
}

int kdk_un_inhibit_lockscreen(uint32_t flag)
{
    kdk::LockScreen lockScreen;
    if (lockScreen.unInhibitLockScreen(flag)) {
        return 0;
    } else {
        return -1;
    }

    return -1;
}