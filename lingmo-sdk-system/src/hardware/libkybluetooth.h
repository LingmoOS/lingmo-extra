#ifndef LIBKYBLUETOOTH_H
#define LIBKYBLUETOOTH_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 获取蓝牙的设备id
 * 
 * @return const int** 蓝牙的设备id
 */
extern int** kdk_bluetooth_get_device_id();

/**
 * @brief 获取蓝牙的制造商
 * 
 * @param num 蓝牙设备的个数，从0开始，0代表1个，以此类推。
 * @return const char* 蓝牙的制造商
 */
extern char* kdk_bluetooth_get_manufacturer(int num);

/**
 * @brief 获取蓝牙的设备版本
 * 
 * @param num 蓝牙设备的个数，从0开始，0代表1个，以此类推。
 * @return const char* 蓝牙的设备版本
 */
extern char* kdk_bluetooth_get_dev_version(int num);

/**
 * @brief 获取蓝牙的名称
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的名称
 */
extern char* kdk_bluetooth_get_name(int id);

/**
 * @brief 获取蓝牙的地址
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的地址
 */
extern char* kdk_bluetooth_get_address(int id);

/**
 * @brief 获取蓝牙的连接模式
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的连接模式
 */
extern char* kdk_bluetooth_get_link_mode(int id);

/**
 * @brief 获取蓝牙的连接策略
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的连接策略
 */
extern char* kdk_bluetooth_get_link_policy(int id);

/**
 * @brief 获取蓝牙的总线
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的总线
 */
extern char* kdk_bluetooth_get_bus(int id);

/**
 * @brief 获取蓝牙的SCO MTU
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的SCO MTU
 */
extern char* kdk_bluetooth_get_scomtu(int id);

/**
 * @brief 获取蓝牙的ALC MTU
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的ALC MTU
 */
extern char* kdk_bluetooth_get_alcmtu(int id);

/**
 * @brief 获取蓝牙的数据包类型
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的数据包类型
 */
extern char* kdk_bluetooth_get_packettype(int id);

/**
 * @brief 获取蓝牙的特征
 * 
 * @param id 蓝牙的设备id
 * @return const char* 蓝牙的特征
 */
extern char* kdk_bluetooth_get_features(int id);

#ifdef __cplusplus
}
#endif

#endif
