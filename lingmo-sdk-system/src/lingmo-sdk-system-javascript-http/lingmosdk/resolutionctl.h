#ifndef RESOLUTIONCTL_H
#define RESOLUTIONCTL_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Resolutionctl : public QWidget
{
    Q_OBJECT
public:
    explicit Resolutionctl(QWidget *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    QStringList getSysLegalResolution();

signals:

};

#endif // RESOLUTIONCTL_H
