#ifndef NETCARD_H
#define NETCARD_H

#include <QWidget>
#include "lingmosdkdbus.h"

class NetCard : public QObject
{
    Q_OBJECT
public:
    explicit NetCard(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;

public slots:
    QStringList getNetCardName();
    int getNetCardUp(QString NetCardName);
    QStringList getNetCardUpcards();
    QString getNetCardPhymac(QString NetCardName);
    QString getNetCardPrivateIPv4(QString NetCardName);
    QStringList getNetCardIPv4(QString NetCardName);
    QString getNetCardPrivateIPv6(QString NetCardName);
    int getNetCardType(QString NetCardName);
    QStringList getNetCardProduct(QString NetCardName);
    QStringList getNetCardIPv6(QString NetCardName);

signals:
    void sendText(const QString &text);
};

#endif // NETCARD_H
