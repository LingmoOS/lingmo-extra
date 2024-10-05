#include "libkybios.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *kdk_bios_get_vendor()
{
    char *vendor = (char *)calloc(32,sizeof(char));
    FILE *fd = fopen("/sys/class/dmi/id/bios_vendor","r");
    if(fd)
    {
        fgets(vendor,32,fd);
        fclose(fd);
    }
    return vendor;
}

const char *kdk_bios_get_version()
{
    char *version = (char *)calloc(32,sizeof(char));
    FILE *fd = fopen("/sys/class/dmi/id/bios_version","r");
    if(fd)
    {
        fgets(version,32,fd);
        fclose(fd);
    }
    return version;
}

void kdk_bios_free(char* info)
{
    free(info);
}