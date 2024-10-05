#ifndef PLAYGLWIDGET_H
#define PLAYGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>

#include "eventpasswidget.h"
#include "core/mpvtypes.h"

class QGSettings;

class PlayGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    PlayGLWidget(QWidget *parent = 0);
    ~PlayGLWidget();
    void setMpvHandle(mpv::qt::Handle mh){mpv_h = mh;initMpvGL();}

private slots:
    void swapped();

private:
    mpv::qt::Handle mpv_h;
    mpv_opengl_cb_context *mpv_gl;

    double m_screenScale;

    static void on_update(void *ctx);
    void initMpvGL();

protected:
    void initializeGL() override;
    void paintGL() override;

public:
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

    int m_volumeChange;

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

#endif // PLAYGLWIDGET_H
