#ifndef BIOS_H
#define BIOS_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Bios : public QWidget
{
    Q_OBJECT
public:
    explicit Bios(QWidget *parent = nullptr);
    Bios(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

    QJsonObject deal_bios();
    QJsonObject deal_bios_path();
    QJsonObject deal_bios_path_para();
private:
    QStringList path_list;
    QString path;

public slots:
    QString getBiosVendor();
    QString getBiosVersion();

signals:

};

#endif // BIOS_H
