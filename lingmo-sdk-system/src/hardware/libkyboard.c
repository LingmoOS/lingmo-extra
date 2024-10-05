#include "libkyboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *kdk_board_get_name()
{
    char *name = (char *)calloc(64,sizeof(char));
    FILE *fd = fopen("/sys/class/dmi/id/board_name","r");
    if(fd)
    {
        fgets(name,64,fd);
        fclose(fd);
    }
    return name;
}

const char *kdk_board_get_date()
{
    char *date = (char *)calloc(64,sizeof(char));
    FILE *fd = fopen("/sys/class/dmi/id/bios_date","r");
    if(fd)
    {
        fgets(date,64,fd);
        fclose(fd);
    }
    return date;
}

const char *kdk_board_get_serial()
{
    char *serial = (char *)calloc(64,sizeof(char));
    FILE *fd = fopen("/sys/class/dmi/id/board_serial","r");
    if(fd)
    {
        fgets(serial,64,fd);
        fclose(fd);
    }
    return serial;
}

const char *kdk_board_get_vendor()
{
    char *vendor = (char *)calloc(64,sizeof(char));
    FILE *fd = fopen("/sys/class/dmi/id/board_vendor","r");
    if(fd)
    {
        fgets(vendor,64,fd);
        fclose(fd);
    }
    return vendor;
}

void kdk_board_free(char* info)
{
    free(info);
}
