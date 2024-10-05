#ifndef LIBKYREALTIMEINFO_H
#define LIBKYREALTIMEINFO_H

/**
 * @file libkyrtinfo.h
 * @author liuyunhe (liuyunhe@lingmoos.cn)
 * @brief 瞬时信息
 * @version 0.1
 * @date 2023-2-17
 * 
 * @copyright Copyright (c) 2021
 * @defgroup 获取瞬时信息
 * @{
 * 
 */

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief 获取瞬时网速信息
 * 
 * @param nc 网卡名称，如eno1
 * @return float 瞬时网速
 */
extern float kdk_real_get_net_speed(const char *nc);

/**
 * @brief 获取CPU温度
 * 
 * 
 * @return double CPU温度
 */
extern double kdk_real_get_cpu_temperature();

/**
 * @brief 获取磁盘温度
 * 
 * @param name 磁盘名称，应当是例如/dev/sda这种绝对路径
 * @return int 磁盘温度
 */
extern int kdk_real_get_disk_temperature(const char *name);

/**
 * @brief 获取磁盘转速
 * 
 * @param name 磁盘名称，应当是例如/dev/sda这种绝对路径
 * @return int 磁盘转速
 */
extern int kdk_real_get_disk_rate(const char *name);

/**
 * @brief 获取下载的瞬时网速信息
 * 
 * @param nc 网卡名称，如eno1
 * @return float 下载的瞬时网速
 */
extern float kdk_real_get_if_speed(const char *nc);

#ifdef __cplusplus
}
#endif

/***********************************
 * 类已弃用 为之前已引用了c++接口的应用保留
 * 新应用获取瞬时信息请使用上面的c接口
************************************/
#ifdef __cplusplus

#define REAL_TIME_EXPORT     __attribute__((visibility("default")))
class REAL_TIME_EXPORT RealTimeInfo
{
public:
    RealTimeInfo();
public:
    float kdk_real_get_io_speed();
    float kdk_real_get_net_speed(const char *nc);
    float kdk_real_get_if_speed(const char *nc);
    float kdk_real_get_cpu_temperature();
    float kdk_real_get_disk_temperature(const char *name);
    int kdk_real_get_disk_rate(const char *name);
};
#endif

#endif // LIBKYREALTIMEINFO_H
