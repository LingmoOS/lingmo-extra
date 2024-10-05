#ifndef BURIEDPOINT_H_
#define BURIEDPOINT_H_

#include <string>
#include <map>

namespace kdk
{

class BuriedPoint
{
public:
    BuriedPoint();
    ~BuriedPoint();

    /**
     * @brief 上传埋点数据
     *
     * @param packageName : 包名
     * @param messageType : 消息类型
     * @param data : 要上传的数据
     *
     * @retval true : 上传成功
     * @retval false : 上传失败
     */
    bool uploadMessage(std::string packageName , std::string messageType , std::map<std::string , std::string> data);

private:
    enum returnState {
        OK = 0,                                        /* 存储成功 */
        InvalidArgumentFormat = 1,                     /* 参数格式错误 */
        InvalidTid = 2,                                /* tid异常 , 但消息存储成功 */
        InvalidUploadedMessageSha256 = 3,              /* shan256异常 */
        InvalidUploadedMessageSha256Decryption = 4,    /* sha256解密异常 */
        InvalidCreateTimeStamp = 5                     /* 时间字段异常 */
    };

    bool checkDir(void);
    std::string getUploadData(std::map<std::string , std::string> &data);
    std::string getCurrentTime(void);
    bool callDbus(const std::string &pkgInfo , const std::string &uploadData , const std::string &uploadMessageSha256);
    std::string readTid(void);
    bool writeTid(std::string tid);
};

}

#endif
