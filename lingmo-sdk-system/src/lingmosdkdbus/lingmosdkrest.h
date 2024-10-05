#ifndef KYSDKREST_H
#define KYSDKREST_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QtDebug>
#include <string.h>

#include <memory>

class KySdkRest : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.resolution")
public:
    explicit KySdkRest(QObject *parent = nullptr);

public slots:
    QStringList getSysLegalResolution() const;

};

#endif // KYSDKREST_H
