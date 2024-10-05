/**
 * @file kyutils.h
 * @brief 
 * @author liuyang <liuyang@lingmoos.cn>
 * @version 1.0
 * @date 2021-09-01
 * @example lingmosdk-base/src/utils/sample/kyutils_sample.c
 * 
 * @copyright Copyright: 2021,LingmoSoft Co.,Ltd.
 * 
 */

#ifndef KYSDK_UTILS_H__
#define KYSDK_UTILS_H__

#include <stdint.h>
#include <ctype.h>
#include <inttypes.h>
#include <bits/types.h>
#include <locale.h>
#include <errno.h>
#include <stddef.h>
#include <sys/syslog.h>

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup 单位进制转换组件
 * @{ 
 */


/** KDKUtilsErrCode */
typedef enum{
    KDK_NOERR,                      /**< 无异常 */
    KDK_INVAILD_BASE,               /**< 非法进制 */
    KDK_INVAILD_ARGUMENT,           /**< 非法参数 */
    KDK_INVAILD_DATA_FORMAT,        /**< 非法数据格式 */
    KDK_SYSTEM_UNKOWNERR,           /**< 系统运行异常引发的未知错误 */
}KDKUtilsErrCode;

/** KDKVolumeBaseType */
typedef enum{
    KDK_KILOBYTE = 1,               /**< KB */
    KDK_MEGABYTE,                   /**< MB */
    KDK_GIGABYTE,                   /**< GB */
    KDK_TERABYTE,                   /**< TB */
    KDK_PETABYTE,                   /**< PB */
    KDK_EXABYTE,                    /**< EB */
    KDK_KILO,                       /**< K */
    KDK_MEGA,                       /**< M */
    KDK_GIGA,                       /**< G */
    KDK_TERA,                       /**< T */
    KDK_PETA,                       /**< P */
    KDK_EXA,                        /**< E */
}KDKVolumeBaseType;


/**
 * @brief 字符格式单位进制转换
 * @param[in]   origin_data      原始字符类型的数据，以具体进制单位结束，如果没有，缺省为‘B’
 * @param[in]   result_base      期望的结果进制单位
 * @param[out]  result_data      转化进制后的字符数据，带进制单位
 * @return int
 * 
 */

extern int kdkVolumeBaseCharacterConvert(const char* origin_data, KDKVolumeBaseType result_base, char* result_data);


/**
 * @brief 数字格式单位进制转换
 * @param[in]   origin_numerical 原始数字类型数据
 * @param[in]   origin_base      原始的进制单位
 * @param[in]   result_base      期望的进制单位
 * @param[out]  result_numerical 期望进制下的数字类型数据
 * @return int 
 */

extern int kdkVolumeBaseNumericalConvert(double origin_numerical, KDKVolumeBaseType origin_base, KDKVolumeBaseType result_base, double* result_numerical);

/** 
 * @} 
 */


#ifdef __cplusplus
}
#endif

#endif