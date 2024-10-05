#ifndef LINGMOOCR_H
#define LINGMOOCR_H

#include <iostream>
#include <string>
#include <vector>

// namespace kdk {

//     class kdkOCR {
//     public:

//         std::vector<std::vector<std::vector<int>>> getRect(const std::string &imagePath);

//         std::vector<std::string> getCls(const std::string &imagePath, int nums);



//     };
// }

namespace kdk
{
    /**
     * @brief ocr的识别结果
     *
     */
    struct TOcrResult
    {
        std::string imagePath;
        volatile bool hasGotRect;
        std::vector<std::vector<std::vector<int>>> textRects;
        std::vector<std::string> texts;

        TOcrResult();

        void reset();

        bool isNewPath(const std::string &strImagePath) const;
    };

    class kdkOCR
    {
    protected:
        TOcrResult objOcrRet;

    public:
        kdkOCR();
        std::vector<std::vector<std::vector<int>>> getRect(const std::string &imagePath);
        std::vector<std::string> getCls(const std::string &imagePath, int nums);

    protected:
        void handleImage(const std::string &imagePath);
    };
}

extern "C" {
        std::vector<std::vector<std::vector<int>>> getRect(const std::string &imagePath) {
            kdk::kdkOCR obj;
            return obj.getRect(imagePath);
        }

        std::vector<std::string> getCls(const std::string &imagePath, int nums) {
                            kdk::kdkOCR obj;
                return obj.getCls(imagePath, nums);
        }
}

#endif //LINGMOOCR_H