#include "stdio.h"
#include "../libkyboard.h"

int main()
{
    
    char *name = kdk_board_get_name();
    char *vendor = kdk_board_get_vendor();
    char *date = kdk_board_get_date();
    char *serial = kdk_board_get_serial();
    printf("name : %s", name);
    printf("vendor : %s", vendor);
    printf("date : %s", date);
    printf("serial : %s", serial);
    free(name);
    free(vendor);
    free(date);
    free(serial);
    return 0;
}
