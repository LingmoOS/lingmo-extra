#ifndef LIBKYDISPLAY_H
#define LIBKYDISPLAY_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 获取显卡的制造商
 * 
 * @return char* 显卡的制造商
 */
extern char* kdk_display_get_vendor();

/**
 * @brief 获取显卡的型号
 * 
 * @return char* 显卡的型号
 */
extern char* kdk_display_get_product();

/**
 * @brief 获取显卡的说明
 * 
 * @return char* 显卡的说明
 */
extern char* kdk_display_get_description();

/**
 * @brief 获取显卡的物理id
 * 
 * @return char* 显卡的物理id
 */
extern char* kdk_display_get_physical_id();

/**
 * @brief 获取显卡的总线地址
 * 
 * @return char* 显卡的总线地址
 */
extern char* kdk_display_get_bus_info();

/**
 * @brief 获取显卡的设备版本
 * 
 * @return char* 显卡的设备版本
 */
extern char* kdk_display_get_version();

/**
 * @brief 获取显卡的数据宽度
 * 
 * @return char* 显卡的数据宽度
 */
extern char* kdk_display_get_width();

/**
 * @brief 获取显卡的频率
 * 
 * @return char* 显卡的频率
 */
extern char* kdk_display_get_clock();

/**
 * @brief 获取显卡的功能
 * 
 * @return char* 显卡的功能
 */
extern char* kdk_display_get_capabilities();

/**
 * @brief 获取显卡的配置
 * 
 * @return char* 显卡的配置
 */
extern char* kdk_display_get_configuration();

/**
 * @brief 获取显卡的资源
 * 
 * @return char* 显卡的资源
 */
extern char* kdk_display_get_resources();

#ifdef __cplusplus
}
#endif

#endif