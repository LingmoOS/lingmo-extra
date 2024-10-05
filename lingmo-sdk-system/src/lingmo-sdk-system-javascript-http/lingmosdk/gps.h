#ifndef GPS_H
#define GPS_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Gps : public QWidget
{
    Q_OBJECT
public:
    explicit Gps(QWidget *parent = nullptr);
    Gps(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;
    QJsonObject deal_gps();
    QJsonObject deal_gps_path();
private:
    QStringList path_list;
    QString path;

public slots:
    QString getGPSInfo();


signals:

};

#endif // GPS_H
