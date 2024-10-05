#include "../libkyprocess.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    // char *run_time = kdk_get_process_running_time(841);
    // char *cpu_time = kdk_get_process_cpu_time(841);
    // char *cmd = kdk_get_process_command(841);
    // char *start_time = kdk_get_process_start_time(841);
    // printf("获取某一进程的CPU利用率：%0.1f\n", kdk_get_process_cpu_usage_percent(841));
    // printf("获取某一进程的内存占用率：%0.1f\n", kdk_get_process_mem_usage_percent(841));
    // printf("获取某一进程的进程状态：%s\n", kdk_get_process_status(841));
    // printf("获取某一进程的进程端口号：%d\n", kdk_get_process_port(841));
    // printf("获取某一进程的启动时间：%s\n", start_time);
    // printf("获取某一进程的运行时间：%s\n", run_time);
    // printf("获取某一进程的CPU时间：%s\n", cpu_time);
    // printf("获取某一进程的Command：%s\n", cmd);
    // printf("获取某一进程的属主：%s\n", kdk_get_process_user(841));
    // free(run_time);
    // free(cpu_time);
    // free(cmd);
    // free(start_time);
    // char** pid =  kdk_procname_get_process_infomation("systemd");
    // size_t index = 0;
    // while (pid[index])
    // {
    //     printf("pid %s\n", pid[index]);
    //     index ++;
    // }
    // kdk_proc_freeall(pid);

    char** info = kdk_get_process_all_information();
    size_t count = 0;
    while (info[count])
    {
        printf("No. %d\t %s\n", count + 1, info[count]);
        count ++;
    }
    kdk_proc_freeall(info);

    // char *name = kdk_get_process_name(1151571);
    // printf("name = %s\n", name);
    // free(name);

    // int id =  kdk_get_process_id("systemd");
    // printf("pid %d\n", id);
   
    return 0;
}
