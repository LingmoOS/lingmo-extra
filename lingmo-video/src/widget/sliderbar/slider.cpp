#include "slider.h"

#include <QDebug>
#include <QTimer>
#include <QMouseEvent>

Slider::Slider(QWidget *parent) : QWidget(parent)
{
    m_canHide = true;

    m_widgetSlider = new QWidget(this);
    m_widgetSlider->resize(size());
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_widgetSlider->setObjectName("slider");
    m_widgetSlider->setStyleSheet("#slider{background-color:#FFFFFF;border-radius:6px;}");
}

Slider::~Slider()
{
    if (m_widgetSlider) {
        delete m_widgetSlider;
        m_widgetSlider = nullptr;
    }
}

void Slider::setHide()
{
    if (m_canHide) {
        // 不延迟有冲突会一直 hide show，不知道为啥
        QTimer::singleShot(100, [this](){
            if (m_canHide)
                hide();
        });
    }
}

void Slider::updatePos()
{
    int correct_l = m_mouse_x - width() / 2;
    // 点击位置矫正，如果鼠标点击位置不在中心的话要移到中心
    if (correct_l != 0) {
        if (correct_l + x() < m_min_x) {
            move(m_min_x, y());
        }
        else if (correct_l + x() > m_max_x) {
            move(m_max_x, y());
        }
        else {
            move(x() + correct_l, y());
        }
    }

    // 矫正之后去更新进度条的位置
    emit posXUpdate(x());
}

void Slider::enterEvent(QEvent *e)
{
    m_canHide = false;
}

void Slider::leaveEvent(QEvent *e)
{
    m_canHide = true;
    hide();
}

void Slider::resizeEvent(QResizeEvent *e)
{
    m_widgetSlider->resize(size());
}

void Slider::mouseMoveEvent(QMouseEvent *e)
{
    m_mouse_x = e->x();
    updatePos();
}

void Slider::mousePressEvent(QMouseEvent *e)
{
    m_canHide = false;
    m_mouse_x = e->x();
    updatePos();

    // 滑块被点击的时候进度条不能自动更新位置，只有被释放的时候才能去更新位置
    emit lockProgress(true);
}

void Slider::mouseReleaseEvent(QMouseEvent *e)
{
    // 鼠标释放的时候进度条可以自动更新位置
    emit lockProgress(false);
}

FixedSlider::FixedSlider(QWidget *parent) :
    QWidget(parent)
{
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_widgetSlider = new QWidget(this);
    setFixedSize(6, 10);
    m_widgetSlider->setFixedSize(6, 10);
    m_widgetSlider->setStyleSheet("background-color:#11A8FF;"
                  "border-radius:3px;"
                  "border:1px solid #FFFFFF;");
}

FixedSlider::~FixedSlider()
{
    if (m_widgetSlider) {
        delete m_widgetSlider;
        m_widgetSlider = nullptr;
    }
}

void FixedSlider::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        emit clicked();
    }
}
