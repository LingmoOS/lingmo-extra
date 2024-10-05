#include "sysinfo.h"

Sysinfo::Sysinfo(QObject *parent) : QObject(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerPath = "/com/lingmo/lingmosdk/sysinfo";
    ServerInterface = "com.lingmo.lingmosdk.sysinfo";
}

QString Sysinfo::getSystemArchitecture()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemArchitecture") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemName()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemName") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemVersion(bool vsname)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getSystemVersion",vsname) ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

int Sysinfo::getSystemActivationStatus()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemActivationStatus") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toInt()));
    return res.toInt();
}

QString Sysinfo::getSystemSerialNumber()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemSerialNumber") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemKernelVersion()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemKernelVersion") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemEffectUser()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemEffectUser") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemProjectName()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemProjectName") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemProjectSubName()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemProjectSubName") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

int Sysinfo::getSystemProductFeatures()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemProductFeatures") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toInt()));
    return res.toInt();
}

QString Sysinfo::getSystemHostVirtType()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemHostVirtType") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemHostCloudPlatform()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemHostCloudPlatform") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemOSVersion()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemOSVersion") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Sysinfo::getSystemUpdateVersion()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemUpdateVersion") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

bool Sysinfo::getSystemIsZYJ()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSystemIsZYJ") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toBool()));
    return res.toBool();
}

QStringList Sysinfo::getSysLegalResolution()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getSysLegalResolution") ;
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
}