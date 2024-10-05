#include <stdio.h>

#include "libkydiagnostics.h"

int main(void)
{
    char appName[] = "lingmo-font-viewer";
    char messageType[] = "FunctionType";

    KBuriedPoint pt[2];
    pt[0].key = "testkey";
    pt[0].value = "testvalue";
    pt[1].key = "testkey1";
    pt[1].value = "testvalue1";

    if (kdk_buried_point(appName , messageType , pt , 2)) {
        printf("buried point fail !\n");
        return -1;
    }

    printf("buried point success !\n");

    return 0;
}