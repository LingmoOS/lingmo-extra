#include "bios.h"

Bios::Bios(QWidget *parent) : QWidget(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerPath = "/com/lingmo/lingmosdk/bios";
    ServerInterface = "com.lingmo.lingmosdk.bios";

}

Bios::Bios(QString urlPath)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerPath = "/com/lingmo/lingmosdk/bios";
    ServerInterface = "com.lingmo.lingmosdk.bios";
    path = urlPath;
    path_list = urlPath.split("?");
}

QJsonObject Bios::deal_bios()
{
    qDebug()<<"bios url path is "<<path;

    if(path_list.size()>1)
    {
        return deal_bios_path_para();
    }else{
        return deal_bios_path();
    }
}

QJsonObject Bios::deal_bios_path()
{
    QJsonArray jarray;
    QJsonObject json;
    if(path == "/bios/getBiosVendor")
    {
        QString res = getBiosVendor();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/bios/getBiosVersion")
    {
        QString res = getBiosVersion();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
}

QJsonObject Bios::deal_bios_path_para()
{
    QJsonObject json;
    return json;
}

QString Bios::getBiosVendor()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getBiosVendor");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QString Bios::getBiosVersion()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getBiosVersion");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

