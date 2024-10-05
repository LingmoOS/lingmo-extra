#ifndef LIBKYEDID_H
#define LIBKYEDID_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 获取显示器的当前接口
 * 
 * @return char** 显示器的当前接口
 */
extern char** kdk_edid_get_interface();

/**
 * @brief 获取显示器的伽马值
 * 
 * @param name 显示器的当前接口
 * @return float 显示器的伽马值
 */
extern float kdk_edid_get_gamma(char *name);

/**
 * @brief 获取显示器的屏幕尺寸（英寸）
 * 
 * @param name 显示器的当前接口
 * @return float 显示器的屏幕尺寸（英寸）
 */
extern float kdk_edid_get_size(char *name);

/**
 * @brief 获取显示器的最大分辨率
 * 
 * @param name 显示器的当前接口
 * @return char* 显示器的最大分辨率
 */
extern char* kdk_edid_get_max_resolution(char *name);

/**
 * @brief 获取显示器的显示器型号
 * 
 * @param name 显示器的当前接口
 * @return char* 显示器的显示器型号
 */
extern char* kdk_edid_get_model(char *name);

/**
 * @brief 获取显示器的可视面积
 * 
 * @param name 显示器的当前接口
 * @return char* 显示器的可视面积
 */
extern char* kdk_edid_get_visible_area(char *name);

/**
 * @brief 获取显示器的厂商
 * 
 * @param name 显示器的当前接口
 * @return char* 显示器的厂商
 */
extern char* kdk_edid_get_manufacturer(char *name);

/**
 * @brief 获取显示器的生产日期/周
 * 
 * @param name 显示器的当前接口
 * @return char* 显示器的生产日期/周
 */
extern int kdk_edid_get_week(char *name);

/**
 * @brief 获取显示器的生产日期/年
 * 
 * @param name 显示器的当前接口
 * @return char* 显示器的生产日期/年
 */
extern int kdk_edid_get_year(char *name);

/**
 * @brief 获取是否是主显示器（是/否）
 * 
 * @param name 显示器的当前接口
 * @return int 是否是主显示器，1为是，0为否
 */
extern int kdk_edid_get_primary(char *name);

/**
 * @brief 获取分辨率
 * 
 * @param name 显示器的当前接口
 * @return char* 分辨率
 */
extern char* kdk_edid_get_resolution(char *name);

/**
 * @brief 获取图像高宽比
 * 
 * @param name 显示器的当前接口
 * @return char* 图像高宽比
 */
extern char* kdk_edid_get_ratio(char *name);

/**
 * @brief 获取显示器edid未解析的字符串
 * 
 * @param name 显示器的当前接口
 * @return char* edid未解析的字符串
 */
extern char* kdk_edid_get_character(char *name);

/**
 * @brief 用于回收字符串列表
 * 
 * @param ptr 字符串列表
 */
extern inline void kdk_edid_freeall(char **ptr);


#ifdef __cplusplus
}
#endif

#endif
