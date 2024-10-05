#include "cpuinfo.h"

Cpuinfo::Cpuinfo(QWidget *parent) : QWidget(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerPath = "/com/lingmo/lingmosdk/cpuinfo";
    ServerInterface = "com.lingmo.lingmosdk.cpuinfo";
}

Cpuinfo::Cpuinfo(QString urlPath)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerPath = "/com/lingmo/lingmosdk/cpuinfo";
    ServerInterface = "com.lingmo.lingmosdk.cpuinfo";
    path = urlPath;
    path_list = urlPath.split("?");
}

QJsonObject Cpuinfo::deal_cpuinfo()
{
    qDebug()<<"cpuinfo url path is "<<path;
    return deal_cpuinfo_path();
}

QJsonObject Cpuinfo::deal_cpuinfo_path()
{
    QJsonArray jarray;
    QJsonObject json;
    if(path == "/cpuinfo/getCpuArch")
    {
        QString res = getCpuArch();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/cpuinfo/getCpuVendor")
    {
        QString res = getCpuVendor();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/cpuinfo/getCpuModel")
    {
        QString res = getCpuModel();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/cpuinfo/getCpuFreqMHz")
    {
        QString res = getCpuFreqMHz();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/cpuinfo/getCpuCorenums")
    {
        int res = getCpuCorenums();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/cpuinfo/getCpuVirt")
    {
        QString res = getCpuVirt();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/cpuinfo/getCpuProcess")
    {
        int res = getCpuProcess();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
}

QString Cpuinfo::getCpuArch()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getCpuArch");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QString Cpuinfo::getCpuVendor()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getCpuVendor");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QString Cpuinfo::getCpuModel()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getCpuModel");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QString Cpuinfo::getCpuFreqMHz()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getCpuFreqMHz");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

int Cpuinfo::getCpuCorenums()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getCpuCorenums");
    dbusConnect = kdbus.dbusConn;
    return res.toInt();
}

QString Cpuinfo::getCpuVirt()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getCpuVirt");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

int Cpuinfo::getCpuProcess()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getCpuProcess");
    dbusConnect = kdbus.dbusConn;
    return res.toInt();
}
