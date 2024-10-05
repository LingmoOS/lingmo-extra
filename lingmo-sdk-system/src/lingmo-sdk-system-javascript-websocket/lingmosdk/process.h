#ifndef PROCESS_H
#define PROCESS_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Process : public QObject
{
    Q_OBJECT
public:
    explicit Process(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    double getProcInfoCpuUsage(int pid);
    //double getProcInfoIoUsage(int pid);
    double getProcInfoMemUsage(int pid);
    QString getProcInfoStatus(int pid);
    int getProcInfoPort(int pid);
    QString getProcInfoStartTime(int pid);
    QString getProcInfoRunningTime(int pid);
    QString getProcInfoCpuTime(int pid);
    QString getProcInfoCmd(int pid);
    QString getProcInfoUser(int pid);
    QStringList getProcInfo(QString procName);
    QStringList getAllProcInfo(void);

signals:
    void sendText(const QString &text);
};

#endif // PROCESS_H
