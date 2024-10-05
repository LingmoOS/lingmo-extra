#include "mainboard.h"

Mainboard::Mainboard(QWidget *parent) : QWidget(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerInterface = "com.lingmo.lingmosdk.mainboard";
    ServerPath = "/com/lingmo/lingmosdk/mainboard";
}

Mainboard::Mainboard(QString urlPath)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerInterface = "com.lingmo.lingmosdk.mainboard";
    ServerPath = "/com/lingmo/lingmosdk/mainboard";
    path = urlPath;
    path_list = urlPath.split("?");
}

QJsonObject Mainboard::deal_mainboard()
{
    qDebug()<<"mainboard url path is "<<path;
    return deal_mainboard_path();
}

QJsonObject Mainboard::deal_mainboard_path()
{
    QJsonArray jarray;
    QJsonObject json;
    if(path == "/mainboard/getMainboardName")
    {
        QString res = getMainboardName();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/mainboard/getMainboardDate")
    {
        QString res = getMainboardDate();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/mainboard/getMainboardSerial")
    {
        QString res = getMainboardSerial();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path == "/mainboard/getMainboardVendor")
    {
        QString res = getMainboardVendor();
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
}


QString Mainboard::getMainboardName()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMainboardName");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QString Mainboard::getMainboardDate()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMainboardDate");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QString Mainboard::getMainboardSerial()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMainboardSerial");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QString Mainboard::getMainboardVendor()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getMainboardVendor");
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}
