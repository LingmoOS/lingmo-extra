#include "processdaemon.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int quit;

void func(void* data)
{
    printf("dog fake.\n");
    quit = 1;
    KYSDK_BASE::stopProcDaemon();
}

int main()
{
    KYSDK_BASE::startProcDaemon(5, func, NULL);
    int counts = 0;
    srand(time(NULL));
    while (!quit)
    {
        int stime = rand() % 7;
        printf("sleep %d\n", stime);
        sleep(stime);
        KYSDK_BASE::feedDog();
    }

    printf("quit.\n");
    KYSDK_BASE::stopProcDaemon();
    return 0;
}