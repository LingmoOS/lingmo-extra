#ifndef CPUINFO_H
#define CPUINFO_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Cpuinfo : public QWidget
{
    Q_OBJECT
public:
    explicit Cpuinfo(QWidget *parent = nullptr);
    Cpuinfo(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

    QJsonObject deal_cpuinfo();
    QJsonObject deal_cpuinfo_path();
//    QJsonObject deal_cpuinfo_path_para();
private:
    QStringList path_list;
    QString path;
public slots:
    QString getCpuArch();
    QString getCpuVendor();
    QString getCpuModel();
    QString getCpuFreqMHz();
    int getCpuCorenums();
    QString getCpuVirt();
    int getCpuProcess();
signals:

};

#endif // CPUINFO_H
