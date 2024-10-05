#ifndef KYSDK_SYSTEM_SYSINFO_H__
#define KYSDK_SYSTEM_SYSINFO_H__

/**
 * @file libkysysinfo.h
 * @author liuyunhe (liuyunhe@lingmoos.cn)
 * @brief 操作系统基础信息
 * @version 0.1
 * @date 2021-11-02
 * 
 * @copyright Copyright (c) 2021
 * 
 * @defgroup 获取系统基础信息
 * @{
 * 
 */

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char    *os_version;            //系统版本
    char    *update_version;        //补丁版本
} version_t;

struct KPci
{
    char slot_path[16];
    char class_name[128];
    char product_name[128];
    unsigned char rev;
    char ss_name[256];
    char driver_use[1024];
    char **modules;
    int module_count;
    struct KPci *next;
};

/**
 * @brief 获取操作系统架构信息
 * 
 * @return char* 成功返回字符串，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_architecture();

/**
 * @brief 获取操作系统名称
 * 
 * @return char* 成功返回字符串，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_systemName();

/**
 * @brief 获取操作系统版本号
 * 
 * @param verbose 0获取简略版本号，1获取详细版本号
 * @return char* 成功返回字符串，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_version(bool verbose);

/**
 * @brief 获取操作系统激活状态
 * 
 * @param status_error_num 用于接收激活状态的错误信息
 * @param date_error_num 用于接收获取技术服务状态错误信息
 * 
 * @return int 2表示已过期；0表示未激活，处于试用期；1表示已激活；-1表示接口内部错误；
 */
extern int kdk_system_get_activationStatus(int *status_error_num, int *date_error_num);

/**
 * @brief 获取操作系统服务序列号
 * 
 * @return char* 成功返回字符串，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_serialNumber();

/**
 * @brief 获取内核版本号
 * 
 * @return char* 成功返回字符串，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_kernelVersion();

/**
 * @brief 获取当前登录用户的用户名（Effect User）
 * 
 * @return char* 成功返回字符串，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_eUser();

/**
 * @brief 获取操作系统项目编号名
 * 
 * @return char* 成功返回字符串，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_projectName();

/**
 * @brief 获取操作系统项目子编号名
 * 
 * @return char* 成功返回字符串，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_projectSubName();

/**
 * @brief 获取操作系统产品标识码
 * 
 * @return unsigned int 返回标志码
 *         0000 信息异常
 *         0001 仅PC特性
 *         0010 仅平板特性
 *         0011 支持平板与PC特性
 */
extern unsigned int kdk_system_get_productFeatures();

/**
 * @brief 获取操作系统宿主机的虚拟机类型
 * 
 * @return char* 获取失败返回NULL，获取成功返回一个字符串，字符串内容如下：
 *         [none, qemu, kvm, zvm, vmware, hyper-v, orcale virtualbox, xen, bochs, \
 *          uml, parallels, bhyve, qnx, arcn, openvz, lxc, lxc-libvirt, systemd-nspawn,\
 *          docker, podman, rkt, wsl]
 *         其中 none 表示运行在物理机环境中；其他字符串代表具体的虚拟环境类型。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_hostVirtType();

/**
 * @brief 获取操作系统宿主机的云平台类型
 * 
 * @return char* 获取失败返回NULL，获取成功返回一个字符串，字符串内容如下：
 *               [none, huawei]
 *               其中 none 表示运行在物理机或未知的云平台环境中；其他字符串代表不同的云平台。返回的字符串需要被 free 释放
 */
extern char* kdk_system_get_hostCloudPlatform();

/**
 * @brief  判断当前镜像系统是否为 专用机 系统
 * 
 * @return bool true代表是 false代表不是
 */

extern bool kdk_system_is_zyj(void);

/**
 * @brief  获取系统版本号/补丁版本号
 * 
 * @return version_t 该系统版本号的详细信息，具体信息自取
 */

extern version_t kdk_system_get_version_detaile();

/**
 * @brief  获取系统当前显示屏幕,分辨率,系统支持分辨率
 * 
 * @return char** 当前显示屏幕,分辨率,系统支持分辨率
 */

extern char** kdk_system_get_resolving_power();

/**
 * @brief  获取显示系统硬件版本类别
 * 
 * @return char* 成功返回字符串系统版本硬件类别，版本类别有{Tablet, MaxTablet},Tablet-平板，MaxTablet-大屏；
 *          失败返回NULL，未读到文件或字段返回字符串none。返回的字符串需要被 free 释放。
 */

extern char* kdk_system_get_systemCategory();

/**
 * @brief  获取云平台类型
 * 
 * @return char* 成功返回字符串应用场景，应用场景有{EDU}，EDU-教育；失败返回NULL，未读到文件或字段返回字符串none。
 *          返回的字符串需要被 free 释放。
 */

// extern char* kdk_system_get_cloudPlatformType();

/**
 * @brief  获取系统开机时间
 * 
 * @return char** 开机时间
 */

extern char** kdk_system_get_startup_time();

/**
 * @brief  获取系统关机时间
 * 
 * @return char** 关机时间
 */

extern char** kdk_system_get_shutdown_time();

/**
 * @brief  获取整机制造商
 * 
 * @return char* 整机制造商，失败返回NULL。返回的字符串需要被 free 释放
 */

extern char *kdk_get_host_vendor();

/**
 * @brief  获取整机型号
 * 
 * @return char* 整机型号，失败返回NULL。返回的字符串需要被 free 释放
 */

extern char *kdk_get_host_product();

/**
 * @brief  获取整机序列号
 * 
 * @return char* 整机序列号，失败返回NULL。返回的字符串需要被 free 释放
 */

extern char *kdk_get_host_serial();

/**
 * @brief 获取主机名
 *
 * @return char* 主机名，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char *kdk_system_get_hostName();

/**
 * @brief 获取操作系统位数
 *
 * @return int 返回系统位数
 */
extern int kdk_system_get_word();

/**
 * @brief 获取操作系统构建时间
 *
 * @return char* 操作系统构建时间，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char *kdk_system_get_buildTime();

/**
 * @brief 用于回收字符串列表
 * 
 * @param ptr 字符串列表
 */
extern inline void kdk_resolving_freeall(char **ptr);

/**
 * @brief 获取总线信息
 * @return pci总线信息结构体，具体参考返回结构体，lspci -k的返回
 */
extern struct KPci *kdk_hw_get_pci_info();

/**
 * @brief 释放由kdk_hw_get_pci_info返回的pci总线信息结构体
 * 
 * @param info 由kdk_hw_get_pci_info返回的结构体指针
 */
extern void kdk_hw_free_pci_info(struct KPci *info);

/**
 * @brief 获取应用场景
 * 
 * @return char* 成功返回字符串应用场景，应用场景有{EDU}，失败返回NULL。返回的字符串需要被 free 释放
 */
extern char *kdk_system_get_appScene();

#ifdef __cplusplus
}
#endif

#endif  // KYSDK_SYSTEM_SYSINFO_H__

/**
  * @}
  */
