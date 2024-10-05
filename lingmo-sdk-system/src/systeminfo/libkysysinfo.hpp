#ifndef KYSDK_SYSTEM_SYSINFO_H__
#define KYSDK_SYSTEM_SYSINFO_H__

#include <string>
#include <stdbool.h>

/**
 * @file systeminfo.hpp（停止维护，请使用C语言接口）
 * @author liuyunhe (liuyunhe@lingmoos.cn)
 * @brief 操作系统基础信息
 * @version 0.1
 * @date 2021-11-02
 * 
 * @copyright Copyright (c) 2021
 * 
 */

namespace KDK_SYSTEM {

/**
 * @brief 获取架构类型
 * 
 * @return std::string 
 */
extern std::string getSystemArchitecture();

/**
* @brief 获取操作系统名称
* 
* @return std::string 
*/
extern std::string getSystemName();

/**
 * @brief 获取操作系统版本号
 * 
 * @param verbose 
 * @return std::string 
 */
extern std::string getSystemVersion(bool verbose = false);

/**
 * @brief 获取操作系统激活状态
 * 
 * @return true 
 * @return false 
 */
extern bool getSystemActivationStatus();

/**
 * @brief 获取操作系统服务序列号
 * 
 * @return std::string 
 */
extern std::string getSystemSerialNumber();

/**
 * @brief 获取内核版本号
 * 
 * @return std::string 
 */
extern std::string getKernelVersion();
}

#endif  // KYSDK_SYSTEM_SYSINFO_H__