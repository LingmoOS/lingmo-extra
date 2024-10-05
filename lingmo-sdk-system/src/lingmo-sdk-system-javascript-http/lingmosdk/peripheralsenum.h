#ifndef PERIPHERALSENUM_H
#define PERIPHERALSENUM_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Peripheralsenum : public QWidget
{
    Q_OBJECT
public:
    explicit Peripheralsenum(QWidget *parent = nullptr);
    Peripheralsenum(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;
    QJsonObject deal_peripheralsenum();
    QJsonObject deal_peripheralsenum_path();
private:
    QStringList path_list;
    QString path;

public slots:
    QStringList getAllUsbInfo();

signals:

};

#endif // PERIPHERALSENUM_H
