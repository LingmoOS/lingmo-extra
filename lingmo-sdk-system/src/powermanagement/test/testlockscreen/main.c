#include <libkylockscreen.h>
#include <stdio.h>

int main(void)
{
    char appName[] = "test-package";
    char reason[] = "test-reason";

    uint32_t flag;
    if ((flag = kdk_set_inhibit_lockscreen(appName , reason)) == 0) {
        printf("set inhibit lock screen fail !\n");
        return -1;
    }

    if (kdk_un_inhibit_lockscreen(flag)) {
        printf("un inhibit lock screen fail !\n");
        return -1;
    }

    return 0;
}