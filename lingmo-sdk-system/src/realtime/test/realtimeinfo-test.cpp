#include "../libkyrealtimeinfo.h"
#include <iostream>

int main()
{
    RealTimeInfo info;
    std::cout << "Of speed:" << info.kdk_real_get_net_speed("enp3s0") << std::endl;
    std::cout << "If speed:" << info.kdk_real_get_if_speed("enp3s0") << std::endl;
    std::cout << "cpu Temp:" << info.kdk_real_get_cpu_temperature() << std::endl;
    std::cout << "Disk Temp:" << info.kdk_real_get_disk_temperature("/dev/sda") << std::endl;
    std::cout << "Disk rate:" << info.kdk_real_get_disk_rate("/dev/sda") << std::endl;
    return 0;
}