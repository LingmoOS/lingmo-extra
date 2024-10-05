#include "timeslider.h"
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>

#include "global/global.h"

TimeSlider::TimeSlider(QWidget *parent):
    SliderBar(parent)
{
    setMouseTracking(true);
    setObjectName("TimeSlider");

    m_isPressed = false;
    m_isPreviewShow = false;
    m_mousePos = 0;
    m_timerPreview = new QTimer;
    m_timerPreview->setInterval(100);
    setEnabled(false);
    connect(m_timerPreview, &QTimer::timeout, [this](){
        m_timerPreview->stop();
        emit sigShowPreview(m_mousePos);
        m_isPreviewShow = true;
    });
    connect(g_core_signal, &GlobalCoreSignal::sigStateChange, [this](){
        if(Global::g_playstate > 0)
        {
//            setCursor(Qt::PointingHandCursor);
            setEnabled(true);
        }
        else
        {
//            setCursor(Qt::ArrowCursor);
            setEnabled(false);
        }
    });

//    // 样式
//    setStyleSheet("QSlider::add-page:Horizontal\
//    {\
//        background-color: rgb(87, 97, 106);\
//        height:4px;\
//    }\
//    QSlider::sub-page:Horizontal \
//    {\
//        border:none; \
//        border-radius:2px; \
//        background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(41,134,255,255), stop:1 rgba(62,255,254,255));\
//        height:4px;\
//    }\
//    QSlider::groove:Horizontal \
//    {\
//        background-color:rgba(255,255,255,204);\
//        height:4px;\
//    }");


//    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
//    effect->setOffset(4,4);
//    effect->setColor(QColor(0,0,0,50));
//    effect->setBlurRadius(10);
//    setGraphicsEffect(effect);
}
#if 0
void TimeSlider::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton && isEnabled())
    {
        m_isPressed = true;
        int duration = maximum() - minimum();
        int pos = (double)minimum() + (double)duration * ((double)e->x() / (double)width());
        if(pos != sliderPosition())
            setValue(pos);
    }
}

void TimeSlider::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton && isEnabled())
    {
        m_isPressed = false;
        // 跳转之后需要隐藏预览框
        m_timerPreview->stop();
        emit sigHidePreview();
        m_isPreviewShow = false;
    }
}

void TimeSlider::mouseMoveEvent(QMouseEvent *e)
{
    if(m_isPressed)
    {
        int duration = maximum() - minimum();
        int pos = minimum() + duration * ((double)e->x() / width());
        if(pos != sliderPosition())
            setValue(pos);
    }
    m_mousePos = (double)e->x() * (double)maximum() / (double)width();
    if(m_isPreviewShow)
        emit sigShowPreview(m_mousePos);
}

void TimeSlider::leaveEvent(QEvent *e)
{
    m_timerPreview->stop();
    emit sigHidePreview();
    m_isPreviewShow = false;
}

void TimeSlider::enterEvent(QEvent *e)
{
    if(isEnabled())
        m_timerPreview->start();
}
#endif
