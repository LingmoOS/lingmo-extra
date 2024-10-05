#include "stdio.h"
#include "../libkybios.h"
#include <stdlib.h>
int main()
{
    char *vendor = kdk_bios_get_vendor();
    char *version = kdk_bios_get_version();
    printf("vendor: %s", vendor);
    printf("version: %s",version);
    free(vendor);
    free(version);
    return 0;
}
