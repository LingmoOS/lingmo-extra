#ifndef SYSINFO_H
#define SYSINFO_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Sysinfo : public QObject
{
    Q_OBJECT
public:
    explicit Sysinfo(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    QString getSystemArchitecture(void);
    QString getSystemName(void);
    QString getSystemVersion(bool vsname);
    int getSystemActivationStatus(void);
    QString getSystemSerialNumber(void);
    QString getSystemKernelVersion(void);
    QString getSystemEffectUser(void);
    QString getSystemProjectName(void);
    QString getSystemProjectSubName(void);
    int getSystemProductFeatures(void);
    QString getSystemHostVirtType(void);
    QString getSystemHostCloudPlatform(void);
    QString getSystemOSVersion(void);
    QString getSystemUpdateVersion(void);
    bool getSystemIsZYJ(void);
    QStringList getSysLegalResolution(void);

signals:
    void sendText(const QString &text);
};

#endif // SYSINFO_H