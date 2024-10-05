#ifndef PROCESS_H
#define PROCESS_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Process : public QWidget
{
    Q_OBJECT
public:
    explicit Process(QWidget *parent = nullptr);
    Process(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

    QJsonObject deal_process();
    QJsonObject deal_process_path();
    QJsonObject deal_process_path_para();
private:
    QStringList path_list;
    QString path;


public slots:
    double getProcInfoCpuUsage(int pid);
    double getProcInfoIoUsage(int pid);
    double getProcInfoMemUsage(int pid);
    QString getProcInfoStatus(int pid);
    int getProcInfoPort(int pid);
    QString getProcInfoStartTime(int pid);
    QString getProcInfoRunningTime(int pid);
    QString getProcInfoCpuTime(int pid);
    QString getProcInfoCmd(int pid);
    QString getProcInfoUser(int pid);
    QStringList getProcInfo(QString procName);
    QStringList getAllProcInfo();


signals:

};

#endif // PROCESS_H
