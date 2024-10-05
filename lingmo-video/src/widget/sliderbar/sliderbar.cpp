#include "sliderbar.h"

#include <QTimer>

#include "progressbar.h"
#include "slider.h"

SliderBar::SliderBar(QWidget *parent) : QWidget(parent)
{
    initUI();
    m_canSetValue = true;
}

SliderBar::~SliderBar()
{
    if (m_slider) {
        delete m_slider;
        m_slider = nullptr;
    }

    if (m_progressBar) {
        delete m_progressBar;
        m_progressBar = nullptr;
    }
}

void SliderBar::addMark(int value, QString desc)
{
    m_progressBar->addMark(value, desc);
}

void SliderBar::deleteMark(int value)
{
    m_progressBar->deleteMark(value);
}

void SliderBar::clearMark()
{
    m_progressBar->clearMark();
}

void SliderBar::setTimeTip(QString tip)
{
    m_progressBar->setToolTip(tip);
}

void SliderBar::setBlackTheme()
{
    m_progressBar->setBlackTheme();
}

void SliderBar::setLightTheme()
{
    m_progressBar->setLightTheme();
}

void SliderBar::setValue(int val)
{
    if (!m_canSetValue)
        return;
    m_value = val;
    m_progressBar->setValue(val);
    m_slider->move(DEFAULT_LR_MARGIN + m_progressBar->getValueBarWidth() -
                   (SLIDER_SIDE_SIZE / 2), (height() - SLIDER_SIDE_SIZE) / 2);
}

void SliderBar::setRange(qint64 min, qint64 max)
{
    m_progressBar->setRange(min, max);
}

void SliderBar::initUI()
{
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_progressBar = new ProgressBar(this);
    connect(m_progressBar, &ProgressBar::valueChange, [this](int value) {
        emit valueChange(value);
    });
    connect(m_progressBar, &ProgressBar::sliderMoved, [this](qint64 value) {
        emit sliderMoved(value);
        // move 之后 200ms 才可以去 setValue
        m_canSetValue = false;
        QTimer::singleShot(200, [this](){m_canSetValue = true;});
    });
    connect(m_progressBar, &ProgressBar::mousePosChange, [this](int value) {
        emit mousePosChange(value);
    });
    connect(m_progressBar, &ProgressBar::mouseLeave, [this]() {
        emit mouseLeave();
    });

    m_slider = new Slider(this);
    m_slider->setCursor(Qt::PointingHandCursor);
    m_slider->setHide();
    m_slider->resize(12, 12);

    m_progressBar->setSlider(m_slider);
}

void SliderBar::resizeEvent(QResizeEvent *e)
{
    m_progressBar->setGeometry(DEFAULT_LR_MARGIN, (height()-DEFAULT_HEIGHT) / 2,
                               width() - (2 * DEFAULT_LR_MARGIN), DEFAULT_HEIGHT);
    // 设置滑块 x 轴滑动范围
    m_slider->setXRange(DEFAULT_LR_MARGIN - (SLIDER_SIDE_SIZE / 2), width() - DEFAULT_LR_MARGIN - (SLIDER_SIDE_SIZE / 2));
    m_progressBar->updateSliderPos();
}

void SliderBar::enterEvent(QEvent *e)
{
    m_progressBar->setGeometry(DEFAULT_LR_MARGIN, (height()-DEFAULT_HEIGHT - 4) / 2,
                               width() - (2 * DEFAULT_LR_MARGIN), DEFAULT_HEIGHT + 4);
}

void SliderBar::leaveEvent(QEvent *e)
{
    m_progressBar->setGeometry(DEFAULT_LR_MARGIN, (height()-DEFAULT_HEIGHT) / 2,
                               width() - (2 * DEFAULT_LR_MARGIN), DEFAULT_HEIGHT);
}
