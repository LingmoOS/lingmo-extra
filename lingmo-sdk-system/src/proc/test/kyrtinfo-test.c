#include "../libkyrtinfo.h"
#include <stdio.h>

int main()
{
    printf("内存总大小：%lu KiB\n", kdk_rti_get_mem_res_total_KiB());
    printf("当前已用内存：%lu KiB，%f\n", kdk_rti_get_mem_res_usage_KiB(), kdk_rti_get_mem_res_usage_percent());
    printf("当前空闲内存：%lu KiB\n", kdk_rti_get_mem_res_free_KiB());
    printf("可用内存大小：%lu KiB\n", kdk_rti_get_mem_res_available_KiB());
    printf("应用总申请虚拟内存大小：%lu KiB\n", kdk_rti_get_mem_virt_alloc_KiB());
    printf("交换分区总量：%lu KiB\n", kdk_rti_get_mem_swap_total_KiB());
    printf("交换分区已用量：%lu KiB, %f\n", kdk_rti_get_mem_swap_usage_KiB(), kdk_rti_get_mem_swap_usage_percent());
    printf("交换分区剩余大小：%lu KiB\n", kdk_rti_get_mem_swap_free_KiB());
    printf("当前CPU使用率：%f\n", kdk_rti_get_cpu_current_usage());
    unsigned int day, hour, min, sec;
    kdk_rti_get_uptime(&day, &hour, &min, &sec);
    printf("开机时长：%u天%u小时%u分钟%u秒\n", day, hour, min, sec);
    return 0;
}