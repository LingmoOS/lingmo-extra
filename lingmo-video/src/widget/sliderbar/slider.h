#ifndef SLIDER_H
#define SLIDER_H

#include <QWidget>

class FixedSlider : public QWidget
{
    Q_OBJECT
public:
    explicit FixedSlider(QWidget *parent = nullptr);
    ~FixedSlider();

    void setDescribe(QString desc) {m_describe = desc;setToolTip(m_describe);}
    int value;

signals:
    void clicked();

private:
    QString m_describe;
    QWidget *m_widgetSlider;

protected:
    void mouseReleaseEvent(QMouseEvent *e) override;
};

class Slider : public QWidget
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = nullptr);
    ~Slider();

    void setHide();
    void setXRange(int min, int max) {m_min_x = min; m_max_x = max;}

public slots:

signals:
    void lockProgress(bool);
    void posXUpdate(int);

private:
    void updatePos();

    QWidget *m_widgetSlider;
    bool m_canHide;
    int m_min_x,
        m_max_x,
        m_mouse_x;

private:
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
};

#endif // SLIDER_H
