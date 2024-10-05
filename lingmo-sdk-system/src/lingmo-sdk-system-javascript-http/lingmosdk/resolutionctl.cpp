#include "resolutionctl.h"

Resolutionctl::Resolutionctl(QWidget *parent) : QWidget(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerPath = "/com/lingmo/lingmosdk/resolution";
    ServerInterface = "com.lingmo.lingmosdk.resolution";
}



QStringList Resolutionctl::getSysLegalResolution()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSysLegalResolution") ;
    dbusConnect = kdbus.dbusConn;
    return res.toStringList();
}
