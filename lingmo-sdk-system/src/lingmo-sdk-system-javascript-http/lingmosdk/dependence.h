#ifndef DEPENDENCE_H
#define DEPENDENCE_H

#include <QWidget>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>
#include <QDBusReply>

class Dependence : public QWidget
{
    Q_OBJECT
public:
    explicit Dependence(QWidget *parent = nullptr);
    int dbusConnect;

private:
    void dbusSend(QString method, QVariant &res);

signals:

};

#endif // DEPENDENCE_H
