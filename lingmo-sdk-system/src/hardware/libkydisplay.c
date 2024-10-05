#include "libkydisplay.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring-extension.h>
#include <errno.h>
#include <libkylog.h>

char *display_get_info(char *str)
{
    char *display = (char*)malloc(sizeof(char) * 512);
    if (!display)
    {
        klog_err("内存申请失败：%s\n", strerror(errno));
        return NULL;
    }
    memset(display, 0, 512);
    char buf[1024] = {0};
    memset(buf, 0, sizeof(buf));
    int i = 0;
	char *buff[3], *p = NULL;
    FILE *pipeLine = popen("lshw -C display", "r");
    if (!pipeLine)
    {
        free(display);
        return NULL;
    }
    if (__glibc_likely(pipeLine != NULL))
    {
        while (fgets(buf, sizeof(buf), pipeLine))
        {
            if(strstr(buf, str))
            {
                p = strtok(buf, ":");
                while(p)
                {
                    buff[i] = p;
                    i++;
                    p = strtok(NULL,"");
                }
                strcpy(display, buff[1]);
		        strstripspace(display);
            }
        }
    }
    pclose(pipeLine);
    return display;
}

char* kdk_display_get_vendor()
{
    return display_get_info("vendor");
}

char* kdk_display_get_product()
{
    return display_get_info("product");
}

char* kdk_display_get_description()
{
    return display_get_info("description");
}

char* kdk_display_get_physical_id()
{
    return display_get_info("physical id");
}

char* kdk_display_get_bus_info()
{
    return display_get_info("bus info");
}

char* kdk_display_get_version()
{
    return display_get_info("version");
}

char* kdk_display_get_width()
{
    return display_get_info("width");
}

char* kdk_display_get_clock()
{
    return display_get_info("clock");
}

char* kdk_display_get_capabilities()
{
    return display_get_info("capabilities");
}

char* kdk_display_get_configuration()
{
    return display_get_info("configuration");
}

char* kdk_display_get_resources()
{
    return display_get_info("resources");
}
