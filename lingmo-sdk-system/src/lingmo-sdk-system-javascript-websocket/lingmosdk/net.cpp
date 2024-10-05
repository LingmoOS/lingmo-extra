#include "net.h"

Net::Net(QObject *parent) : QObject(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerInterface = "com.lingmo.lingmosdk.net";
    ServerPath = "/com/lingmo/lingmosdk/net";
}


int Net::getPortState(QString port)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getPortState",port);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(QString::number(res.toInt())));
    return res.toInt();
}

QStringList Net::getMultiplePortStat(QString beginport, QString endport)
{
    QString port = beginport + "," + endport;
    qDebug() << "getMultiplePortStat" << "port" << port;
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getMultiplePortStat",port);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
}

QStringList Net::getGatewayInfo()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getGatewayInfo");
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
}

 QStringList Net::getFirewallState(void)
 {
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getFirewallState");
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
 }
