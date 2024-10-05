#include "customerevent.h"


QEvent::Type CustomerEvent::m_EventType = QEvent::None;

CustomerEvent::CustomerEvent(QString urlString,QString ipString,QString typeString,QString sendString) : QEvent(eventType())
{
    m_url = urlString;
    m_ip = ipString;
    m_type = typeString;
    m_send = sendString;
}

CustomerEvent::~CustomerEvent()
{

}

QEvent::Type CustomerEvent::eventType()
{
    // 创建事件Type
    if (m_EventType == QEvent::None)
        m_EventType = (QEvent::Type)QEvent::registerEventType();

    return m_EventType;
}

QString CustomerEvent::geturlValueString(void)
{
    return m_url;
}

QString CustomerEvent::getipValueString(void)
{
    return m_ip;
}

QString CustomerEvent::getTypeValueString(void)
{
    return m_type;
}
QString CustomerEvent::getSendValueString(void)
{
    return m_send;
}
