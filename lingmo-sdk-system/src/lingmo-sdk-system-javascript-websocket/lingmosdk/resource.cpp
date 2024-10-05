#include "resource.h"

Resource::Resource(QObject *parent) : QObject(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerPath = "/com/lingmo/lingmosdk/resource";
    ServerInterface = "com.lingmo.lingmosdk.resource";
}

double Resource::getMemTotalKiB()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemTotalKiB") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemUsagePercent()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemUsagePercent") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemUsageKiB()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemUsageKiB") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemAvailableKiB()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemAvailableKiB") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemFreeKiB()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemFreeKiB") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemVirtAllocKiB()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemVirtAllocKiB") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemSwapTotalKiB()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemSwapTotalKiB") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemSwapUsagePercent()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemSwapUsagePercent") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemSwapUsageKiB()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemSwapUsageKiB") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getMemSwapFreeKiB()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMemSwapFreeKiB") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

double Resource::getCpuCurrentUsage()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getCpuCurrentUsage") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toDouble()));
    return res.toDouble();
}

QString Resource::getUpTime()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getUpTime") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}