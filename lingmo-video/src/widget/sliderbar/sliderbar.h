#ifndef SLIDERBAR_H
#define SLIDERBAR_H

#include <QWidget>

#define DEFAULT_LR_MARGIN   8
#define MIN_HEIGHT          20

QT_FORWARD_DECLARE_CLASS(ProgressBar)
QT_FORWARD_DECLARE_CLASS(Slider)

class SliderBar : public QWidget
{
    Q_OBJECT
public:
    explicit SliderBar(QWidget *parent = nullptr);
    ~SliderBar();

    void addMark(int value, QString desc);
    void deleteMark(int value);
    void clearMark();
    void setTimeTip(QString tip);

    void setBlackTheme();
    void setLightTheme();


public slots:
    void setValue(int val);
    void setRange(qint64 min, qint64 max);

signals:
    void valueChange(int);
    void sliderMoved(qint64);
    void mousePosChange(qint64);
    void mouseLeave();

private:
    void initUI();

private:
    ProgressBar *m_progressBar;
    Slider *m_slider;
    int m_value;
    bool m_canSetValue;

protected:
    void resizeEvent(QResizeEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
};

#endif // SLIDERBAR_H
