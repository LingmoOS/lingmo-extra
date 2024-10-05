#ifndef NET_H
#define NET_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Net : public QWidget
{
    Q_OBJECT
public:
    explicit Net(QWidget *parent = nullptr);
    Net(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

    QJsonObject deal_net();
    QJsonObject deal_net_path();
    QJsonObject deal_net_path_para();
private:
    QStringList path_list;
    QString path;

public slots:
    int getPortState(int port);
    QStringList getGatewayInfo();
    QStringList getMultiplePortStat(int startPort,int endPort);
    QStringList getFirewallState();

signals:

};

#endif // NET_H
