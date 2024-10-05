#ifndef RUNINFO_H
#define RUNINFO_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Runinfo : public QObject
{
    Q_OBJECT
public:
    explicit Runinfo(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    // double getIoSpeed();
    double getNetSpeed(QString);
    int getDiskRate(QString diskname);
    // double getMainboardTemperature();
    double getCpuTemperature();
    double getDiskTemperature( QString diskpath);

signals:
    void sendText(const QString &text);
};

#endif // RUNINFO_H
