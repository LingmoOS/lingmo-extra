#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>

#include "slider.h"

#define DEFAULT_HEIGHT      4
#define SLIDER_SIDE_SIZE    12
#define SLIDER_MARK_HEIGHT  10
#define SLIDER_MARK_WIDTH   6

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr);

    void setRange(qint64 min, qint64 max);
    void setValue(qint64 value);
    void setSlider(Slider *sl);
    void updateSliderPos();

    void addMark(int value, QString desc);
    void deleteMark(int value);
    void clearMark();

    int getValueBarWidth(){return m_valueBar->width();}

    void setBlackTheme();
    void setLightTheme();

signals:
    void valueChange(int);
    void sliderMoved(qint64);
    void mousePosChange(qint64);
    void mouseLeave();

private:
    QString m_color;
    Slider *m_slider;
    QWidget *m_valueBar,
            *m_widgetDock;
    qint64  m_minValue,
            m_maxValue,
            m_currentValue,
            m_mouse_value;
    bool m_can_change_value,
         m_mouse_move_seek_flag;
    QList<FixedSlider*> m_markList;

    qint64 w2v(qint64 width);
    qint64 v2w(qint64 value);
    void updateMouseValue(int vb_x);

protected:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
};

#endif // PROGRESSBAR_H
