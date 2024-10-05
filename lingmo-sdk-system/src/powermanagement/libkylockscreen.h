#ifndef POWERMANAGEMENT_LIBKYLOCKSCREEN_H_
#define POWERMANAGEMENT_LIBKYLOCKSCREEN_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 设置禁止锁屏 
 * 
 * @param appName : 应用名
 * @param reason : 禁止锁屏原因
 * 
 * @return 0 : 失败 , >0 : 成功
 */
extern uint32_t kdk_set_inhibit_lockscreen(const char *appName , const char *reason);

/**
 * @brief 取消禁止锁屏 
 * 
 * @param flag : 禁止锁屏接口的返回值
 * 
 * @retval 0 : 成功
 * @retval -1 : 失败
 */
extern int kdk_un_inhibit_lockscreen(uint32_t flag);

#ifdef __cplusplus
}
#endif

#endif