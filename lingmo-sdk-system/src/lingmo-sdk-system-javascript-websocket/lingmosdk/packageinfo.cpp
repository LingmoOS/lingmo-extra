#include "packageinfo.h"

Packageinfo::Packageinfo(QObject *parent) : QObject(parent)
{
   dbusConnect = -1;
   ServerName = "com.lingmo.lingmosdk.service";
   ServerInterface = "com.lingmo.lingmosdk.packageinfo";
   ServerPath = "/com/lingmo/lingmosdk/packageinfo";
}

QStringList Packageinfo::getPackageList()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getPackageList") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
}

QString Packageinfo::getPackageVersion(QString packageName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getPackageVersion", packageName) ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

int Packageinfo::getPackageInstalled(QString packageName, QString versionName)
{
    QVariant res = kdbus.dbusSendpara2(ServerName,ServerPath,ServerInterface,"getPackageInstalled", packageName, versionName) ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toInt()));
    return res.toInt();
}
