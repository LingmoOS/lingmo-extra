#include "../libkycpu.h"
#include <stdio.h>

int main()
{
    printf("架构：%s\n", kdk_cpu_get_arch());
    printf("生产厂商：%s\n", kdk_cpu_get_vendor());
    printf("CPU 型号：%s\n", kdk_cpu_get_model());
    printf("CPU 主频：%s MHz\n", kdk_cpu_get_freq_MHz());
    printf("CPU 单核核心数量：%u\n", kdk_cpu_get_corenums());
    printf("CPU 虚拟化支持：%s\n", kdk_cpu_get_virt());
    printf("CPU 线程数：%u\n", kdk_cpu_get_process());
    return 0;
}