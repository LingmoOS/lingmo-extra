#include "netlink.h"

NetLink::NetLink(QObject *parent) : QObject(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerInterface = "com.lingmo.lingmosdk.netLink";
    ServerPath = "/com/lingmo/lingmosdk/netlink";
}

unsigned int NetLink::getNetState()
{

    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getNetState");
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(QString::number(res.toUInt())));
    return res.toUInt();
}


