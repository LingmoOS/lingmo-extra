#ifndef KYSDKDBUS_H
#define KYSDKDBUS_H
#include <QString>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include <QDBusConnection>


class Kysdkdbus
{
public:
    Kysdkdbus();
    QString ServerName;
    QString ServerInterface;
    QString ServerPath;
    int dbusConn;

    QVariant dbusSend(QString SName, QString SPath, QString SInterface, QString method);
    QVariant dbusSend2(QString SName, QString SPath, QString SInterface, QString method);
    QVariant dbusSendpara(QString SName, QString SPath, QString SInterface, QString method, QVariant para);
    QVariant dbusSendpara2(QString SName, QString SPath, QString SInterface, QString method, QVariant para1, QVariant para2);
    QVariant dbusSendpara4(QString SName, QString SPath, QString SInterface, QString method, QVariant para1, QVariant para2, QVariant para3, QVariant para4);
};

#endif // KYSDKDBUS_H
