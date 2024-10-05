#include "../libkydiskinfo.h"
#include <stdio.h>

int main()
{
    char **disklist = kdk_get_disklist();
    for (int i = 0; disklist[i]; i ++)
    {
        printf("%s\n", disklist[i]);
    }

    kdk_free_disklist(disklist);

    return 0;
}