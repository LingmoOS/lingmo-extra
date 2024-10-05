#include "filletwidget.h"
#include <QPainter>
#include <QGSettings>
#include <QDebug>

#include "global/global.h"
#include "global/globalsignal.h"

using namespace Global;

FilletWidget::FilletWidget(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    m_radius = 8;

    if (FOLLOW_SYS_THEME) {
        // 根据主题设置样式
        if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
            m_color = QColor(249, 249, 249, 240);
        else
            m_color = QColor(38, 38, 38, 240);
        connect(g_gsettings, &QGSettings::changed, [&](QString key){
            // 如果不是跟随主题的话直接返回
            if(key == "styleName") {
                if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                    m_color = QColor(249, 249, 249, 240);
                else
                    m_color = QColor(38, 38, 38, 240);
            }
        });
        connect(g_user_signal, &GlobalUserSignal::sigTheme, [&](int theme){
            switch (theme) {
            case 0:
                if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                    m_color = QColor(249, 249, 249, 240);
                else
                    m_color = QColor(38, 38, 38, 240);
                break;
            case 1:
                m_color = QColor(249, 249, 249, 240);
                break;
            case 2:
                m_color = QColor(38, 38, 38, 240);
                break;
            default:
                break;
            }
        });
    }
    else {
        m_color = QColor(38, 38, 38, 240);
    }
}

void FilletWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 抗锯齿;
    QRect rect = this->rect();
    painter.setBrush(QBrush(m_color));
    painter.setPen(Qt::transparent);
    painter.drawRoundedRect(rect, m_radius, m_radius);
    QWidget::paintEvent(event);
}
