#ifndef LOGROTATE_H
#define LOGROTATE_H

#include <QObject>

class LogRotate : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmosdk.base.logrotate")
public:
    explicit LogRotate(QObject *parent = nullptr);

public slots:
    int setConfig(QString path);
};

#endif // LOGROTATE_H
