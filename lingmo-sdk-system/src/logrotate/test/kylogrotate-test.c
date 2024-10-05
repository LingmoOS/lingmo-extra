#include <stdio.h>
#include <libkylog.h>
#include <unistd.h>

int main()
{
    while(1)
    {
        klog_err("logrotate 测试程序\n");
        usleep(500);
    }
}