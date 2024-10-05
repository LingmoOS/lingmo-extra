#ifndef KDK_SYSTEM_HW_CPU_H__
#define KDK_SYSTEM_HW_CPU_H__
/**
 * @file libkycpu.h
 * @author liuyunhe (liuyunhe@lingmoos.cn)
 * @brief CPU信息
 * @version 0.1
 * @date 2021-11-17
 * 
 * @copyright Copyright (c) 2021
 * @defgroup 获取硬件信息
 * @{
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取CPU架构
 * 
 * @return const char* 架构信息，如"x86_64"；返回的是const char*，不要free
 */
extern const char* kdk_cpu_get_arch();

/**
 * @brief 获取CPU制造厂商
 * 
 * @return const char* 制造厂商字符串，如“GenuineIntel”；返回的是const char*，不要free
 */
extern const char* kdk_cpu_get_vendor();

/**
 * @brief 获取CPU型号
 * 
 * @return const char* CPU型号名称，如“Intel(R) Core(TM) i7-4790K CPU @ 4.00GHz”；返回的是const char*，不要free
 */
extern const char* kdk_cpu_get_model();

/**
 * @brief 获取CPU额定主频
 * 
 * @return const char* 额定主频赫兹数，如“1794.742”，单位是MHz；返回的是const char*，不要free
 */
extern const char* kdk_cpu_get_freq_MHz();

/**
 * @brief 获取CPU核心数量
 * 
 * @return unsigned int 所有可用的CPU核心数量
 */
extern unsigned int kdk_cpu_get_corenums();

/**
 * @brief 获取CPU对虚拟化的支持
 * 
 * @return const char* 若CPU支持虚拟化，则返回虚拟化技术，如“vmx”；若不支持，返回NULL；返回的是const char*，不要free
 */
extern const char* kdk_cpu_get_virt();

/**
 * @brief 获取CPU线程数
 * 
 * @return const char* 返回cpu支持的线程数
 */
extern unsigned int kdk_cpu_get_process();

#ifdef __cplusplus
}
#endif

#endif
/**
  * @}
  */