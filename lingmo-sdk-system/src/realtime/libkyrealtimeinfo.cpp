#include "libkyrealtimeinfo.h"

float (*net_speed)(const char *) = kdk_real_get_net_speed;
double (*cpu_temperature)() = kdk_real_get_cpu_temperature;
int (*disk_temperature)(const char *name) = kdk_real_get_disk_temperature;
int (*disk_rate)(const char *name) = kdk_real_get_disk_rate;
float (*if_speed)(const char *nc) = kdk_real_get_if_speed;

RealTimeInfo::RealTimeInfo()
{
}

float RealTimeInfo::kdk_real_get_io_speed()
{
    return 0;
}

float RealTimeInfo::kdk_real_get_net_speed(const char *nc)
{
    return net_speed(nc);
}

float RealTimeInfo::kdk_real_get_if_speed(const char *nc)
{
    return if_speed(nc);
}

float RealTimeInfo::kdk_real_get_cpu_temperature()
{
    return cpu_temperature();
}

float RealTimeInfo::kdk_real_get_disk_temperature(const char *name)
{
    return disk_temperature(name);
}

int RealTimeInfo::kdk_real_get_disk_rate(const char *name)
{
    return disk_rate(name);
}