#ifndef	KYSDK_BASE_GSETTINGS_H__
#define	KYSDK_BASE_GSETTINGS_H__


/** @defgroup Gsettings
  * @{
  */


/**
 * @file libkygseetings.h
 * @author zhouhuazhi (zhouhuazhi@lingmoos.cn)
 * @brief KYSDK C语言Gsettings模块
 * @version 0.1
 * @date 2022-10-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */



#ifdef	__cplusplus
extern "C"
{

#endif
/**
 * @brief 设置gesettings指定key值
 * 
 * @return int 成功返回1
 */
extern int kdk_gsettings_set(const char *schema_id, const char *key, const char *format, ...);

/**
 * @brief 重置gesettings指定key值
 * 
 * @return int 成功返回0
 */
extern int	kdk_settings_reset(const char *schema_id, const char *key);

/**
 * @brief 设置gesettings指定string类型key值
 * 
 * @return int 成功返回0
 */
extern int kdk_settings_set_string(const char *schema_id, const char *key, const char *value);

/**
 * @brief 设置gesettings指定int类型key值
 * 
 * @return int 成功返回0
 */
extern int kdk_settings_set_int(const char *schema_id, const char *key, int value);

/**
 * @brief 获取gesettings指定key值
 * 
 * @return GVariant key的值
 */
void* kdk_gsettings_get(const char *schema_id, const char *key, const char *format, ...);

/**
 * @brief 获取gesettings指定string类型key值
 * 
 * @return char* key的值
 */
extern char* kdk_settings_get_string(const char *schema_id, const char *key);

/**
 * @brief 获取gesettings指定int类型key值
 * 
 * @return int key的值
 */
extern int kdk_settings_get_int(const char *schema_id, const char *key);

/**
 * @brief 获取gesettings指定double类型key值
 * 
 * @return int key的值
 */
extern double kdk_settings_get_double(const char *schema_id, const char *key);

#ifdef	__cplusplus
}
#endif

#endif	//KYSDK_BASE_GSETTINGS_H__
