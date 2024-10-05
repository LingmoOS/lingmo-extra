#include "process.h"

Process::Process(QObject *parent) : QObject(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerInterface = "com.lingmo.lingmosdk.process";
    ServerPath = "/com/lingmo/lingmosdk/process";

}


double Process::getProcInfoCpuUsage(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoCpuUsage",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(QString::number(res.toDouble())));
    return res.toDouble();
}

// double Process::getProcInfoIoUsage(int pid)
// {
//     QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoIoUsage",pid);
//     dbusConnect = kdbus.dbusConn;
//     sendText(QString("%1").arg(QString::number(res.toDouble())));
//     return res.toDouble();
// }

double Process::getProcInfoMemUsage(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoMemUsage",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(QString::number(res.toDouble())));
    return res.toDouble();
}

QString Process::getProcInfoStatus(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoStatus",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

int Process::getProcInfoPort(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoPort",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(QString::number(res.toInt())));
    return res.toInt();
}

QString Process::getProcInfoStartTime(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoStartTime",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Process::getProcInfoRunningTime(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoRunningTime",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Process::getProcInfoCpuTime(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoCpuTime",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Process::getProcInfoCmd(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoCmd",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QString Process::getProcInfoUser(int pid)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfoUser",pid);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toString()));
    return res.toString();
}

QStringList Process::getProcInfo(QString procName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getProcInfo",procName);
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
}

QStringList Process::getAllProcInfo()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getAllProcInfo");
    dbusConnect = kdbus.dbusConn;
    sendText(QString("%1").arg(res.toStringList().join(",")));
    return res.toStringList();
}
