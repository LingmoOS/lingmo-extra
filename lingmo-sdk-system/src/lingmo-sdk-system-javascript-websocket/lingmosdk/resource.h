#ifndef RESOURCE_H
#define RESOURCE_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Resource : public QObject
{
    Q_OBJECT
public:
    explicit Resource(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    double getMemTotalKiB(void);
    double getMemUsagePercent(void);
    double getMemUsageKiB(void);
    double getMemAvailableKiB(void);
    double getMemFreeKiB(void);
    double getMemVirtAllocKiB(void);
    double getMemSwapTotalKiB(void);
    double getMemSwapUsagePercent(void);
    double getMemSwapUsageKiB(void);
    double getMemSwapFreeKiB(void);
    double getCpuCurrentUsage(void);
    QString getUpTime(void);

signals:
    void sendText(const QString &text);
};

#endif // RESOURCE_H