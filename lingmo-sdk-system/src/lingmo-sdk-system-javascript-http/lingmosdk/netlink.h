#ifndef NETLINK_H
#define NETLINK_H

#include <QWidget>
#include "lingmosdkdbus.h"

class NetLink : public QWidget
{
    Q_OBJECT
public:
    explicit NetLink(QWidget *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

public slots:
    unsigned int getNetState();

signals:

};

#endif // NETLINK_H
