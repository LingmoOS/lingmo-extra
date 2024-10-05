#include "netcard.h"

NetCard::NetCard(QWidget *parent) : QWidget(parent)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerInterface = "com.lingmo.lingmosdk.netcard";
    ServerPath = "/com/lingmo/lingmosdk/netcard";
}

NetCard::NetCard(QString urlPath)
{
    dbusConnect = -1;
    ServerName = "com.lingmo.lingmosdk.service";
    ServerInterface = "com.lingmo.lingmosdk.netcard";
    ServerPath = "/com/lingmo/lingmosdk/netcard";
    path = urlPath;
    path_list = urlPath.split("?");
}

QJsonObject NetCard::deal_netcard()
{
    qDebug()<<"netcard url path is "<<path;

    if(path_list.size()>1)
    {
        return deal_netcard_path_para();
    }else{
        return deal_netcard_path();
    }
}

QJsonObject NetCard::deal_netcard_path()
{
    QJsonArray jarray;
    QJsonObject json;
    if(path == "/netcard/getNetCardName")
    {
        QStringList res = getNetCardName();
        for(int i=0;i<res.size();i++)
        {
            jarray.append(res[i]);
        }
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",jarray);
        return json;
    }
    else if(path == "/netcard/getNetCardUpcards")
    {
        QStringList res = getNetCardUpcards();
        for(int i=0;i<res.size();i++)
        {
            jarray.append(res[i]);
        }
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",jarray);
        return json;
    }
}

QJsonObject NetCard::deal_netcard_path_para()
{
    QJsonArray jarray;
    QJsonObject json;
    QString netcardName = path_list[1];
    netcardName =netcardName.split("=").last();
    qDebug()<<"netcard name is "<<netcardName;
    if(path_list[0] == "/netcard/getNetCardUp")
    {
        int res = getNetCardUp(netcardName);
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path_list[0] == "/netcard/getNetCardPhymac")
    {
        QString res = getNetCardPhymac(netcardName);
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path_list[0] == "/netcard/getNetCardPrivateIPv4")
    {
        QString res = getNetCardPrivateIPv4(netcardName);
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path_list[0] == "/netcard/getNetCardIPv4")
    {
        QStringList res = getNetCardIPv4(netcardName);
        for(int i=0;i<res.size();i++)
        {
            jarray.append(res[i]);
        }
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",jarray);
        return json;
    }
    else if(path_list[0] == "/netcard/getNetCardPrivateIPv6")
    {
        QString res = getNetCardPrivateIPv6(netcardName);
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path_list[0] == "/netcard/getNetCardType")
    {
        int res = getNetCardType(netcardName);
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",res);
        return json;
    }
    else if(path_list[0] == "/netcard/getNetCardProduct")
    {
        QStringList res = getNetCardProduct(netcardName);
        for(int i=0;i<res.size();i++)
        {
            jarray.append(res[i]);
        }
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",jarray);
        return json;
    }
    else if(path_list[0] == "/netcard/getNetCardIPv6")
    {
        QStringList res = getNetCardIPv6(netcardName);
        for(int i=0;i<res.size();i++)
        {
            jarray.append(res[i]);
        }
        json.insert("Result",dbusConnect);
        json.insert("ResultMessage",jarray);
        return json;
    }
}

QStringList NetCard::getNetCardName()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getNetCardName") ;
    dbusConnect = kdbus.dbusConn;
    return res.toStringList();
}

int NetCard::getNetCardType(QString NetCardName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getNetCardType",NetCardName) ;
    dbusConnect = kdbus.dbusConn;
    return res.toInt();
}

QStringList NetCard::getNetCardProduct(QString NetCardName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getNetCardProduct",NetCardName) ;
    dbusConnect = kdbus.dbusConn;
    return res.toStringList();
}

int NetCard::getNetCardUp(QString NetCardName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getNetCardUp",NetCardName) ;
    dbusConnect = kdbus.dbusConn;
    return res.toInt();
}

QStringList NetCard::getNetCardUpcards()
{
    QVariant res = kdbus.dbusSend(ServerName,ServerPath,ServerInterface,"getNetCardUpcards") ;
    dbusConnect = kdbus.dbusConn;
    return res.toStringList();
}

QString NetCard::getNetCardPhymac(QString NetCardName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getNetCardPhymac",NetCardName) ;
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QString NetCard::getNetCardPrivateIPv4(QString NetCardName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getNetCardPrivateIPv4",NetCardName) ;
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QStringList NetCard::getNetCardIPv4(QString NetCardName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getNetCardIPv4",NetCardName) ;
    dbusConnect = kdbus.dbusConn;
    return res.toStringList();
}

QString NetCard::getNetCardPrivateIPv6(QString NetCardName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getNetCardPrivateIPv6",NetCardName) ;
    dbusConnect = kdbus.dbusConn;
    return res.toString();
}

QStringList NetCard::getNetCardIPv6(QString NetCardName)
{
    QVariant res = kdbus.dbusSendpara(ServerName,ServerPath,ServerInterface,"getNetCardIPv6",NetCardName) ;
    dbusConnect = kdbus.dbusConn;
    return res.toStringList();
}
