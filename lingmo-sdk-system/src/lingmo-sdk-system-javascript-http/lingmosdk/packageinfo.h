#ifndef PACKAGEINFO_H
#define PACKAGEINFO_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Packageinfo : public QWidget
{
    Q_OBJECT
public:
    explicit Packageinfo(QWidget *parent = nullptr);
    Packageinfo(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;
    QJsonObject deal_packageinfo();
    QJsonObject deal_packageinfo_path();
    QJsonObject deal_packageinfo_path_para();
private:
    QStringList path_list;
    QString path;

signals:

public slots:
    QStringList getPackageList();
    QString getPackageVersion(QString packageName);
    int getPackageInstalled(QString packageName,QString packageNum);

};

#endif // PACKAGEINFO_H
