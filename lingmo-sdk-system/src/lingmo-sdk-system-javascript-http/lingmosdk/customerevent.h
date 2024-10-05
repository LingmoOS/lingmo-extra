#ifndef CUSTOMEREVENT_H
#define CUSTOMEREVENT_H
#include <QString>
#include <QEvent>

class CustomerEvent:public QEvent
{
public:
    CustomerEvent(QString urlString ="",QString ipString ="",QString typeString ="",QString sendString="");
    ~CustomerEvent();

    static Type eventType();
    QString geturlValueString(void);
    QString getipValueString(void);
    QString getTypeValueString(void);
    QString getSendValueString(void);
private:
    static Type m_EventType;
    QString m_url;
    QString m_ip;
    QString m_type;
    QString m_send;
};

#endif // CUSTOMEREVENT_H
