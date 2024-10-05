#ifndef MAINBOARD_H
#define MAINBOARD_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Mainboard : public QWidget
{
    Q_OBJECT
public:
    explicit Mainboard(QWidget *parent = nullptr);
    Mainboard(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;
    QJsonObject deal_mainboard();
    QJsonObject deal_mainboard_path();
private:
    QStringList path_list;
    QString path;

public slots:
    QString getMainboardName();
    QString getMainboardDate();
    QString getMainboardSerial();
    QString getMainboardVendor();

signals:

};

#endif // MAINBOARD_H
