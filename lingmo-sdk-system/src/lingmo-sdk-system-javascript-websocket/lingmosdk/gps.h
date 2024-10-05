#ifndef GPS_H
#define GPS_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include "lingmosdkdbus.h"

class Gps : public QObject
{
    Q_OBJECT
public:
    explicit Gps(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    QString getGPSInfo();


signals:
    void sendText(const QString &text);
};

#endif // GPS_H
