#ifndef KDK_SYSTEM_PACKAGES_H__
#define KDK_SYSTEM_PACKAGES_H__

/**
 * @file libkypackages.h
 * @author liuyunhe (liuyunhe@lingmoos.cn)
 * @brief 包列表信息
 * @version 0.1
 * @date 2021-11-17
 * 
 * @copyright Copyright (c) 2021
 * @defgroup 获取包列表信息
 * @{
 * 
 * 
 */
// typedef enum kdk_packageStatus {
//     UNINIT,
//     INSTALLED,
//     UNCONFIG,
//     NONE
// }kdk_packageStatus;
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _kdk_package_t{
    char *name;     // 包名
    char *version;  // 版本号
    char *section;  // 包类型
    char *status;   // 状态
    unsigned long size_kb;   // 包大小
    struct _kdk_package_t **depends; // 依赖包
}kdk_package_t;

typedef struct {
    unsigned int nums;
    kdk_package_t **list;
}kdk_package_list;

/**
 * @brief 获取系统中所有包列表
 * 
 * @return kdk_package_list* 包描述结构体列表；返回的结构体需要由kdk_package_free_packagelist()回收
 */
extern kdk_package_list* kdk_package_get_packagelist();

/**
 * @brief 回收由kdk_package_get_packagelist()返回的结构体
 * 
 * @param list 由kdk_package_get_packagelist()返回的结构体
 */
extern void kdk_package_free_packagelist(kdk_package_list *list);

/**
 * @brief [未启用]安装指定的软件包
 * 
 * @param packagePath 软件包路径
 * @return int 成功返回0，失败返回错误码
 */
extern int kdk_package_install_package(const char *packagePath);

/**
 * @brief [未启用]删除指定的包
 * 
 * @param name 包名
 * @param version 指定版本号，大部分情况下为NULL，为NULL时删除所有版本，该参数只是为以后snap兼容做准备
 * @return int 成功返回0，失败返回错误码
 */
extern int kdk_package_remove_package(const char *name, const char *version);

/**
 * @brief 获取系统中指定包的版本号
 * 
 * @param name 软件包名
 * @return char* 版本号，由alloc生成，需要被free；若给定的包不存在，则返回NULL
 */
extern char* kdk_package_get_version(const char *name);

/**
 * @brief 检测指定包名的软件包是否正确安装
 * 
 * @param name 包名
 * @param version 版本号，大部分情况下为NULL，预留为snap做准备
 * @return int 成功返回0，失败返回错误码
 */
extern int kdk_package_is_installed(const char *name, const char *version);

#ifdef __cplusplus
}
#endif

#endif
/**
  * @}
  */