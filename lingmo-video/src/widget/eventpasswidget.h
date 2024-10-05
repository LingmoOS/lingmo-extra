#ifndef EVENTPASSWIDGET_H
#define EVENTPASSWIDGET_H

#include <QWidget>

#include "core/mpvtypes.h"

class QGSettings;

class EventPassWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EventPassWidget(QWidget *parent = nullptr);
    ~EventPassWidget();

    void setMouseUsed(bool used);

signals:
    void mousePressed();

private slots:
    void playStateChange();
    void videIdChange(int vid);

private:
    void initGlobalSig();

    bool m_mouseUsed,
         m_isMouseEnter,
         m_dbClicked,
         m_hasVideo,
         m_is_adjust_volume = false,
         m_is_adjust_light = false,
         m_is_adjust_progress = false,
         m_isMousePressed = false;

    QPoint m_posStart;
    QPoint m_posLast;
    QPoint m_posCurrent;
    QPoint m_mousePosPressed;

    QTimer  *m_checkMouseTimer,
            *m_lMouseClickTimer;

    qint64 m_timePressStart;
    QGSettings *m_powermanager_gsettings;

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
    void enterEvent(QEvent *e) override;
    void leaveEvent(QEvent *e) override;
    bool event(QEvent *e) override;
};

#endif // EVENTPASSWIDGET_H
