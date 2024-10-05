#ifndef NETCARD_H
#define NETCARD_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class NetCard : public QWidget
{
    Q_OBJECT
public:
    explicit NetCard(QWidget *parent = nullptr);
    NetCard(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

    QJsonObject deal_netcard();
    QJsonObject deal_netcard_path();
    QJsonObject deal_netcard_path_para();
private:
    QStringList path_list;
    QString path;

public slots:
    QStringList getNetCardName();
    int getNetCardType(QString NetCardName);
    QStringList getNetCardProduct(QString NetCardName);

    int getNetCardUp(QString NetCardName);
    QStringList getNetCardUpcards();
    QString getNetCardPhymac(QString NetCardName);
    QString getNetCardPrivateIPv4(QString NetCardName);
    QStringList getNetCardIPv4(QString NetCardName);
    QString getNetCardPrivateIPv6(QString NetCardName);
//    int getNetCardType(QString NetCardName);
    QStringList getNetCardIPv6(QString NetCardName);

signals:

};

#endif // NETCARD_H
