#include "netlink.h"

NetLink::NetLink(QWidget *parent) : QWidget(parent)
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
    return res.toUInt();
}


