#ifndef DISK_H
#define DISK_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include <QJsonObject>
#include <QJsonArray>
#include "lingmosdkdbus.h"

class Disk : public QWidget
{
    Q_OBJECT
public:
    Disk(QString urlPath);
    Disk();
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface ;
    QString ServerPath ;

    QJsonObject deal_disk();
    QJsonObject deal_disk_path();
    QJsonObject deal_disk_path_para();
private:
    QStringList path_list;
    QString path;

public slots:
    QString getDiskType(QString DiskName);
    QString getDiskVersion(QString DiskName);
    unsigned int getDiskSpeed(QString DiskName);
    unsigned long long getDiskSectorNum(QString DiskName);
    QStringList getDiskList();
    int getDiskSectorSize(QString DiskName);
    double getDiskTotalSizeMiB(QString DiskName);
    QString getDiskModel(QString DiskName);
    QString getDiskSerial(QString DiskName);
    unsigned int getDiskPartitionNums(QString DiskName);

signals:

};

#endif // DISK_H
