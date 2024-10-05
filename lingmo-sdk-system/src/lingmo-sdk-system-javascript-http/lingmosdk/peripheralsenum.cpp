#include "peripheralsenum.h"

Peripheralsenum::Peripheralsenum(QWidget *parent) : QWidget(parent)
{

}

Peripheralsenum::Peripheralsenum(QString urlPath)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerInterface = "com.lingmo.lingmosdk.peripheralsenum";
    ServerPath = "/com/lingmo/lingmosdk/peripheralsenum";
    path = urlPath;
    path_list = urlPath.split("?");
}

QJsonObject Peripheralsenum::deal_peripheralsenum()
{
    qDebug()<<"mainboard url path is "<<path;
    return deal_peripheralsenum_path();
}

QJsonObject Peripheralsenum::deal_peripheralsenum_path()
{
    QJsonArray jarray;
    QJsonObject json;
    if(path == "/peripheralsenum/getAllUsbInfo")
    {
        QStringList res = getAllUsbInfo();
        for(int i=0;i<res.size();i++)
        {
            jarray.append(res[i]);
        }
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",jarray);
        return json;
    }
}

QStringList Peripheralsenum::getAllUsbInfo()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getAllUsbInfo");
    dbusConnect = kdbus.dbusConn;
    return res.toStringList();
}
