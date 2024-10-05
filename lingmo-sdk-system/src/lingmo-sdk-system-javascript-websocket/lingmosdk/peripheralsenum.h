#ifndef PERIPHERALSENUM_H
#define PERIPHERALSENUM_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Peripheralsenum : public QObject
{
    Q_OBJECT
public:
    explicit Peripheralsenum(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

public slots:
    QStringList getAllUsbInfo();

signals:
    void sendText(const QString &text);
};

#endif // PERIPHERALSENUM_H
