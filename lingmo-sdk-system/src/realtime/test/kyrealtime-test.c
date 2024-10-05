#include "stdio.h"
#include "../libkyrealtimeinfo.h"

int main()
{
    // if(argc < 3)
    // {
    //     printf("./kyrealtime-test <网卡名> <磁盘名>\n");
    //     return 0;
    // }
    printf("netSpeed : %f\n", kdk_real_get_net_speed("enaftgm1i0"));
    printf("cpuTemp: %f\n", kdk_real_get_cpu_temperature());
    printf("diskTemp : %d\n", kdk_real_get_disk_temperature("/dev/nvme0n1"));
    printf("diskRate : %d\n", kdk_real_get_disk_rate("/dev/nvme0n1"));
    printf("ifnetSpeed : %f\n", kdk_real_get_if_speed("enaftgm1i0"));
    return 0;
}
