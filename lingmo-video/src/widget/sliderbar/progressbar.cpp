#include "progressbar.h"

#include <QDebug>
#include <QEvent>
#include <QToolTip>
#include <QMouseEvent>

#include "global/functions.h"
#include "global/global.h"
#include "sliderbar.h"

using namespace Global;

ProgressBar::ProgressBar(QWidget *parent) :
    QWidget(parent),
    m_widgetDock(nullptr),
    m_valueBar(nullptr),
    m_maxValue(0),
    m_minValue(0)
{
    m_can_change_value = true;
    m_mouse_move_seek_flag = false;

    setAttribute(Qt::WA_TranslucentBackground, true);

    m_widgetDock = new QWidget(this);
    m_widgetDock->setObjectName("progress");
    m_widgetDock->setStyleSheet("#progress{background-color:rgba(255,255,255,26); border-radius:2px;}");
    m_widgetDock->setCursor(Qt::PointingHandCursor);

    m_valueBar = new QWidget(m_widgetDock);
    m_valueBar->resize(0, DEFAULT_HEIGHT);

    m_valueBar->setMouseTracking(true);
    setMouseTracking(true);
    m_widgetDock->setMouseTracking(true);

    connect(g_core_signal, &GlobalCoreSignal::sigStateChange, [this](){if (g_playstate == Mpv::Stopped) m_can_change_value = true;});
}

void ProgressBar::setRange(qint64 min, qint64 max)
{
    if (min > max) {
        return;
    }
    m_minValue = min;
    m_maxValue = max;
    m_currentValue = 0;
    m_valueBar->resize(0, height());
}

void ProgressBar::setValue(qint64 value)
{
    if (!m_can_change_value) {
        return;
    }
    if (value < m_minValue || value > m_maxValue) {
        return;
    }
    m_currentValue = value;

    if (m_maxValue != m_minValue)
        m_valueBar->resize((double)width() / (double)(m_maxValue - m_minValue) * (m_currentValue - m_minValue), height());
}

void ProgressBar::setSlider(Slider *sl)
{
    m_slider = sl;
    // 拖动滑块改变显示进度
    connect(m_slider, &Slider::posXUpdate, [this](int x){
        m_valueBar->resize(x - 8 + m_slider->width() / 2, height());
        setToolTip(QString::number(w2v(m_valueBar->width())));
        updateMouseValue(m_valueBar->width());
        emit sliderMoved(m_mouse_value);
    });
    connect(m_slider, &Slider::lockProgress, [this](bool flag) {
        // 如果锁定进度条，则说明不能外部更新 value
        m_can_change_value = !flag;
        // 滑块释放之后 value 需要通知外部
        if (m_can_change_value) {
            emit valueChange(m_mouse_value);
        }
    });
}

/**
** @brief     : 更新滑块位置，如果整体大小发生改变的话
** @param[in] :
** @param[out]:
** @return    :
***/
void ProgressBar::updateSliderPos()
{
    m_slider->move(DEFAULT_LR_MARGIN + m_valueBar->width() - (SLIDER_SIDE_SIZE / 2),
                   (parentWidget()->height() - SLIDER_SIDE_SIZE) / 2);

    for (FixedSlider *fs : m_markList) {
        fs->move(fs->x(), (parentWidget()->height() - SLIDER_MARK_HEIGHT) / 2);
    }
}

/**
** @brief     : 添加书签
** @param[in] : value 书签的进度值（秒），desc 书签描述
** @param[out]:
** @return    :
***/
void ProgressBar::addMark(int value, QString desc)
{
    FixedSlider *fs = new FixedSlider(parentWidget());
    connect(fs, &FixedSlider::clicked, [this, fs](){
        m_currentValue = fs->value;
        updateMouseValue(fs->x() - DEFAULT_LR_MARGIN + fs->width() / 2);
        emit sliderMoved(m_mouse_value);
        emit valueChange(fs->value);
    });
    fs->value = value * 1000;
    fs->move(v2w(fs->value) + DEFAULT_LR_MARGIN - fs->width() / 2,
             (parentWidget()->height() - fs->height()) / 2);
    fs->setDescribe(desc);
    fs->show();
    fs->raise();

    m_markList.push_back(fs);
}

void ProgressBar::deleteMark(int value)
{
    int i=0;
    for (FixedSlider *fs : m_markList) {
        if (fs->value == value * 1000) {
            delete fs;
            break;
        }
        i++;
    }
    m_markList.removeAt(i);
}

void ProgressBar::clearMark()
{
    for (FixedSlider *fs : m_markList) {
        delete fs;
    }
    m_markList.clear();
}

void ProgressBar::setBlackTheme()
{
    m_color = "rgba(255,255,255,50)";
    m_widgetDock->setStyleSheet("#progress{background-color:rgba(255,255,255,50); border-radius:2px;}");
}

void ProgressBar::setLightTheme()
{
    m_color = "rgba(48,49,51,50)";
    m_widgetDock->setStyleSheet("#progress{background-color:rgba(48,49,51,50); border-radius:2px;}");
}

/**
** @brief     : width 转 value
** @param[in] :
** @param[out]:
** @return    :
***/
qint64 ProgressBar::w2v(qint64 width)
{
    return double(m_maxValue - m_minValue) * ((double)width / (double)this->width()) + m_minValue;
}

/**
** @brief     : value 转 width
** @param[in] :
** @param[out]:
** @return    :
***/
qint64 ProgressBar::v2w(qint64 value)
{
    if (m_maxValue != m_minValue)
        return (double)width() / (double)(m_maxValue - m_minValue) * (value - m_minValue);
    return 1;
}

void ProgressBar::updateMouseValue(int vb_x)
{
    if (vb_x < 0) {
        vb_x = 0;
    }
    else if (vb_x > width()) {
        vb_x = width();
    }
    m_valueBar->resize(vb_x, height());

    m_mouse_value = w2v(m_valueBar->width());
    m_slider->move(DEFAULT_LR_MARGIN + vb_x - (SLIDER_SIDE_SIZE / 2),
                   (parentWidget()->height() - SLIDER_SIDE_SIZE) / 2);
}

void ProgressBar::enterEvent(QEvent *e)
{
    m_slider->show();
    m_slider->raise();
}

void ProgressBar::leaveEvent(QEvent *e)
{
    m_slider->setHide();
    emit mouseLeave();
}

void ProgressBar::resizeEvent(QResizeEvent *e)
{
    m_widgetDock->resize(size());
    m_widgetDock->setStyleSheet(QString("#progress{background-color:%1; border-radius:%2px;}").arg(m_color).arg(e->size().height()/2));
    m_valueBar->setStyleSheet(QString("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, "
                              "stop:0 rgba(41, 134, 255, 255), stop:1 rgba(62, 255, 254, 255));"
                              "border-radius:%1px;").arg(e->size().height()/2));
    if (m_maxValue != m_minValue)
        m_valueBar->resize((m_currentValue - m_minValue) * width() / (m_maxValue - m_minValue), height());

    for (FixedSlider *fs : m_markList) {
        fs->move(v2w(fs->value) + DEFAULT_LR_MARGIN - fs->width() / 2,
                 (parentWidget()->height() - fs->height()) / 2);
    }
}

void ProgressBar::mouseMoveEvent(QMouseEvent *e)
{
    emit mousePosChange(w2v(e->x()));
    if (!m_can_change_value) {
        updateMouseValue(e->x());
        m_mouse_move_seek_flag = true;
        emit sliderMoved(m_mouse_value);
    }
}

void ProgressBar::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_can_change_value = false;
        updateMouseValue(e->x());
    }
}

void ProgressBar::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_can_change_value = true;
        updateMouseValue(e->x());
        // 如果没有 move 过就直接用此事件去判断，如果 move 过就不要用此去跳转
        if (!m_mouse_move_seek_flag) {
            emit sliderMoved(m_mouse_value);
        }
        else {
            m_mouse_move_seek_flag = false;
        }
    }
}
