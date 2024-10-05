#ifndef KDK_SYSTEM_HW_NWCARD_H__
#define KDK_SYSTEM_HW_NWCARD_H__
/**
 * @file libkync.h
 * @author liuyunhe (liuyunhe@lingmoos.cn)
 * @brief 网卡信息
 * @version 0.1
 * @date 2021-11-17
 * 
 * @copyright Copyright (c) 2021
 * 
 * @defgroup 获取硬件信息
 * @{
 * 
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取系统中所有的网卡
 * 
 * @return char** 网卡名称列表，由NULL字符串表示结尾；由alloc生成，需要被kdk_nc_freeall回收；若获取出错，返回NULL；
 */
extern char** kdk_nc_get_cardlist();

/**
 * @brief 检测指定网卡是否处于UP状态
 * 
 * @param nc 网卡名称，如eno1
 * @return int Up返回1，Down返回0
 */
extern int kdk_nc_is_up(const char *nc);

/**
 * @brief 获取系统中当前处于 link up 状态的网卡列表
 * 
 * @return char** 网卡名称列表，由NULL字符串表示结尾；由alloc生成，需要被kdk_nc_freeall回收；若获取出错，返回NULL；
 */
extern char** kdk_nc_get_upcards();

/**
 * @brief 获取指定网卡的物理MAC地址
 * 
 * @param nc 网卡名称，如eno1
 * @return char* 物理MAC地址，由alloc生成，应当被free；若指定网卡不存在，则返回NULL
 */
extern char* kdk_nc_get_phymac(const char *nc);

/**
 * @brief 获取指定网卡的第一个IPv4地址
 * 
 * @param nc 网卡名称，如eno1
 * @return char* IPv4地址，获取出错或无IP返回NULL
 */
extern char* kdk_nc_get_private_ipv4(const char *nc);

/**
 * @brief 获取指定网卡的所有IPv4地址
 * 
 * @param nc 网卡名称，如eno1
 * @return char** IPv4地址列表，以NULL表示结尾，由alloc生成，需要被kdk_nc_freeall回收；若获取出错，返回NULL；
 */
extern char** kdk_nc_get_ipv4(const char* nc);

/**
 * @brief 获取指定网卡的第一个IPv6地址
 * 
 * @param nc 网卡名称，如eno1
 * @return char* IPv6地址，获取出错或无IP返回NULL
 */
extern char* kdk_nc_get_private_ipv6(const char *nc);

/**
 * @brief 获取指定网卡的所有IPv6地址
 *
 * @param nc 网卡名称，如eno1
 * @return char** IPv6地址列表，以NULL表示结尾，由alloc生成，需要被kdk_nc_freeall回收；若获取出错，返回NULL；
 */
extern char **kdk_nc_get_ipv6(const char *nc);

/**
 * @brief 获取指定网卡的有线/无线类型
 * 
 * @param nc 网卡名称，如eno1
 * @return int 0 有线 1 无线
 */
extern int kdk_nc_is_wireless(const char *nc);

/**
 * @brief 获取指定网卡的厂商名称和设备型号
 * 
 * @param nc 网卡名称，如eno1
 * @param vendor 网卡制造厂商，出参，需要提前分配好空足够空间
 * @param porduct 网卡型号，出参，需要提前分配好空足够空间
 * @return int 成功返回 0 失败返回 -1
 */
extern int kdk_nc_get_vendor_and_product(const char *nc, char *vendor, char *product);

/**
 * @brief 获取指定网卡的驱动
 * 
 * @param nc 网卡名称，如eno1
 * @return char * 网卡驱动
 */
extern char *kdk_nc_get_driver(const char *nc);

/**
 * @brief 用于回收字符串列表
 * 
 * @param ptr 字符串列表
 */
extern inline void kdk_nc_freeall(char **ptr);

#ifdef __cplusplus
}
#endif

#endif
/**
  * @}
  */
