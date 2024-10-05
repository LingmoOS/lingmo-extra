#include <stdint.h>
#include <string>

namespace kdk
{

class LockScreen
{
public:
    LockScreen();
    ~LockScreen();

    /**
     * @brief 设置禁止锁屏
     * 
     * @param appName : 应用名
     * @param reason : 原因描述 
     * 
     * @return 标记
     */
    uint32_t setInhibitLockScreen(std::string appName , std::string reason);

    /**
     * @brief 取消禁止锁屏
     * 
     * @param flag : 禁止锁屏接口返回来的标记
     * 
     * @return true : 成功 , false : 失败
     */
    bool unInhibitLockScreen(uint32_t flag);
};

}