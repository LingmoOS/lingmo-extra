#include "lingmosdkdbus.h"

Kysdkdbus::Kysdkdbus()
{
    dbusConn = -1;
}

QVariant Kysdkdbus::dbusSend(QString SName, QString SPath, QString SInterface, QString method)
{
    QVariant res;
    QDBusInterface interface (SName,SPath,SInterface,QDBusConnection::systemBus());
    if(!interface.isValid())
    {
       res = "dbus 连接失败";
       return res;
    }
    QDBusMessage reply = interface.call(method);
    qDebug() << "reply:" << reply;
    if(reply.arguments()[0].isValid())
    {
        qDebug()<<"连接成功";
        dbusConn = 0;
        res=reply.arguments()[0];
        return res;
    }
}

QVariant Kysdkdbus::dbusSend2(QString SName, QString SPath, QString SInterface, QString method)
{
    QVariant res;
    QDBusInterface interface (SName,SPath,SInterface,QDBusConnection::sessionBus());
    if(!interface.isValid())
    {
       res = "dbus 连接失败";
       return res;
    }
    QDBusMessage reply = interface.call(method);
    qDebug() << "reply:" << reply;
    if(reply.arguments()[0].isValid())
    {
        qDebug()<<"连接成功";
        dbusConn = 0;
        res=reply.arguments()[0];
        return res;
    }
}

QVariant Kysdkdbus::dbusSendpara(QString SName, QString SPath, QString SInterface, QString method, QVariant para)
{
    QVariant res;
    QDBusInterface interface (SName,SPath,SInterface,QDBusConnection::systemBus());
    if(!interface.isValid())
    {
       res = "dbus 连接失败";
       return res;
    }
    QDBusMessage reply = interface.call(method,para);
    qDebug() << "reply:" << reply;
    if(reply.arguments()[0].isValid())
    {
        qDebug()<<"连接成功";
        dbusConn = 0;
        res=reply.arguments()[0];
        return res;
    }
}

QVariant Kysdkdbus::dbusSendpara2(QString SName, QString SPath, QString SInterface, QString method, QVariant para1, QVariant para2)
{
    QVariant res;
    QDBusInterface interface (SName,SPath,SInterface,QDBusConnection::systemBus());
    if(!interface.isValid())
    {
       res = "dbus 连接失败";
       return res;
    }
    QDBusMessage reply = interface.call(method,para1, para2);
    qDebug() << "reply:" << reply;
    if(reply.arguments()[0].isValid())
    {
        qDebug()<<"连接成功";
        dbusConn = 0;
        res=reply.arguments()[0];
        return res;
    }
}

QVariant Kysdkdbus::dbusSendpara4(QString SName, QString SPath, QString SInterface, QString method, QVariant para1, QVariant para2, QVariant para3, QVariant para4)
{
    QVariant res;
    QDBusInterface interface (SName,SPath,SInterface,QDBusConnection::systemBus());
    if(!interface.isValid())
    {
       res = "dbus 连接失败";
       return res;
    }
    QDBusMessage reply = interface.call(method,para1,para2, para3, para4);
    qDebug() << "reply:" << reply;
    if(reply.arguments()[0].isValid())
    {
        qDebug()<<"连接成功";
        dbusConn = 0;
        res=reply.arguments()[0];
        return res;
    }
}
