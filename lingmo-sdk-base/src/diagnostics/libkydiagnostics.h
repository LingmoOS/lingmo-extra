#ifndef DIAGNOSTICS_KYDIAGNOSTICS_H_
#define DIAGNOSTICS_KYDIAGNOSTICS_H_

typedef struct {
    const char *key;
    const char *value;
} KBuriedPoint;

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 上传埋点数据 
 * 
 * @param appName : 包名
 * @param messageType : 消息类型
 * @param data : 埋点数据数组
 * @param length : 数组长度
 * 
 * @retval 0 : 成功
 * @retval -1 : 失败
 */
extern int kdk_buried_point(char *appName , char *messageType , KBuriedPoint *data , int length);

#ifdef __cplusplus
}
#endif

#endif