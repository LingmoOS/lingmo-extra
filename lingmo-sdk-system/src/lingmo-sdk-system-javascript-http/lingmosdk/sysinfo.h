#ifndef SYSINFO_H
#define SYSINFO_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Sysinfo : public QWidget
{
    Q_OBJECT
public:
    explicit Sysinfo(QWidget *parent = nullptr);
    Sysinfo(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;
    QJsonObject deal_sysinfo();
    QJsonObject deal_sysinfo_path();
    QJsonObject deal_sysinfo_path_para();
private:
    QStringList path_list;
    QString path;

public slots:
    QString getSystemArchitecture();
    QString getSystemName();
    QString getSystemVersion(bool version);
    int getSystemActivationStatus();
    QString getSystemSerialNumber();
    QString getSystemKernelVersion();
    QString getSystemEffectUser();
    QString getSystemProjectName();
    QString getSystemProjectSubName();
    int getSystemProductFeatures();
    QString getSystemHostVirtType();
    QString getSystemHostCloudPlatform();
    QString getSystemOSVersion();
    QString getSystemUpdateVersion();
    bool getSystemIsZYJ();
    QStringList getSysLegalResolution();

signals:

};

#endif // SYSINFO_H
