#include "stdio.h"
#include "../libkylocation.h"

int main()
{
    printf("location: %s", kdk_loaction_get());
    return 0;
}