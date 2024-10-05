#include "peripheralsenum.h"

Peripheralsenum::Peripheralsenum(QObject *parent) : QObject(parent)
{
   dbusConnect = -1;
   ServerName = "com.lingmo.lingmosdk.service";
   ServerInterface = "com.lingmo.lingmosdk.peripheralsenum";
   ServerPath = "/com/lingmo/lingmosdk/peripheralsenum";
}



QStringList Peripheralsenum::getAllUsbInfo()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getAllUsbInfo") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
}
