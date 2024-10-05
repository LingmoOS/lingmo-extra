#ifndef LIBKYFAN_H
#define LIBKYFAN_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 获取风扇的信息
 * 
 * @return char** 风扇的信息（名称，转速）
 */
extern char** kdk_fan_get_information();

/**
 * @brief 用于回收字符串列表
 * 
 * @param ptr 字符串列表
 */
extern inline void kdk_fan_freeall(char **ptr);

#ifdef __cplusplus
}
#endif

#endif