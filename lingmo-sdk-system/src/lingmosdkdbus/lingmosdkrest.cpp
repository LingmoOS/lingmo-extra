#include "lingmosdkrest.h"
#include "../systeminfo/libkysysinfo.h" // 分辨率

KySdkRest::KySdkRest(QObject *parent) : QObject(parent)
{

}
QStringList KySdkRest::getSysLegalResolution() const
{
    QStringList res = {};
    char **power = kdk_system_get_resolving_power();
    size_t count = 0;
    while(power[count])
    {
        res << QString::fromLocal8Bit(power[count]);
        count ++;
    }
    kdk_resolving_freeall(power);
    return res;
}
