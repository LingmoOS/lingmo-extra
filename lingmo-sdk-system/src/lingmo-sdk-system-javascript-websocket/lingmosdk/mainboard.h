#ifndef MAINBOARD_H
#define MAINBOARD_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include "lingmosdkdbus.h"

class Mainboard : public QObject
{
    Q_OBJECT
public:
    explicit Mainboard(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

public slots:
    QString getMainboardName();
    QString getMainboardDate();
    QString getMainboardSerial();
    QString getMainboardVendor();

signals:
    void sendText(const QString &text);

};

#endif // MAINBOARD_H
