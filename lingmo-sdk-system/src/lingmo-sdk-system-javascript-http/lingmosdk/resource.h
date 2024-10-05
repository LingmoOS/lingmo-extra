#ifndef RESOURCE_H
#define RESOURCE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Resource : public QWidget
{
    Q_OBJECT
public:
    explicit Resource(QWidget *parent = nullptr);
    Resource(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;
    QJsonObject deal_resource();
    QJsonObject deal_resource_path();
//    QJsonObject deal_resource_path_para();
private:
    QStringList path_list;
    QString path;

signals:

public slots:
    double getMemTotalKiB();
    double getMemUsagePercent();
    double getMemUsageKiB();
    double getMemAvailableKiB();
    double getMemFreeKiB();
    double getMemVirtAllocKiB();
    double getMemSwapTotalKiB();
    double getMemSwapUsagePercent();
    double getMemSwapUsageKiB();
    double getMemSwapFreeKiB();
    double getCpuCurrentUsage();
    QString getUpTime();

};

#endif // RESOURCE_H
