#include "../libkyfan.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char **fan = kdk_fan_get_information();
    int index = 0;
    while (fan[index])
    {
        printf("%s\n", fan[index]);
        index++;
    }
    kdk_fan_freeall(fan);
    return 0;
}