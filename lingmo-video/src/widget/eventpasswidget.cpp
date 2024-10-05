#include "eventpasswidget.h"

#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QGSettings>
#include <QMouseEvent>
#include <QTouchEvent>

#include "global/globalsignal.h"
#include "global/global.h"

#include <lingmo-log4qt.h>

using namespace Global;

EventPassWidget::EventPassWidget(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    setMouseTracking(true);

    m_dbClicked = false;
    m_mouseUsed = true;
    m_powermanager_gsettings = new QGSettings("org.lingmo.power-manager");

    initGlobalSig();

    m_checkMouseTimer = new QTimer;
    m_checkMouseTimer->setInterval(2000);
    connect(m_checkMouseTimer, &QTimer::timeout, [&](){
        if (!m_hasVideo)
            return;
        if (m_isMouseEnter) {
            if (m_mouseUsed) {
                KyInfo() << "set cursor blank cursor";
                this->setCursor(QCursor(Qt::BlankCursor));
            }
        }
        g_user_signal->hideBar(true);

        m_checkMouseTimer->stop();
    });

    m_lMouseClickTimer = new QTimer;
    m_lMouseClickTimer->setInterval(300);
    connect(m_lMouseClickTimer, &QTimer::timeout, [&](){
        if (m_dbClicked) {
            m_lMouseClickTimer->stop();
            return;
        }
        emit mousePressed();
        m_lMouseClickTimer->stop();
    });
}

EventPassWidget::~EventPassWidget()
{

}

void EventPassWidget::setMouseUsed(bool used)
{
    m_mouseUsed = used;
    if (!m_mouseUsed) {
        setCursor(QCursor(Qt::ArrowCursor));
        m_checkMouseTimer->stop();
    }
}

void EventPassWidget::playStateChange()
{
    if(g_playstate < 0) {
        m_checkMouseTimer->stop();
    }
    else {
        m_checkMouseTimer->start();
    }
}

void EventPassWidget::videIdChange(int vid)
{
    if(vid < 0)
        m_hasVideo = false;
    else
        m_hasVideo = true;
}

void EventPassWidget::initGlobalSig()
{
    connect(g_core_signal, &GlobalCoreSignal::sigStateChange, this, &EventPassWidget::playStateChange);
    connect(g_core_signal, &GlobalCoreSignal::sigVideoIdChange, this, &EventPassWidget::videIdChange);
}

void EventPassWidget::mousePressEvent(QMouseEvent *e)
{
    m_mousePosPressed = e->pos();
    m_posStart = e->pos();
    m_posLast = e->pos();

    // 右键点击触发显示菜单
    if (e->button() == Qt::LeftButton) {
        m_isMousePressed = true;
        m_timePressStart = QDateTime::currentMSecsSinceEpoch();
    }
    return QWidget::mousePressEvent(e);
}

void EventPassWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_isMousePressed = false;
    m_is_adjust_volume = false;
    m_is_adjust_light = false;
    m_is_adjust_progress = false;
    // 左键离开触发暂停，如果左键按得时间很长就不触发了，不然会和触摸屏长按冲突
    if (e->button() == Qt::LeftButton && (QDateTime::currentMSecsSinceEpoch() - m_timePressStart) < 500) {
        if (!m_mouseUsed)
            return QWidget::mouseReleaseEvent(e);
        if (e->pos() == m_mousePosPressed) {
            m_lMouseClickTimer->start();
        }
    }
    return QWidget::mouseReleaseEvent(e);
}

void EventPassWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (m_isMousePressed) {
        m_posCurrent = e->pos();
        int xc = abs(m_posCurrent.x() - m_posLast.x());
        int yc = abs(m_posCurrent.y() - m_posLast.y());
        if (!m_is_adjust_volume && !m_is_adjust_light && !m_is_adjust_progress) {
            // 如果是第一次的话就判断是调节什么
            if (xc < yc) {
                // y 改变大于 x 改变，要么是调节音量，要么是调节亮度
                // 左边调节亮度，右边调节音量
                if (m_posLast.x() < width() / 2)
                    m_is_adjust_light = true;
                else
                    m_is_adjust_volume = true;
            }
            else {
                m_is_adjust_progress = true;
            }
        }

        // 调节音量
        if (m_is_adjust_volume) {
            int volume_change = yc * 200 / height();
            //符合音量调节
            if (volume_change > 0) {
                if (m_posCurrent.y() - m_posLast.y() > 2) {
                    g_user_signal->setVolumeDown(volume_change);
                }
                else {
                    g_user_signal->setVolumeUp(volume_change);
                }
                m_posLast = m_posCurrent;
            }
        }
        else if (m_is_adjust_light) {
            int light_change = yc * 200 / height();
            if (light_change > 0) {
                double light_current = m_powermanager_gsettings->get("brightness-ac").toDouble();
                if (m_posCurrent.y() - m_posLast.y() > 2) {
                    light_current -= light_change;
                    light_current = light_current < 1 ? 1 : light_current;
                }
                else {
                    light_current += light_change;
                    light_current = light_current > 100 ? 100 : light_current;
                }
                m_posLast = m_posCurrent;
                m_powermanager_gsettings->set("brightness-ac", light_current);
            }
        }
        else if (m_is_adjust_progress) {
            int progress_change = xc * 200 / height();
            if (progress_change > 0) {
                if (m_posCurrent.x() - m_posLast.x() > 2) {
                    g_user_signal->progressUp(progress_change);
                }
                else {
                    g_user_signal->progressDown(progress_change);
                }
                m_posLast = m_posCurrent;
            }
        }
    }

    m_isMouseEnter = true;
    m_checkMouseTimer->stop();
    setCursor(QCursor(Qt::ArrowCursor));
    g_user_signal->hideBar(false);
    if(g_playstate > 0)
        m_checkMouseTimer->start();
    e->accept();
}

void EventPassWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (!m_mouseUsed)
        return QWidget::mouseDoubleClickEvent(e);
    if(e->button() == Qt::LeftButton) {
        g_user_signal->fullScreen();
        m_dbClicked = true;
        QTimer::singleShot(1000, [this]{m_dbClicked = false;});
        m_lMouseClickTimer->stop();
    }
    return QWidget::mouseDoubleClickEvent(e);
}

void EventPassWidget::wheelEvent(QWheelEvent *e)
{
    // 鼠标滚轮和触控板双指滑动调节音量
    int wheel_distance = e->delta();
    if (abs(wheel_distance) / 120 == 1) {
        if (wheel_distance > 0)
            g_user_signal->setVolumeUp(5);
        else
            g_user_signal->setVolumeDown(5);
    }
    return QWidget::wheelEvent(e);
}

void EventPassWidget::enterEvent(QEvent *e)
{
    m_isMouseEnter = true;
    e->accept();
}

void EventPassWidget::leaveEvent(QEvent *e)
{
    m_isMouseEnter = false;
    m_isMousePressed = false;
    if(g_playstate > 0)
        m_checkMouseTimer->start();

    return QWidget::leaveEvent(e);
}

bool EventPassWidget::event(QEvent *e)
{
//    if (e->type() == QEvent::ContextMenu) {
//        setCursor(Qt::ArrowCursor);
//        m_isMouseEnter = false;
//        g_user_signal->showRightMenu();
//        return QWidget::event(e);
//    }

    return QWidget::event(e);
}
