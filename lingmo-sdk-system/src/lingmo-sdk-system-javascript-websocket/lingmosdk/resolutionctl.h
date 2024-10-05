#ifndef RESOLUTIONCTL_H
#define RESOLUTIONCTL_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Resolutionctl : public QObject
{
    Q_OBJECT
public:
    explicit Resolutionctl(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    QStringList getSysLegalResolution();

signals:
    void sendText(const QString &text);
};

#endif // RESOLUTIONCTL_H
