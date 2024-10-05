#ifndef RUNINFO_H
#define RUNINFO_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Runinfo : public QWidget
{
    Q_OBJECT
public:
    explicit Runinfo(QWidget *parent = nullptr);
    Runinfo(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;
    QJsonObject deal_runinfo();
    QJsonObject deal_runinfo_path();
    QJsonObject deal_runinfo_path_para();
private:
    QStringList path_list;
    QString path;


public slots:
//    double getIoSpeed();
    double getNetSpeed(QString netname);
//    double getMainboardTemperature();
    double getCpuTemperature();
    double getDiskTemperature( QString diskpath);
    int getDiskRate(QString diskpath);

signals:

};

#endif // RUNINFO_H
