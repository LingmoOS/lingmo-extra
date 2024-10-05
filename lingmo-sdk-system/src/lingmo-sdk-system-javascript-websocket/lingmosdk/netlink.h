#ifndef NETLINK_H
#define NETLINK_H

#include <QWidget>
#include "lingmosdkdbus.h"

class NetLink : public QObject
{
    Q_OBJECT
public:
    explicit NetLink(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

public slots:
    unsigned int getNetState();

signals:
    void sendText(const QString &text);
};

#endif // NETLINK_H
