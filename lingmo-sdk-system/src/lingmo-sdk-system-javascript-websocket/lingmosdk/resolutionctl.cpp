#include "resolutionctl.h"

Resolutionctl::Resolutionctl(QObject *parent) : QObject(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerPath = "/com/lingmo/lingmosdk/resolution";
    ServerInterface = "com.lingmo.lingmosdk.resolution";
}



QStringList Resolutionctl::getSysLegalResolution()
{
    QVariant res = kdbus.dbusSend2(ServerName,ServerPath,ServerInterface,"getSysLegalResolution") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
}
