#ifndef TIMESLIDER_H
#define TIMESLIDER_H

#include <QSlider>
#include "sliderbar.h"

class QTimer;

//class TimeSlider : public QSlider
class TimeSlider : public SliderBar
{
    Q_OBJECT
public:
    TimeSlider(QWidget *parent = nullptr);

signals:
    void sigShowPreview(int);
    void sigHidePreview();

private:
    bool m_isPressed,
         m_isPreviewShow;
    int m_mousePos;
    QTimer* m_timerPreview;

protected:
#if 0
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void enterEvent(QEvent *e) override;
#endif
};

#endif // TIMESLIDER_H
