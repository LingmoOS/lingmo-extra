#ifndef CPUINFO_H
#define CPUINFO_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Cpuinfo : public QObject
{
    Q_OBJECT
public:
    explicit Cpuinfo(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    QString getCpuArch();
    QString getCpuVendor();
    QString getCpuModel();
    QString getCpuFreqMHz();
    int getCpuCorenums();
    QString getCpuVirt();
    int getCpuProcess();

signals:
    void sendText(const QString &text);
};

#endif // CPUINFO_H