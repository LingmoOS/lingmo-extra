#ifndef NET_H
#define NET_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include "lingmosdkdbus.h"

class Net : public QObject
{
    Q_OBJECT
public:
    explicit Net(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

public slots:
    int getPortState(QString port);
    QStringList getMultiplePortStat(QString, QString);
    QStringList getGatewayInfo();
    QStringList getFirewallState(void);

signals:
    void sendText(const QString &text);
};

#endif // NET_H
