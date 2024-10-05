#include "playglwidget.h"
#include <QOpenGLContext>
#include <QApplication>
#include <QMouseEvent>
#include <QGSettings>
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <lingmo-log4qt.h>

#include "global/global.h"

using namespace Global;

static void *get_proc_address(void *ctx, const char *name) {
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return NULL;
    return (void *)glctx->getProcAddress(QByteArray(name));
}

PlayGLWidget::PlayGLWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_screenScale(1.0),
      mpv_gl(nullptr)
{
    setlocale(LC_NUMERIC, "C");
    setMouseTracking(true);

    m_dbClicked = false;
    m_mouseUsed = true;
    m_powermanager_gsettings = new QGSettings("org.lingmo.power-manager");

    initGlobalSig();

    m_checkMouseTimer = new QTimer;
    m_checkMouseTimer->setInterval(2000);
    connect(m_checkMouseTimer, &QTimer::timeout, [&](){
        if(!m_hasVideo)
            return;
        if(m_isMouseEnter) {
            if (m_mouseUsed)
                setCursor(QCursor(Qt::BlankCursor));
        }
        g_user_signal->hideBar(true);

        m_checkMouseTimer->stop();
    });

    m_lMouseClickTimer = new QTimer;
    m_lMouseClickTimer->setInterval(300);
    connect(m_lMouseClickTimer, &QTimer::timeout, [&](){
        if (m_dbClicked) {
            m_lMouseClickTimer->stop();
            return;
        }
        emit mousePressed();
        m_lMouseClickTimer->stop();
    });
}

PlayGLWidget::~PlayGLWidget()
{
}

void PlayGLWidget::swapped()
{
    mpv_opengl_cb_report_flip(mpv_gl, 0);
}

void PlayGLWidget::on_update(void *ctx)
{
    QMetaObject::invokeMethod((PlayGLWidget*)ctx, "update");
}

void PlayGLWidget::initMpvGL()
{
    if(mpv_h)
    {
        mpv_gl = (mpv_opengl_cb_context *)mpv_get_sub_api(mpv_h, MPV_SUB_API_OPENGL_CB);
        if (!mpv_gl)
        {
            KyInfo() << "get sub api error";
            return;
        }
        mpv_opengl_cb_set_update_callback(mpv_gl, PlayGLWidget::on_update, (void *)this);
        connect(this, &PlayGLWidget::frameSwapped, this, &PlayGLWidget::swapped);

        int r = mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address, NULL);
        if (r < 0)
        {
            KyInfo("could not initialize OpenGL");
        }
    }
}

void PlayGLWidget::initGlobalSig()
{
    connect(g_core_signal, &GlobalCoreSignal::sigStateChange, [&](){
        if (g_playstate == Mpv::Playing) {
            m_screenScale = qApp->devicePixelRatio();
        }
        else if (g_playstate == Mpv::Stopped) {
            update();
        }
    });

    connect(g_core_signal, &GlobalCoreSignal::sigStateChange, this, &PlayGLWidget::playStateChange);
    connect(g_core_signal, &GlobalCoreSignal::sigVideoIdChange, this, &PlayGLWidget::videIdChange);
}

void PlayGLWidget::initializeGL()
{
    if(mpv_gl)
    {
        int r = mpv_opengl_cb_init_gl(mpv_gl, NULL, get_proc_address, NULL);
        if (r < 0)
        {
            KyInfo("could not initialize OpenGL");
        }
    }
}

void PlayGLWidget::paintGL()
{
    if(mpv_gl)
    {
        mpv_opengl_cb_draw(mpv_gl,
                           this->defaultFramebufferObject(),
                           this->width()*m_screenScale,
                           -this->height()*m_screenScale);
    }
}

void PlayGLWidget::setMouseUsed(bool used)
{
    m_mouseUsed = used;
    if (!m_mouseUsed) {
        setCursor(QCursor(Qt::ArrowCursor));
        m_checkMouseTimer->stop();
    }
}

void PlayGLWidget::playStateChange()
{
    if(g_playstate < 0) {
        m_checkMouseTimer->stop();
    }
    else {
        m_checkMouseTimer->start();
    }
}

void PlayGLWidget::videIdChange(int vid)
{
    if(vid < 0)
        m_hasVideo = false;
    else
        m_hasVideo = true;
}


void PlayGLWidget::mousePressEvent(QMouseEvent *e)
{
    m_isMousePressed = true;
    m_mousePosPressed = e->pos();
    m_posStart = e->pos();
    m_posLast = e->pos();

    // 右键点击触发显示菜单
    if (e->button() == Qt::LeftButton) {
        m_timePressStart = QDateTime::currentMSecsSinceEpoch();
    }
    return QOpenGLWidget::mousePressEvent(e);
}

void PlayGLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_isMousePressed = false;
    m_is_adjust_volume = false;
    m_is_adjust_light = false;
    m_is_adjust_progress = false;
    // 左键离开触发暂停，如果左键按得时间很长就不触发了，不然会和触摸屏长按冲突
    if (e->button() == Qt::LeftButton && (QDateTime::currentMSecsSinceEpoch() - m_timePressStart) < 500) {
        if (!m_mouseUsed)
            return QOpenGLWidget::mouseReleaseEvent(e);
        if (e->pos() == m_mousePosPressed) {
            m_lMouseClickTimer->start();
        }
    }
    return QOpenGLWidget::mouseReleaseEvent(e);
}

void PlayGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (m_isMousePressed) {
        m_posCurrent = e->pos();
        int xc = abs(m_posCurrent.x() - m_posLast.x());
        int yc = abs(m_posCurrent.y() - m_posLast.y());
        if (!m_is_adjust_volume && !m_is_adjust_light && !m_is_adjust_progress) {
            // 如果是第一次的话就判断是调节什么
            if (xc < yc) {
                // y 改变大于 x 改变，要么是调节音量，要么是调节亮度
                // 左边调节亮度，右边调节音量
                if (m_posLast.x() < width() / 2)
                    m_is_adjust_light = true;
                else
                    m_is_adjust_volume = true;
            }
            else {
                m_is_adjust_progress = true;
            }
        }

        // 调节音量
        if (m_is_adjust_volume) {
            int volume_change = yc * 200 / height();
            //符合音量调节
            if (volume_change > 0) {
                if (m_posCurrent.y() - m_posLast.y() > 2) {
                    g_user_signal->setVolumeDown(volume_change);
                }
                else {
                    g_user_signal->setVolumeUp(volume_change);
                }
                m_posLast = m_posCurrent;
            }
        }
        else if (m_is_adjust_light) {
            int light_change = yc * 200 / height();
            if (light_change > 0) {
                double light_current = m_powermanager_gsettings->get("brightness-ac").toDouble();
                if (m_posCurrent.y() - m_posLast.y() > 2) {
                    light_current -= light_change;
                    light_current = light_current < 1 ? 1 : light_current;
                }
                else {
                    light_current += light_change;
                    light_current = light_current > 100 ? 100 : light_current;
                }
                m_posLast = m_posCurrent;
                m_powermanager_gsettings->set("brightness-ac", light_current);
            }
        }
        else if (m_is_adjust_progress) {
            int progress_change = xc * 200 / height();
            if (progress_change > 0) {
                if (m_posCurrent.x() - m_posLast.x() > 2) {
                    g_user_signal->progressUp(progress_change);
                }
                else {
                    g_user_signal->progressDown(progress_change);
                }
                m_posLast = m_posCurrent;
            }
        }
    }

    m_isMouseEnter = true;
    m_checkMouseTimer->stop();
    setCursor(QCursor(Qt::ArrowCursor));
    g_user_signal->hideBar(false);
    if(g_playstate > 0)
        m_checkMouseTimer->start();
    e->accept();
}

void PlayGLWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (!m_mouseUsed)
        return QOpenGLWidget::mouseDoubleClickEvent(e);
    if(e->button() == Qt::LeftButton) {
        g_user_signal->fullScreen();
        m_dbClicked = true;
        QTimer::singleShot(500, [this]{m_dbClicked = false;});
        m_lMouseClickTimer->stop();
    }
    return QOpenGLWidget::mouseDoubleClickEvent(e);
}

void PlayGLWidget::wheelEvent(QWheelEvent *e)
{
    // 鼠标滚轮和触控板双指滑动调节音量
    int wheel_distance = e->delta();
    if (abs(wheel_distance) / 120 == 1) {
        if (wheel_distance > 0)
            g_user_signal->setVolumeUp(5);
        else
            g_user_signal->setVolumeDown(5);
    }
    return QOpenGLWidget::wheelEvent(e);
}

void PlayGLWidget::enterEvent(QEvent *e)
{
    m_isMouseEnter = true;
    e->accept();
}

void PlayGLWidget::leaveEvent(QEvent *e)
{
    m_isMousePressed = false;
    m_isMouseEnter = false;
    if(g_playstate > 0)
        m_checkMouseTimer->start();

    return QOpenGLWidget::leaveEvent(e);
}

bool PlayGLWidget::event(QEvent *e)
{
//    if (e->type() == QEvent::ContextMenu) {
//        setCursor(Qt::ArrowCursor);
//        m_isMouseEnter = false;
//        g_user_signal->showRightMenu();
//        return QOpenGLWidget::event(e);
//    }

    return QOpenGLWidget::event(e);
}

