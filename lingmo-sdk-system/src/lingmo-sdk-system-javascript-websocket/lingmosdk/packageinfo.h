#ifndef PACKAGEINFO_H
#define PACKAGEINFO_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Packageinfo : public QObject
{
    Q_OBJECT
public:
    explicit Packageinfo(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

public slots:
    QStringList getPackageList();
    QString getPackageVersion(QString);
    int getPackageInstalled(QString,QString);

signals:
    void sendText(const QString &text);
};

#endif // PACKAGEINFO_H