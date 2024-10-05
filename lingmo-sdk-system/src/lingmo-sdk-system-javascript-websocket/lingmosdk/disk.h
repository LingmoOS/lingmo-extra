#ifndef DISK_H
#define DISK_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>
#include "lingmosdkdbus.h"

class Disk : public QObject
{
    Q_OBJECT
public:
    explicit Disk(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface ;
    QString ServerPath ;


public slots:
    QStringList getDiskList();
    int getDiskSectorSize(QString DiskName);
    double getDiskTotalSizeMiB(QString DiskName);
    QString getDiskModel(QString DiskName);
    QString getDiskSerial(QString DiskName);
    int getDiskPartitionNums(QString DiskName);
    QString getDiskType(QString DiskName);
    QString getDiskVersion(QString DiskName);
    //unsigned int getDiskSpeed(QString DiskName);
    unsigned long long getDiskSectorNum(QString DiskName);

signals:
    void sendText(const QString &text);

};

#endif // DISK_H
