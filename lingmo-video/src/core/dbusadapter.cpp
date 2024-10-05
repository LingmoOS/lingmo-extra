#include "dbusadapter.h"
#include <QDBusConnection>

#include "global/globalsignal.h"
#include "global/global.h"

using namespace Global;

DbusAdapter::DbusAdapter(QObject *parent) : QObject(parent)
{
    m_currentFile = "";

    connect(g_core_signal, &GlobalCoreSignal::sigStateChange, [&](){
        if(g_playstate < 0)
            m_currentFile = "";
    });
    connect(g_core_signal, &GlobalCoreSignal::sigFileInfoChange, [&](Mpv::FileInfo fi){
        m_currentFile = fi.file_path;
    });

    QDBusConnection::sessionBus().unregisterService("org.mpris.MediaPlayer2.LingmoVideo");
    QDBusConnection::sessionBus().registerService("org.mpris.MediaPlayer2.LingmoVideo");
    QDBusConnection::sessionBus().registerObject("/org/mpris/MediaPlayer2", "org.mpris.MediaPlayer2.Player", this, QDBusConnection::ExportNonScriptableSlots);
}

void DbusAdapter::Stop() const
{
    g_user_signal->stop();
    // 手动停止之后需要显示默认界面
    g_user_signal->showStopFrame();
}

void DbusAdapter::Next() const
{
//    g_user_signal->stop();
//    g_user_signal->play();
    g_user_signal->playNext(true);
}

void DbusAdapter::KvPlayPause() const
{
    if(g_playstate == Mpv::Paused)
        g_user_signal->play();
    else if(g_playstate == Mpv::Playing)
        g_user_signal->pause();
}

void DbusAdapter::PlayPause() const
{
    if(g_playstate == Mpv::Paused)
        g_user_signal->play();
    else if(g_playstate == Mpv::Playing)
        g_user_signal->pause();
}

void DbusAdapter::Previous() const
{
    g_user_signal->playPrev(true);
}

void DbusAdapter::AddFile(QString file) const
{
    g_user_signal->addFiles(QStringList() << file);
}

void DbusAdapter::VolumeUp() const
{
    g_user_signal->setVolumeUp(10);
}

void DbusAdapter::VolumeDown() const
{
    g_user_signal->setVolumeDown(10);
}

void DbusAdapter::FullScreen() const
{
    g_user_signal->fullScreen();
}

void DbusAdapter::LoopMode() const
{
    g_user_signal->changePlayOrder();
}

void DbusAdapter::Exit() const
{
    g_user_signal->exitApp();
}

/**
 * 返回播放状态
 * @return:
 *  -1  :停止
 *  0   :暂停
 *  1   :播放
 */
int DbusAdapter::PlayState()
{
    if(g_playstate == Mpv::Playing)
        return 1;
    else if(g_playstate == Mpv::Paused)
        return 0;
    else
        return -1;
}

QString DbusAdapter::CurrentFile()
{
    return m_currentFile;
}
