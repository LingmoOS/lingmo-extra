#include "buriedpoint.h"
#include "libkydiagnostics.h"

int kdk_buried_point(char *appName , char *messageType , KBuriedPoint *data , int length)
{
    std::map<std::string , std::string> uploadData;
    for (int i = 0 ; i < length ; i++) {
        std::string key = data[i].key;
        std::string value = data[i].value;
        uploadData[key] = value;
    }

    kdk::BuriedPoint buriedPoint;
    if (buriedPoint.uploadMessage(appName , messageType , uploadData)) {
        return 0;
    }

    return -1;
}