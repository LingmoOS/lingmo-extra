#include "../libkyprinter.h"
#include <stdio.h>

int main()
{
    int index = 0;
    //获取全部打印机列表
    char **printers = kdk_printer_get_list();
    while (printers[index])
    {
        int status = kdk_printer_get_status(printers[index]);
        printf("%zd: %s , %d\n", index + 1, printers[index], status);
        index++;
    }
    kdk_printer_freeall(printers);
    //获取可用打印机列表
    index = 0;
    char **available_printers = kdk_printer_get_available_list();
    while (available_printers[index])
    {
        int status = kdk_printer_get_status(available_printers[index]);
        printf("%zd: %s , %d\n", index + 1, available_printers[index], status);
        index++;
    }
    kdk_printer_freeall(available_printers);

    return 0;
}
