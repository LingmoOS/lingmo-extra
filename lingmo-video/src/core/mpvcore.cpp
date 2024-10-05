#include "mpvcore.h"

#include <QThread>
#include <QProcess>
#include <QDateTime>
#include <QDBusMessage>
#include <QApplication>
#include <QStandardPaths>
#include <QDBusConnection>
#include <QCoreApplication>

#include <lingmo-log4qt.h>
#include <ZenLib/Ztring.h>

#include "widget/playxwidget.h"
#include "widget/playglwidget.h"
#include "core/sqlitehandle.h"
#include "global/util.h"
#include "global/global.h"
#include "global/path.h"
#include "global/functions.h"

using namespace MediaInfoLib;
using namespace ZenLib;
using namespace Global;

#define wstring2QString(_DATA) \
    QString::fromUtf8(Ztring(_DATA).To_UTF8().c_str())
#define QString2wstring(_DATA) \
    Ztring().From_UTF8(_DATA.toUtf8())

extern QString int2ts(int64_t time, bool use_msec);

static void wakeup(void *ctx)
{
    MpvCore *mpvhandler = (MpvCore*)ctx;
    QCoreApplication::postEvent(mpvhandler, new QEvent(QEvent::User));
}

MpvCore::MpvCore(QWidget *pw, QObject *parent) :
    QObject(parent)
{
    setlocale(LC_NUMERIC, "C");

    m_playWidget = pw;

    m_volume = 100;
    m_duration = -1;

    initMpvHandle();
    initGlobalSig();
    initVolumeDBus();

    m_isLoaded = false;
    m_osdShow = true;
    m_isHFlip = false;
    m_isVFlip = false;
    m_needSeek = false;
    m_showProfile = false;
    m_isManualStop = false;
    m_isSinkMuteChange = true;
    m_isSinkVolumeChange = true;
    m_isPauseWhenNeedSeek = false;
    m_canSaveScreenShot = true;
    m_changeVolumeForce = false;

    ScreenshotDirectory(g_config->screenShotPath.first);
}

void MpvCore::ShowText(QString text, int duration)
{
    // 只有在播放或者暂停状态才去显示
    if (g_playstate < 0 || m_lastTime < 0.5)
        return;

    KyInfo() << "show osd text " << text;
    // 其他osd内容显示的时候停止profile刷新，显示完成后profile刷新继续
    if(text != "" && m_showProfile)
    {
        m_showInfoTimer->stop();
        QTimer::singleShot(2400, [&](){if(m_showProfile) m_showInfoTimer->start();});
    }
    // 如果打开osd显示信息开关则需要将显示内容追加到fps和profile信息之后
    if(m_showProfile)
    {
        QString tab = "   ";
        QString nl = "\n";
        QString path = m_fileInfo.file_path;
        mpv_get_property(m_mpvHandle, "estimated-vf-fps", MPV_FORMAT_DOUBLE, &m_fps);

        QString info = tr("File:") + path.right(path.length() - path.lastIndexOf("/") - 1) + nl + nl;
        QString infov =
                tr("Video:") + QString(" (x%0) %1").arg(QString::number(m_vtracks)).arg(m_fileInfo.video_params.codec) + nl +
                tab + tr("Resolution:") + QString(" %0x%1").arg(m_fileInfo.video_params.width).arg(m_fileInfo.video_params.height) + nl +
                tab + tr("fps:") + QString(" %0").arg(m_fps) + nl +
                tab + tr("Bitrate:") + QString(" %0").arg(m_videoBitrate==0 ? "(unavailable)" : QString::number((double)m_videoBitrate/1000.0).append("kbps")) + nl +
                nl;
        QString infoa =
                tr("Audio:") + QString(" (x%0) %1").arg(QString::number(m_vtracks)).arg(m_fileInfo.audio_params.codec) + nl +
                tab + tr("Sample Rate:") + QString(" %0Hz").arg(m_fileInfo.audio_params.samplerate) + nl +
                tab + tr("Channels:") + QString(" %0").arg(m_fileInfo.audio_params.channels) + nl +
                tab + tr("Bitrate:") + QString(" %0").arg(m_audioBitrate==0 ? "(unavailable)" : QString::number((double)m_audioBitrate/1000.0).append("kbps")) + nl +
                nl;
        if(m_vtracks > 0)
            info += infov;
        if(m_atracks > 0)
            info += infoa;

        text = QString("%1\n%2").arg(info).arg(text);
    }

    if (m_vid >= 0) {
        const QByteArray tmp1 = text.toUtf8(),
                         tmp2 = QString::number(duration).toUtf8(),
                         tmp3 = QString::number(2).toUtf8();
        const char *args[] = {"show_text", tmp1.constData(), tmp2.constData(), tmp3.constData(), NULL};
        AsyncCommand(args);
    }
    else {
        // 让无视频流界面也显示一下
        emit sigShowText(text);
    }
}

void MpvCore::VideoDecoder(QString vd)
{
    if (vd == "default") {
        SetOption("hwdec", "default,");
    }
    else {
        SetOption("hwdec", vd);
    }
}

void MpvCore::VideoOutput(QString vo)
{
    if (g_config->videoOutputType() == GlobalConfig::VO_WID) {
        if (vo == "auto") {
            if (g_config->hardwareType() == GlobalConfig::JM7X_VDPAU)
                SetOption("vo", "vdpau,xv,x11");
            else if (g_config->hardwareType() == GlobalConfig::JM9X_VAAPI)
                SetOption("vo", "vaapi,xv,x11");
            else
                SetOption("vo", "");
        }
        else {
            SetOption("vo", vo + ",vaapi,vdpau,x11,xv");
        }
    }
}

void MpvCore::DecodeThreads(int threads)
{
    mpv::qt::set_option_variant(m_mpvHandle, "vd-lavc-threads", threads);
}

QString MpvCore::getMediaInfo()
{
    if(m_mediaInfo.length() != 0) {
        return m_mediaInfo;
    }

    QFileInfo fi(m_playingFile);
    if(!fi.exists()) {
        return QString();
    }

    double fps, vbitrate, abitrate;
    mpv_get_property(m_mpvHandle, "estimated-vf-fps", MPV_FORMAT_DOUBLE, &fps);

    QString current_vo, current_ao, hwdec_active;

    char *property_string = nullptr;
    property_string = mpv_get_property_string(m_mpvHandle, "current-vo");
    if(property_string) {
        current_vo = property_string;
        mpv_free(property_string);
        property_string = nullptr;
    }
    property_string = mpv_get_property_string(m_mpvHandle, "current-ao");
    if(property_string) {
        current_ao = property_string;
        mpv_free(property_string);
        property_string = nullptr;
    }
    property_string = mpv_get_property_string(m_mpvHandle, "hwdec-active");
    if(property_string) {
        hwdec_active = property_string;
        mpv_free(property_string);
        property_string = nullptr;
    }

    m_fileInfo.video_params.fps = fps;

    MediaInfoLib::MediaInfo mi;
    mi.Open(QString2wstring(m_playingFile));
    fps = wstring2QString(mi.Get(Stream_Video, 0, __T("FrameRate"))).toDouble();

    vbitrate = wstring2QString(mi.Get(Stream_Video, 0, __T("BitRate"))).toDouble();
    abitrate = wstring2QString(mi.Get(Stream_Audio, 0, __T("BitRate"))).toDouble();

    const QString outer = "%0: %1\n", inner = "    %0: %1\n";

    QString out = outer.arg(tr("File"), fi.fileName()) +
            inner.arg(tr("Title"), m_fileInfo.media_title) +
            inner.arg(tr("File size"), Util::HumanSize(fi.size())) +
            inner.arg(tr("Date created"), fi.created().toString()) +
            inner.arg(tr("Media length"), Util::FormatTime(m_fileInfo.length, m_fileInfo.length)) + '\n';
    if(m_fileInfo.video_params.codec != QString())
        out += outer.arg(tr("Video (x%0)").arg(QString::number(m_vtracks)), m_fileInfo.video_params.codec) +
            inner.arg(tr("Video Output"), QString("%0 (hwdec %1)").arg(current_vo, hwdec_active)) +
            inner.arg(tr("Resolution"), QString("%0 x %1 (%2)").arg(QString::number(m_fileInfo.video_params.width),
                                                                    QString::number(m_fileInfo.video_params.height),
                                                                    Util::Ratio(m_fileInfo.video_params.width, m_fileInfo.video_params.height))) +
            inner.arg(tr("FPS"), QString::number(fps)) +
            inner.arg(tr("Bitrate"), tr("%0 kbps").arg(vbitrate/1000)) + '\n';
    if(m_fileInfo.audio_params.codec != QString())
        out += outer.arg(tr("Audio (x%0)").arg(QString::number(m_atracks)), m_fileInfo.audio_params.codec) +
            inner.arg(tr("Audio Output"), current_ao) +
            inner.arg(tr("Sample Rate"), QString::number(m_fileInfo.audio_params.samplerate)) +
            inner.arg(tr("Channels"), QString::number(m_fileInfo.audio_params.channels)) +
            inner.arg(tr("Bitrate"), tr("%0 kbps").arg(abitrate/1000)) + '\n';

    if(m_fileInfo.chapters.length() > 0)
    {
        out += outer.arg(tr("Chapters"), QString());
        int n = 1;
        for(auto &chapter : m_fileInfo.chapters)
            out += inner.arg(QString::number(n++), chapter.title);
        out += '\n';
    }

    if(m_fileInfo.metadata.size() > 0)
    {
        out += outer.arg(tr("Metadata"), QString());
        for(auto data = m_fileInfo.metadata.begin(); data != m_fileInfo.metadata.end(); ++data)
            out += inner.arg(data.key(), *data);
        out += '\n';
    }

    return out;
}

void MpvCore::LoadTracks()
{
    mpv_node node;
    mpv_get_property(m_mpvHandle, "track-list", MPV_FORMAT_NODE, &node);
    std::lock_guard<std::mutex> lg(m_mtxTracks);
    m_fileInfo.atracks.clear();
    m_fileInfo.vtracks.clear();
    m_fileInfo.stracks.clear();
    if(node.format == MPV_FORMAT_NODE_ARRAY)
    {
        for(int i = 0; i < node.u.list->num; i++)
        {
            if(node.u.list->values[i].format == MPV_FORMAT_NODE_MAP)
            {
                Mpv::Track track;
                for(int n = 0; n < node.u.list->values[i].u.list->num; n++)
                {
                    if(QString(node.u.list->values[i].u.list->keys[n]) == "id")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                            track.id = node.u.list->values[i].u.list->values[n].u.int64;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "type")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                        {
                            track.type = node.u.list->values[i].u.list->values[n].u.string;
                        }
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "src-id")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                            track.src_id = node.u.list->values[i].u.list->values[n].u.int64;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "title")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track.title = node.u.list->values[i].u.list->values[n].u.string;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "lang")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track.lang = node.u.list->values[i].u.list->values[n].u.string;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "albumart")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track.albumart = node.u.list->values[i].u.list->values[n].u.flag;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "default")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track._default = node.u.list->values[i].u.list->values[n].u.flag;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "external")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track.external = node.u.list->values[i].u.list->values[n].u.flag;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "external-filename")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track.external_filename = node.u.list->values[i].u.list->values[n].u.string;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "codec")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track.codec = node.u.list->values[i].u.list->values[n].u.string;
                    }
                }
                if (track.type == "video") {
                    m_vtracks++;
                    m_fileInfo.vtracks.push_back(track);
                }
                else if (track.type == "audio") {
                    m_atracks++;
                    m_fileInfo.atracks.push_back(track);
                }
                else if (track.type == "sub") {
                    m_stracks++;
                    m_fileInfo.stracks.push_back(track);
                }
            }
        }
    }
    mpv_free_node_contents(&node);
    g_core_signal->notifyTracks(m_fileInfo.vtracks + m_fileInfo.atracks + m_fileInfo.stracks);
}

void MpvCore::LoadChapters()
{
    m_fileInfo.chapters.clear();
    mpv_node node;
    mpv_get_property(m_mpvHandle, "chapter-list", MPV_FORMAT_NODE, &node);
    if(node.format == MPV_FORMAT_NODE_ARRAY)
    {
        for(int i = 0; i < node.u.list->num; i++)
        {
            if(node.u.list->values[i].format == MPV_FORMAT_NODE_MAP)
            {
                Mpv::Chapter ch;
                for(int n = 0; n < node.u.list->values[i].u.list->num; n++)
                {
                    if(QString(node.u.list->values[i].u.list->keys[n]) == "title")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            ch.title = node.u.list->values[i].u.list->values[n].u.string;
                    }
                    else if(QString(node.u.list->values[i].u.list->keys[n]) == "time")
                    {
                        if(node.u.list->values[i].u.list->values[n].format == MPV_FORMAT_DOUBLE)
                            ch.time = (int)node.u.list->values[i].u.list->values[n].u.double_;
                    }
                }
                m_fileInfo.chapters.push_back(ch);
            }
        }
    }
    mpv_free_node_contents(&node);
}

void MpvCore::LoadVideoParams()
{
    char *property_string = nullptr;
    property_string = mpv_get_property_string(m_mpvHandle, "video-codec");
    m_fileInfo.video_params.codec = property_string;
    if(property_string) {
        m_isVideo = true;
        if (m_fileInfo.video_params.codec.indexOf("jpeg") >= 0 ||
            m_fileInfo.video_params.codec.indexOf("png") >= 0) {
            // jpeg 图片，就不能去获取缩略图了，想要获取缩略图要换其他方式
            m_isVideo = false;
        }
        else {
            m_isVideo = true;
        }
        mpv_free(property_string);
    }
    mpv_get_property(m_mpvHandle, "width",        MPV_FORMAT_INT64, &m_fileInfo.video_params.width);
    mpv_get_property(m_mpvHandle, "height",       MPV_FORMAT_INT64, &m_fileInfo.video_params.height);
    mpv_get_property(m_mpvHandle, "dwidth",       MPV_FORMAT_INT64, &m_fileInfo.video_params.dwidth);
    mpv_get_property(m_mpvHandle, "dheight",      MPV_FORMAT_INT64, &m_fileInfo.video_params.dheight);
    // though this has become useless, removing it causes a segfault--no clue:
    mpv_get_property(m_mpvHandle, "video-aspect-override", MPV_FORMAT_DOUBLE, &m_fileInfo.video_params.aspect);
}

void MpvCore::LoadAudioParams()
{
    char *property_string = nullptr;
    property_string = mpv_get_property_string(m_mpvHandle, "audio-codec");
    if(property_string) {
        m_fileInfo.audio_params.codec = property_string;
        mpv_free(property_string);
    }
    mpv_node node;
    mpv_get_property(m_mpvHandle, "audio-params", MPV_FORMAT_NODE, &node);
    if(node.format == MPV_FORMAT_NODE_MAP)
    {
        for(int i = 0; i < node.u.list->num; i++)
        {
            if(QString(node.u.list->keys[i]) == "samplerate")
            {
                if(node.u.list->values[i].format == MPV_FORMAT_INT64)
                    m_fileInfo.audio_params.samplerate = node.u.list->values[i].u.int64;
            }
            else if(QString(node.u.list->keys[i]) == "channel-count")
            {
                if (node.u.list->values[i].format == MPV_FORMAT_INT64) {
                    m_fileInfo.audio_params.channels = node.u.list->values[i].u.int64;

                    // 声道设置，加载声道之后再设置声道，如果是单声道的时候设置右声道播放左声道
                    Channel((Mpv::Channel)(g_config->audioChannel.first+1));
                }
            }
        }
    }
    mpv_free_node_contents(&node);
}

void MpvCore::LoadMetadata()
{
    m_fileInfo.metadata.clear();
    mpv_node node;
    mpv_get_property(m_mpvHandle, "metadata", MPV_FORMAT_NODE, &node);
    if(node.format == MPV_FORMAT_NODE_MAP)
        for(int n = 0; n < node.u.list->num; n++)
            if(node.u.list->values[n].format == MPV_FORMAT_STRING)
                m_fileInfo.metadata[node.u.list->keys[n]] = node.u.list->values[n].u.string;
    mpv_free_node_contents(&node);
}

void MpvCore::Open(QString file, int start)
{
    // 打开之前先判断文件是否存在，如果不存在做判断
    QFile f(file);
    if(!f.exists())
    {
        KyInfo() << QString("file %1 not exit!").arg(file);
        g_core_signal->notifyFileNotExist(file);
        return;
    }

    m_lastStopTime = g_sqlite->getLastTime(file);

    if (g_playstate > 0) {
        // 如果播放状态大于 0，还原画面，不然就有问题
        Command(QStringList() << "set" << "video-rotate" << "0");
        m_rotate = 0;
        if(m_isHFlip)
        {
            Command(QStringList() << "vf" << "del" << "hflip");
            m_isHFlip = false;
        }
        if(m_isVFlip)
        {
            Command(QStringList() << "vf" << "del" << "vflip");
            m_isVFlip = false;
        }
        Command(QStringList() << "set" << "video-aspect-override" << "-1");
    }

    m_mediaInfo = QString();
    m_playingFile = file;
    m_subDelay = 0;
    m_lastTime = 0;
    m_rotate = 0;
    m_fps = 0;

    if (start > 0) {
        m_needSeek = true;
        m_seekTime = start;
    }

    // 等待停止后再进行播放
    Command(QStringList() << "stop");
    QThread::create([this]{
        while (g_playstate > 0) {
            QThread::msleep(20);
        }
        LoadOptions();
        if (m_playingFile.endsWith("iso")) {
            // iso 文件判断下有没有音频和视频流
            MediaHandle mi(m_playingFile);
            if (mi.getAudioCount() <= 0 && mi.getVideoCount() <= 0) {
                g_core_signal->notifyFileLoadedError(m_playingFile);
                return;
            }
        }
        m_vid = m_aid = m_sid = -1;
        Command(QStringList() << "loadfile" << this->m_playingFile);
    })->start();
}

void MpvCore::Play()
{
    if (g_playstate == Mpv::Playing)
        return;
    if(g_playstate > 0 && m_mpvHandle)
    {
        int f = 0;
        mpv_set_property_async(m_mpvHandle, MPV_REPLY_PROPERTY, "pause", MPV_FORMAT_FLAG, &f);
    }
}

void MpvCore::Pause()
{
    if (g_playstate == Mpv::Paused)
        return;
    if (g_playstate > 0 && m_mpvHandle) {
        int f = 1;
        mpv_set_property_async(m_mpvHandle, MPV_REPLY_PROPERTY, "pause", MPV_FORMAT_FLAG, &f);
    }
}

void MpvCore::Stop()
{
    if(g_playstate < 0)
        return;
    m_isManualStop = true;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // 如果播放状态大于 0，还原画面，不然就有问题
    // 在stop这有点多余了，需要整合到一块统一一下还原位置
    Command(QStringList() << "set" << "video-rotate" << "0");
    m_rotate = 0;
    if(m_isHFlip)
    {
        Command(QStringList() << "vf" << "del" << "hflip");
        m_isHFlip = false;
    }
    if(m_isVFlip)
    {
        Command(QStringList() << "vf" << "del" << "vflip");
        m_isVFlip = false;
    }
    Command(QStringList() << "set" << "video-aspect-override" << "-1");
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if(g_playstate > 0)
    {
        // 如果是手动停止的话，记录上次停止位置
        g_sqlite->updateLastTime(m_fileInfo.file_path, m_lastTime);
    }
    if (g_playstate > 0 && m_mpvHandle)
        Command(QStringList() << "stop");
}

void MpvCore::Seek(int pos, bool relative, bool osd)
{
    if (m_duration <= 0)
        return;
    KyInfo() << "seek to " << Functions::timeToStr(pos) << "/" << Functions::timeToStr(m_duration);
    {
        // 判断跳转是否越界（<0 或者 >视频长度）
        pos = pos < 0 ? 0 : pos;
        pos = pos > m_fileInfo.length ? m_fileInfo.length : pos;
    }
    if (g_playstate > 0)
    {
        if(relative)
        {
            const QByteArray tmp = (((pos >= 0) ? "+" : QString())+QString::number(pos)).toUtf8();
            if(osd)
            {
                const char *args[] = {"osd-msg", "seek", tmp.constData(), NULL};
                AsyncCommand(args);
            }
            else
            {
                const char *args[] = {"seek", tmp.constData(), NULL};
                AsyncCommand(args);
            }
        }
        else
        {
            if(osd)
            {
                const char *args[] = {"osd-msg", "seek", QString::number(pos).toStdString().c_str(), "absolute", NULL};
                AsyncCommand(args);
//                Command(QVariantList() << "osd-msg" << "seek" << pos << "absolute");
            }
            else
            {
                const char *args[] = {"seek", QString::number(pos).toStdString().c_str(), "absolute", NULL};
                AsyncCommand(args);
//                Command(QVariantList() << "seek" << pos << "absolute");
            }
        }
    }
}

void MpvCore::Restart()
{
    m_restartOkState = false;
    // 重启需要有间隔，不然会报错
    static int64_t restartTime = 0;
    if (QDateTime::currentMSecsSinceEpoch() - restartTime < 300) {
        m_restartOkState = true;
        return;
    }

    if (m_playingFile == QString() || g_playstate < 0) {
        m_restartOkState = true;
        return;
    }

    KyInfo() << "[Restart] file play restart...";
    m_isPauseWhenNeedSeek = g_playstate == Mpv::Paused;
    QThread::create([this](){
        std::unique_lock<std::mutex> lk(m_mtxStop);
        m_condState.wait(lk);
        m_seekTime = m_lastTime;
        if(m_seekTime > 0) {
            SetOption("volume", "0");
            m_needSeek = true;
        }
        KyInfo() << "[Restart] open file...";
        Open(m_playingFile);
        KyInfo() << "[Restart] open file command ok.";
        lk.unlock();
    })->start();

    KyInfo() << "[Restart] stop file...";
    Stop();

    // 因为 open 是非阻塞的 所以跳转要在播放状态为 加载之后.
    restartTime = QDateTime::currentMSecsSinceEpoch();
}

void MpvCore::Mute(bool m)
{
    if (m_isSinkMuteChange) {
        // 设置音量，此音量和系统同步，不单独设置mpv音量
        QDBusMessage message = QDBusMessage::createSignal("/", "org.lingmo.video", "sinkInputVolumeChanged");
        message << "lingmo-video" << m_volume << m;
        KyInfo() << "mute set send message : " << message << " | " << QDBusConnection::sessionBus().send(message);

        return;
    }
    else {
        m_isSinkMuteChange = true;
    }
    return;
}

void MpvCore::Volume(int level)
{
    if(level > 100) level = 100;
    else if(level < 0) level = 0;

    if (m_volume == level && !m_changeVolumeForce)
        return;

    m_changeVolumeForce = false;

    // 如果是 alsa 输出，直接调节 mpv 音量
    if (g_config->audioOut.first == "alsa") {
        SetOption("volume", QString::number(level));
        return;
    }

    if (m_isSinkVolumeChange) {
        // 设置音量，此音量和系统同步，不单独设置mpv音量
        QDBusMessage message = QDBusMessage::createSignal("/", "org.lingmo.video", "sinkInputVolumeChanged");
        message << "lingmo-video" << level << false;
        KyInfo() << "volume set send message : " << message << " | " << QDBusConnection::sessionBus().send(message);
        return;
    }
    else
        m_isSinkVolumeChange = true;
}

void MpvCore::Channel(Mpv::Channel c)
{
    const char *args[] = {"af", "", "", NULL};
    switch (c) {
    case Mpv::Default:
        args[1] = "set";args[2] = "";
        AsyncCommand(args);
        break;
    case Mpv::Stereo:
        args[1] = "set";args[2] = "";
        AsyncCommand(args);
        if (m_osdShow)
            ShowText(tr("Stereo"));
        break;
    case Mpv::Left:
        args[1] = "set";args[2] = "lavfi=[pan=stereo|c0=c0|c1=0*c1]";
        AsyncCommand(args);
        if (m_osdShow)
            ShowText(tr("Left Channel"));
        break;
    case Mpv::Right:
        args[1] = "set";
        args[2] = m_fileInfo.audio_params.channels == 1 ?
                    "lavfi=[pan=stereo|c0=0*c0|c1=c0]" :
                    "lavfi=[pan=stereo|c0=0*c0|c1=c1]";
        AsyncCommand(args);
        if (m_osdShow)
            ShowText(tr("Right Channel"));
        break;
    default:
        break;
    }

    g_config->audioChannel.second = (int)c - 1;
    g_config->flushChange();
}

void MpvCore::Speed(double d)
{
    if (g_playstate > 0) {
        Command(QStringList() << "set" << "speed" << QString::number(d));
    }
    else {
        SetOption("speed", QString::number(d));
    }
}

void MpvCore::SpeedUp()
{
    if (g_playstate > 0 && m_speed < 1.9)
    {
        if(m_speed > 1.4)
            m_speed += 0.5;
        else
            m_speed += 0.25;
    }
    Speed(m_speed);
}

void MpvCore::SpeedDown()
{
    if (g_playstate > 0 && m_speed > 0.6)
    {
        if(m_speed > 1.9)
            m_speed -= 0.5;
        else
            m_speed -= 0.25;
    }
    Speed(m_speed);
}

void MpvCore::BrightnessUp()
{
    // 如果是jjw显卡，vdpau 和 vaapi 亮度调节不生效，提示一下
    if ((g_config->hardwareType() == GlobalConfig::JM7X_VDPAU || g_config->hardwareType() == GlobalConfig::JM9X_VAAPI) && g_config->videoOutput.first != "x11") {
        ShowText(tr("not support brightness set."));
        return;
    }
    if (m_brightness < 100)
    {
        m_brightness += 2;
        g_settings->setValue("General/brightness", m_brightness);
    }
    Command(QStringList() << "set" << "brightness" << QString::number(m_brightness));
    ShowText(tr("brightness : %1").arg((m_brightness + 100) / 2));
}

void MpvCore::BrightnessDown()
{
    // 如果是jjw显卡，vdpau 和 vaapi 亮度调节不生效，提示一下
    if ((g_config->hardwareType() == GlobalConfig::JM7X_VDPAU ||
         g_config->hardwareType() == GlobalConfig::JM9X_VAAPI) && g_config->videoOutput.first != "x11") {
        ShowText(tr("not support brightness set."));
        return;
    }
    if (m_brightness > -100)
    {
        m_brightness -= 2;
        g_settings->setValue("General/brightness", m_brightness);
    }
    Command(QStringList() << "set" << "brightness" << QString::number(m_brightness));
    ShowText(tr("brightness : %1").arg((m_brightness + 100) / 2));
}

void MpvCore::SubId(int id)
{
    Command(QStringList() << "set" << "sid" << QString::number(id));
}

void MpvCore::AudioId(int id)
{
    const char *args[] = {"set", "aid", QString("%1").arg(id).toStdString().c_str(), NULL};
    AsyncCommand(args);
}

void MpvCore::AudioNext()
{
    bool use_next = false;
    foreach (auto track, m_fileInfo.atracks) {
        if (use_next) {
            Command(QStringList() << "set" << "aid" << QString::number(track.id));
            break;
        }
        if (track.id == m_aid) {
            use_next = true;
            if (track == m_fileInfo.atracks.last()) {
                Command(QStringList() << "set" << "aid" << QString::number(m_fileInfo.atracks.first().id));
            }
        }
    }

#if 0
    if (m_currentAid < m_atracks) {
        const char *args[] = {"set", "aid", QString("%1").arg(m_currentAid++).toStdString().c_str(), NULL};
        AsyncCommand(args);
    }
    else {
        const char *args[] = {"set", "aid", QString("%1").arg(m_aid).toStdString().c_str(), NULL};
        AsyncCommand(args);
    }
#endif
}

void MpvCore::NextFrame()
{
    const char *args[] = {"frame_step", NULL};
    AsyncCommand(args);
}

void MpvCore::PrevFrame()
{
    const char *args[] = {"frame_back_step", NULL};
    AsyncCommand(args);
}

void MpvCore::Aspect(Mpv::VideoAspect aspect)
{
    if (m_vid < 0) {
        return;
    }
    QString arg;
    QString str_aspect = "";
    switch (aspect) {
    case Mpv::AUTO:
        arg = "-1";
        str_aspect = tr("default");
        break;
    case Mpv::DIV_4_3:
        arg = "4:3";
        str_aspect = "4/3";
        break;
    case Mpv::DIV_16_9:
        arg = "16:9";
        str_aspect = "16/9";
        break;
    case Mpv::FULL:
        mpv_set_option_string(m_mpvHandle, "keepaspect", "no");
        ShowText(tr("aspect ratio").append(":").append(tr("fill window")));
        return;
    }
    mpv_set_option_string(m_mpvHandle, "keepaspect", "yes");
    SetAspect(arg);
    ShowText(tr("aspect ratio").append(":").append(str_aspect));
}

void MpvCore::SetAspect(QString scale)
{
    Command(QStringList() << "set" << "video-aspect-override" << scale);
}

void MpvCore::RestoreFrame()
{
    mpv_set_option_string(m_mpvHandle, "keepaspect", "yes");
    Command(QStringList() << "set" << "video-rotate" << "0");
    m_rotate = 0;
    if(m_isHFlip)
    {
        Command(QStringList() << "vf" << "del" << "hflip");
        m_isHFlip = false;
    }
    if(m_isVFlip)
    {
        Command(QStringList() << "vf" << "del" << "vflip");
        m_isVFlip = false;
    }
    Command(QStringList() << "set" << "video-aspect-override" << "-1");
    ShowText(tr("restore frame"));
}

void MpvCore::ClockwiseRotate()
{
    if (!m_isVideo || m_vid < 0) {
        return;
    }
    m_rotate = (m_rotate + 90) % 360;
    Command(QStringList() << "set" << "video-rotate" << QString::number(m_rotate));
    ShowText(tr("clockwise rotation"));
}

void MpvCore::CounterClockwiseRotate()
{
    if (!m_isVideo || m_vid < 0) {
        return;
    }
    if(m_rotate >= 90)
        m_rotate = (m_rotate - 90) % 360;
    else
        m_rotate = 270;
    Command(QStringList() << "set" << "video-rotate" << QString::number(m_rotate));
    ShowText(tr("counterclockwise rotation"));
}

void MpvCore::FlipHorizontally()
{
    if (!m_isVideo || m_vid < 0) {
        return;
    }
    QString s = m_isHFlip ? "del" : "add";
    if (m_mpvHandle && g_playstate > 0)
    {
        Command(QStringList() << "vf" << s << "hflip");
        ShowText(tr("Horizontal Flip: ").append(m_isHFlip ? tr("close") : tr("open")));
    }
    m_isHFlip = !m_isHFlip;
}

void MpvCore::FlipVertically()
{
    if (!m_isVideo || m_vid < 0) {
        return;
    }
    QString s = m_isVFlip ? "del" : "add";
    if (m_mpvHandle && g_playstate > 0)
    {
        Command(QStringList() << "vf" << s << "vflip");
        ShowText(tr("Vertical Flip: ").append(m_isVFlip ? tr("close") : tr("open")));
    }
    m_isVFlip = !m_isVFlip;
}

void MpvCore::AddSub(QString sub)
{
    // 0 为没有字幕
    int sub_index = m_subs.indexOf(sub) + 1;
    KyInfo() << "add sub " << sub;
    if(sub_index > 0) {
        SubId(sub_index);
        return;
    }
    Command(QStringList() << "sub_add" << sub);
    m_subs.push_back(sub);
    LoadTracks();
}

void MpvCore::AddSubs(QStringList subs)
{
    for(auto sub : subs)
    {
        AddSub(sub);
    }
}

void MpvCore::SubMoveUp()
{
    if(m_subPos > 0)
        m_subPos -= 1;
    else
        return;
    SetOption("sub-pos", QString::number(m_subPos));
}

void MpvCore::SubMoveDown()
{
    if(m_subPos < 100)
        m_subPos += 1;
    else
        return;
    SetOption("sub-pos", QString::number(m_subPos));
}

void MpvCore::SubNext()
{
    bool use_next = false;
    foreach (auto track, m_fileInfo.stracks) {
        if (use_next) {
            Command(QStringList() << "set" << "sid" << QString::number(track.id));
            break;
        }
        if (track.id == m_sid) {
            use_next = true;
            if (track == m_fileInfo.stracks.last()) {
                Command(QStringList() << "set" << "sid" << QString::number(0));
            }
        }
    }
    if (!use_next && m_fileInfo.stracks.size() > 0) {
        Command(QStringList() << "set" << "sid" << QString::number(m_fileInfo.stracks.first().id));
    }

#if 0
    if(m_sid < m_stracks)
        Command(QStringList() << "set" << "sid" << QString::number(m_sid+1));
    else
        Command(QStringList() << "set" << "sid" << QString::number(0));
#endif
}

void MpvCore::SubForward()
{
    m_subDelay -= 0.5;
    Command(QStringList() << "set" << "sub-delay" << QString::number(m_subDelay));
    ShowText(tr("subtitle delay : %1s").arg(m_subDelay));
}

void MpvCore::SubBackward()
{
    m_subDelay += 0.5;
    Command(QStringList() << "set" << "sub-delay" << QString::number(m_subDelay));
    ShowText(tr("subtitle delay : %1s").arg(m_subDelay));
}

void MpvCore::SubFontSize(int size)
{
    SetOption("sub-font-size", QString("%1").arg(size));
}

void MpvCore::SubFontFamily(QString family)
{
    SetOption("sub-font", family);
}

void MpvCore::AddBookMark(QString desc)
{
    QFileInfo fi(m_playingFile);
    if (!fi.exists() || g_playstate < 0) {
        ShowText(tr("Add mark error"));
        return;
    }
    // 第一秒和最后一秒不能添加书签
    if ((int)m_lastTime <= 0 || (int)m_lastTime >= (int)m_duration || m_marks.find((int)m_lastTime) != m_marks.end())
        return;
    KyInfo() << "add book mark " << m_lastTime;
    // 获取一个截图，作为预览图
    // 判断预览文件夹是否存在
    QThread::create([this, desc](){
        int mark_time = m_lastTime;
        m_marks.insert(mark_time);
        KyInfo() << "insert mark " << mark_time << "s";
        // 获取书签数据表名字，如果文件夹不存在的话创建文件夹
        QString mark_dir = Paths::configPath().append("/").append(g_sqlite->getMarkCharacter(m_playingFile));
        QDir d;
        if (!d.exists(mark_dir)) {
            if (!d.mkdir(mark_dir)) {
                qDebug() << "create " << mark_dir << " error";
                return;
            }
        }
        QString mark_view = mark_dir.append("/").append(QString::number(m_lastTime)).append(".png");

        // 如果只有音频的话预览图为默认图片
        if (m_vid < 0 || !m_isVideo)
            mark_view = ":/ico/no-preview.png";
        else {
            // 保存当前图片
            QProcess p;
            QString cmd_str = QString("ffmpegthumbnailer -i %1 -o %2 -t %3")
                                .arg("\"" + m_playingFile + "\"")
                                .arg(mark_view)
                                .arg(Functions::timeToStr(m_lastTime));
            p.start(cmd_str);
            p.waitForFinished();
        }
        g_core_signal->sigMarkAdded(m_playingFile, m_lastTime, desc, mark_view);
    })->start();

    ShowText(tr("Add mark ok").append(" : ").append(Functions::timeToStr(m_lastTime)));
}

void MpvCore::ScreenShot(bool sub)
{
    if(m_vid < 0)
        return;
    QDir dir(m_screenshotDir);
    if(m_canSaveScreenShot && dir.exists())
        ShowText(tr("ScreenShot OK"));
    else {
        ShowText(tr("ScreenShot Failed, folder has no write permission or folder not exit."));
        return;
    }
    const char *args[] = {"screenshot", (sub ? "subtitles" : (g_config->screenShotCurrentSize.first ? "window" : "video")), NULL};
    AsyncCommand(args);
}

void MpvCore::ScreenshotFormat(QString format)
{
    SetOption("screenshot-format", format);
}

void MpvCore::ScreenshotDirectory(QString dir)
{
    SetOption("screenshot-directory", dir);
    m_screenshotDir = dir;
    // 如果文件夹没有权限的话，每次截图都报错就行
    QFile fi(dir.append("/.test"));
    if(!fi.open(QIODevice::ReadWrite)) {
        m_canSaveScreenShot = false;
    }
    else {
        // 删除创建的临时文件
        QProcess::execute(QString("rm -rf ").append(dir));
        m_canSaveScreenShot = true;
    }
}

void MpvCore::ShowInfo(bool show)
{
    if(show)
    {
        m_showProfile = true;
        m_showInfoTimer->start();
    }
    else
    {
        m_showProfile = false;
        m_showInfoTimer->stop();
    }
    ShowText("");
}

// -
void MpvCore::LoadFileInfo()
{
    std::lock_guard<std::mutex> lg(m_mtxFileInfo);
    char *property_string = nullptr;
    m_isHFlip = false;
    m_isVFlip = false;
    m_vtracks = m_atracks = m_stracks = 0;
    m_fileInfo.file_path = m_playingFile;

    // 文件大小
    QFileInfo fi(m_playingFile);
    m_fileInfo.file_size = Functions::humanSize(fi.size());
    m_fileInfo.file_type = m_playingFile.split(".").last();

    // get media-title
    property_string = mpv_get_property_string(m_mpvHandle, "media-title");
    if(property_string) {
        m_fileInfo.media_title = property_string;
        mpv_free(property_string);
    }

    QThread::create([this]{
        // 时长获取，mpv特殊格式获取失败，用mediainfo获取时长
        MediaHandle mh;
        mh.setFile(m_playingFile);
        double tmp_duration = (double)mh.getDuration()/1000.0;
        if (tmp_duration > 0) {
            m_duration = tmp_duration;
            g_core_signal->notifyDuration(m_playingFile, m_duration);
            m_fileInfo.length = m_duration;
        }
    })->start();

    LoadTracks();
    LoadChapters();
    LoadVideoParams();
    LoadAudioParams();
    LoadMetadata();

    // 通知其他组件播放的文件改变了
    g_core_signal->notifyFileInfo(m_fileInfo);

    mpv_get_property(m_mpvHandle, "vid", MPV_FORMAT_INT64, &m_vid);
    g_core_signal->notifyVideoId(m_vid);
    mpv_get_property(m_mpvHandle, "sid", MPV_FORMAT_INT64, &m_sid);
    g_core_signal->notifySubId(m_sid);
    mpv_get_property(m_mpvHandle, "aid", MPV_FORMAT_INT64, &m_aid);
    g_core_signal->notifyAudioId(m_aid);
    mpv_get_property(m_mpvHandle, "sub-pos", MPV_FORMAT_INT64, &m_subPos);

    m_marks.clear();
    QVector<MarkRecord> vec_mark = g_sqlite->getMarks();
    for (MarkRecord mark : vec_mark) {
        if (mark.path == m_playingFile) {
            m_marks.insert(mark.pos);
        }
    }
}

void MpvCore::SetProperties()
{
    Speed(g_settings->value("General/speed").toDouble());
    // 声道设置
    m_osdShow = false;
    Channel((Mpv::Channel)(g_config->audioChannel.first+1));
    m_osdShow = true;

    // 需要设置一下音量，不然会和音乐音量冲突
    QTimer::singleShot(100, [this]{
        QDBusMessage message = QDBusMessage::createSignal("/", "org.lingmo.video", "sinkInputVolumeChanged");
        message << "lingmo-video" << m_volume << m_mute;
        KyInfo() << "file start send message : " << message << " | " << QDBusConnection::sessionBus().send(message);
    });
}

void MpvCore::slotVolumeChange(QString app, int value, bool mute)
{
    if (app == "lingmo-video-990" || app == "lingmo-video") {
        if (value < 0) {
            return;
        }

        KyInfo("slotVolumeChange ,volume:%d value:%d,mute:%d ,this->mute:%d", m_volume, value, mute, m_mute);
        m_isSinkVolumeChange = false;
        m_isSinkMuteChange = false;
        if (value != m_volume)
            ShowText(QString(tr("volume : %1")).arg(value));
        if (mute != m_mute) {
            m_mute = mute;
            g_core_signal->notifyMute(mute);
            ShowText(mute ? tr("Mute") : tr("Cancel Mute"));
        }
        // 如果是重启之后有了音量信号，则是pulse主动发过来的
        if (m_restartOkState) {
            m_isSinkVolumeChange = true;
            m_changeVolumeForce = true;
            Volume(m_volume);
            m_restartOkState = false;
        }
        else
            m_volume = value;
        g_core_signal->notifyVolume(value);
        m_isSinkVolumeChange = true;
        m_isSinkMuteChange = true;
    }
}

void MpvCore::initMpvHandle()
{
    m_mpvHandle = mpv::qt::Handle::FromRawHandle(mpv_create());
    if (!m_mpvHandle)
    {
        KyInfo("could not create mpv context");
        return;
    }
    if (mpv_initialize(m_mpvHandle) < 0)
    {
        KyInfo("could not initialize mpv context");
    }

    mpv_set_option_string(m_mpvHandle, "terminal", "yes");
    mpv_set_option_string(m_mpvHandle, "msg-level", "all=v");
    mpv_set_option_string(m_mpvHandle, "input-cursor", "no");
    mpv_set_option_string(m_mpvHandle, "hwdec-codecs", "all");
    mpv_set_option_string(m_mpvHandle, "gpu-context", "x11egl");
    mpv_set_option_string(m_mpvHandle, "cursor-autohide", "no");
    mpv_set_option_string(m_mpvHandle, "input-vo-keyboard", "no");
    mpv_set_option_string(m_mpvHandle, "input-default-bindings ", "no");

    mpv_observe_property(m_mpvHandle, 0, "mute",          MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpvHandle, 0, "hwdec",         MPV_FORMAT_STRING);
    mpv_observe_property(m_mpvHandle, 0, "speed",         MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpvHandle, 0, "avsync",        MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpvHandle, 0, "volume",        MPV_FORMAT_INT64);
    mpv_observe_property(m_mpvHandle, 0, "duration",      MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpvHandle, 0, "time-pos",      MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpvHandle, 0, "video-bitrate", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpvHandle, 0, "audio-bitrate", MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpvHandle, 0, "paused-for-cache",MPV_FORMAT_FLAG);
    mpv_set_wakeup_callback(m_mpvHandle, wakeup, this);

    if (g_config->videoOutputType() == GlobalConfig::VO_WID)
        SetOption("wid", QString::number(m_playWidget->winId()));
    else {
        SetOption("vo", "opengl-cb");
        dynamic_cast<PlayGLWidget*>(m_playWidget)->setMpvHandle(m_mpvHandle);
    }
    SetOption("vd", "h264_omx,h265_omx,");

    m_volume = g_settings->value("General/volume").toInt();
    m_brightness = g_settings->value("General/brightness").toInt();

    // osd显示文件信息
    m_showInfoTimer = new QTimer;
    m_showInfoTimer->setInterval(500);
    connect(m_showInfoTimer, &QTimer::timeout, [&](){
        ShowText("");
    });

    m_showBufferingTimer = new QTimer;
    m_showBufferingTimer->setInterval(500);
    connect(m_showBufferingTimer, &QTimer::timeout, [&](){
        ShowText(tr("Buffering..."));
    });
}

void MpvCore::initGlobalSig()
{
    connect(g_user_signal, &GlobalUserSignal::sigForword, [&](bool b){
        int seekto;
        if(b) seekto = m_lastTime+10 > m_fileInfo.length ? m_fileInfo.length : m_lastTime+10;
        else  seekto = m_lastTime+30 > m_fileInfo.length ? m_fileInfo.length : m_lastTime+30;
        Seek(seekto);
        if (seekto == 0)
            ShowText(QString("▶▶ 00:00:00"));
        else
            ShowText(QString("▶▶ ").append(Functions::timeToStr((double)seekto, false)));
    });

    connect(g_user_signal, &GlobalUserSignal::sigBackword, [&](bool b){
        int seekto;
        if(b) seekto = m_lastTime-10<0 ? 0 : m_lastTime-10;
        else  seekto = m_lastTime-30<0 ? 0 : m_lastTime-30;
        Seek(seekto);
        if (seekto == 0)
            ShowText(QString("◀◀ 00:00:00"));
        else
            ShowText(QString("◀◀ ").append(Functions::timeToStr((double)seekto, false)));
    });

    connect(g_user_signal, &GlobalUserSignal::sigAudioNext, this, &MpvCore::AudioNext);
    connect(g_user_signal, &GlobalUserSignal::sigVideoDecoder, this, &MpvCore::VideoDecoder);
    connect(g_user_signal, &GlobalUserSignal::sigVideoOutput, this, &MpvCore::VideoOutput);
    connect(g_user_signal, &GlobalUserSignal::sigVideoDecodeThread, this, &MpvCore::DecodeThreads);
    connect(g_user_signal, &GlobalUserSignal::sigChannel, this, &MpvCore::Channel);
    connect(g_user_signal, &GlobalUserSignal::sigSeek, [&](int time){Seek(time);});
    connect(g_user_signal, &GlobalUserSignal::sigVolume, this, &MpvCore::Volume);
    connect(g_user_signal, SIGNAL(sigMute(bool)), this, SLOT(Mute(bool)));
    connect(g_user_signal, &GlobalUserSignal::sigVolumeUp, [&](int up){Volume(m_volume+up);});
    connect(g_user_signal, &GlobalUserSignal::sigVolumeDown, [&](int down){Volume(m_volume-down);});
    connect(g_user_signal, &GlobalUserSignal::sigAspect, this, &MpvCore::Aspect);
    connect(g_user_signal, &GlobalUserSignal::sigRestoreFrame, this, &MpvCore::RestoreFrame);
    connect(g_user_signal, &GlobalUserSignal::sigHorizontallyFlip, this, &MpvCore::FlipHorizontally);
    connect(g_user_signal, &GlobalUserSignal::sigVerticalFlip, this, &MpvCore::FlipVertically);
    connect(g_user_signal, &GlobalUserSignal::sigClockwiseRotate, this, &MpvCore::ClockwiseRotate);
    connect(g_user_signal, &GlobalUserSignal::sigCounterClockwiseRotate, this, &MpvCore::CounterClockwiseRotate);
    connect(g_user_signal, &GlobalUserSignal::sigBrightnessUp, this, &MpvCore::BrightnessUp);
    connect(g_user_signal, &GlobalUserSignal::sigBrightnessDown, this, &MpvCore::BrightnessDown);
    connect(g_user_signal, &GlobalUserSignal::sigRestart, this, &MpvCore::Restart);
    connect(g_user_signal, &GlobalUserSignal::sigStop, this, &MpvCore::Stop);
    connect(g_user_signal, &GlobalUserSignal::sigPlay, this, &MpvCore::Play);
    connect(g_user_signal, &GlobalUserSignal::sigPause, this, &MpvCore::Pause);
    connect(g_user_signal, &GlobalUserSignal::sigNFrame, this, &MpvCore::NextFrame);
    connect(g_user_signal, &GlobalUserSignal::sigPFrame, this, &MpvCore::PrevFrame);
    connect(g_user_signal, &GlobalUserSignal::sigSpeed, this, &MpvCore::Speed);
    connect(g_user_signal, &GlobalUserSignal::sigSpeedUp, this, &MpvCore::SpeedUp);
    connect(g_user_signal, &GlobalUserSignal::sigSpeedDown, this, &MpvCore::SpeedDown);
    connect(g_user_signal, &GlobalUserSignal::sigAudioId, this, &MpvCore::AudioId);
    connect(g_user_signal, &GlobalUserSignal::sigSubId, this, &MpvCore::SubId);
    connect(g_user_signal, &GlobalUserSignal::sigAddSub, this, &MpvCore::AddSub);
    connect(g_user_signal, &GlobalUserSignal::sigSubUp, this, &MpvCore::SubMoveUp);
    connect(g_user_signal, &GlobalUserSignal::sigSubDown, this, &MpvCore::SubMoveDown);
    connect(g_user_signal, &GlobalUserSignal::sigSubNext, this, &MpvCore::SubNext);
    connect(g_user_signal, &GlobalUserSignal::sigSubForward, this, &MpvCore::SubForward);
    connect(g_user_signal, &GlobalUserSignal::sigSubBackward, this, &MpvCore::SubBackward);
    connect(g_user_signal, &GlobalUserSignal::sigSubSize, this, &MpvCore::SubFontSize);
    connect(g_user_signal, &GlobalUserSignal::sigSubFont, this, &MpvCore::SubFontFamily);
    connect(g_user_signal, &GlobalUserSignal::sigAddBookMark, this, &MpvCore::AddBookMark);
    connect(g_user_signal, &GlobalUserSignal::sigScreenShot, this, &MpvCore::ScreenShot);
    connect(g_user_signal, &GlobalUserSignal::sigScreenShotDir, this, &MpvCore::ScreenshotDirectory);
    connect(g_user_signal, &GlobalUserSignal::sigScreenShotFormat, this, &MpvCore::ScreenshotFormat);
    connect(g_user_signal, &GlobalUserSignal::sigShowInfo, this, &MpvCore::ShowInfo);
    connect(g_core_signal, &GlobalCoreSignal::sig10FrameUseTime, [&](qint64 use_time){
        if (g_playstate == Mpv::Paused)
            return;
        m_fps = 10000.0 / (double)use_time;
        m_fps = ((double)((int)((m_fps+0.005)*100)))/100;
    });
}

void MpvCore::initVolumeDBus()
{
    // 音量全局控制
    QDBusConnection::sessionBus().connect(QString(), "/", "org.lingmo.media", "sinkVolumeChanged", this, SLOT(slotVolumeChange(QString,int,bool)));
    // 初始化静音状态和音量
    m_mute = g_settings->value("General/mute").toBool();
    m_volume = g_settings->value("General/volume").toInt();

    g_core_signal->notifyMute(m_mute);
    g_core_signal->notifyVolume(m_volume);
}

void MpvCore::LoadOptions()
{
    // 播放之前先设置参数
    // 1.截图格式和截图存放位置
    SetOption("screenshot-format", g_config->screenShotFormat.first);
    SetOption("screenshot-template", "cap_%F_%p_%02n");

    SetOption("screenshot-directory", g_config->screenShotPath.first == ""
              ? QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
              : g_config->screenShotPath.first);

    // 2.设置字幕
    SetOption("sub-font", g_config->subFontFamily.first);
    SetOption("sub-font-size", QString::number(g_config->subFontSize.first));

    // 3.设置音频输出驱动
    SetOption("ao", g_config->audioOut.first);

    // 4.设置倍速
    SetOption("speed", g_settings->value("General/speed").toString());

    SetOption("osd-level", "2");
    SetOption("osd-msg2", " ");
    SetOption("osd-font-size", QString::number(27 * qApp->devicePixelRatio()));
    // 设置字体大小不随窗口变化而变化，否则 mini 模式osd显示字体特别小
    SetOption("osd-scale-by-window", "no");

    SetOption("brightness", QString::number(m_brightness));

    // 设置视频解码相关
    if (g_config->hardwareType() == GlobalConfig::INTEL_IrisXe_VAAPI && m_playingFile.endsWith(".ts")) {
        // 如果时 IrisXe 显卡并是 ts 格式的视频走软解
        KyInfo() << ".ts file set hwdec no";
        SetOption("hwdec", "no");
    }
    else if (g_config->videoDecoder.first == tr("default") || g_config->videoDecoder.first == "default") {
        SetOption("hwdec", "default,vaapi,vdpau,");
        if (g_config->hardwareType() == GlobalConfig::X100_GPU)
            SetOption("hwdec", "vdpau");
    }
    else {
        SetOption("hwdec", g_config->videoDecoder.first);
    }

    if (g_config->videoOutputType() == GlobalConfig::VO_WID) {
#if 1
    if (g_config->videoOutput.first == tr("auto") || g_config->videoOutput.first == "auto") {
        if (g_config->hardwareType() == GlobalConfig::JM7X_VDPAU)
            SetOption("vo", "vdpau,xv,x11");
        else if (g_config->hardwareType() == GlobalConfig::JM9X_VAAPI)
            SetOption("vo", "vaapi,xv,x11");
        else
            SetOption("vo", "");
#ifdef __loongarch__
        SetOption("vo", "x11,");
#endif
#ifdef __mips64
        SetOption("vo", "x11,");
#endif
        if (g_config->videoOutputType() == GlobalConfig::VO_WID)
            SetOption("vo", "");
    }
    else {
        SetOption("vo", g_config->videoOutput.first + ",vaapi,vdpau,x11,xv");
#ifdef __loongarch__
        SetOption("vo", g_config->videoOutput.first + ",x11,");
#endif
#ifdef __mips64
        SetOption("vo", g_config->videoOutput.first + ",x11,");
#endif
    }
#endif
    }

    SetOption("vd-lavc-threads", QString::number(g_config->videoDecodeThreads.first));
    if (g_config->audioOut.first == "alsa")
        SetOption("volume", QString::number(g_settings->value("General/volume").toInt()));
    else
        SetOption("volume", "100");

    SetOption("sub-file-paths", g_config->subDir.first);
    // 是否自动加载字幕
    SetOption("sub-auto", g_config->loadSameNameSub.first ? "exact" : "no");
}

void MpvCore::SetOption(QString key, QString val)
{
    QByteArray tmp1 = key.toUtf8(),
               tmp2 = val.toUtf8();
    mpv::qt::set_option_variant(m_mpvHandle, tmp1.constData(), tmp2.constData());
}

QVariant MpvCore::Command(const QVariant &params)
{
    if(m_mpvHandle)
        return mpv::qt::command_variant(m_mpvHandle, params);
    return QVariant();
}

int MpvCore::AsyncCommand(const char *args[])
{
    if(m_mpvHandle)
        return mpv_command_async(m_mpvHandle, MPV_REPLY_COMMAND, args);
    return -1;
}

bool MpvCore::event(QEvent *event)
{
    if(event->type() == QEvent::User)
    {
        int64_t sid, vid, aid;
        while(m_mpvHandle)
        {
            mpv_event *event = mpv_wait_event(m_mpvHandle, 0);
            if(event == nullptr || event->event_id == MPV_EVENT_NONE)
            {
                break;
            }
            switch (event->event_id)
            {
            case MPV_EVENT_PROPERTY_CHANGE:
            {
                mpv_event_property *prop = (mpv_event_property*)event->data;
                if(QString(prop->name) == "video-bitrate" && prop->format == MPV_FORMAT_DOUBLE)
                {
                    m_videoBitrate = *(double*)prop->data;
                }
                else if(QString(prop->name) == "audio-bitrate" && prop->format == MPV_FORMAT_DOUBLE)
                {
                    m_audioBitrate = *(double*)prop->data;
                }
                else if(QString(prop->name) == "avsync" && prop->format == MPV_FORMAT_DOUBLE)
                {
                    m_avsync = *(double*)prop->data;
                    if(m_avsync < 0)
                        m_avsync = -m_avsync;
                    m_avsync = ((double)(int)(m_avsync * 10000 * 1000 + 0.5)) / 1000;
                }
                else if(QString(prop->name) == "duration") // playback-time does the same thing as time-pos but works for streaming media
                {
                    if(prop->format == MPV_FORMAT_DOUBLE)
                    {
                        double tmp_duration = *(double*)prop->data;
                        if (m_duration <= 0) {
                            m_duration = g_sqlite->getDuration(m_playingFile);
                        }

                        if (tmp_duration > m_duration) {
                            m_duration = tmp_duration;
                            m_fileInfo.length = m_duration;
                            g_core_signal->notifyDuration(m_playingFile, m_duration);
                        }

                        // 重启之后是否需要跳转，获取到总长度才能跳转
                        if (m_needSeek) {
                            Seek(m_seekTime);
                            if (m_isPauseWhenNeedSeek)
                                Pause();
                            if (g_config->audioOut.first == "alsa")
                                SetOption("volume", QString::number(m_volume));
                            else
                                SetOption("volume", "100");
                            m_restartOkState = true;
                        }
                        m_needSeek = false;
                    }
                    // 从上次停止位置播放，收到时长改变才能跳转，不然会跳转失败
                    if (g_config->playLastPos.first) {
                        Seek(m_lastStopTime);
                    }
                }
                else if(QString(prop->name) == "time-pos") // playback-time does the same thing as time-pos but works for streaming media
                {
                    if(prop->format == MPV_FORMAT_DOUBLE)
                    {
                        m_lastTime = *(double*)prop->data;
                        g_core_signal->notifyCurrentTime(m_lastTime);
                    }
                }
                else if(QString(prop->name) == "volume")
                {
#if 1
                    if (g_config->audioOut.first == "alsa") {
                        if (prop->format == MPV_FORMAT_INT64)
                        {
                            m_volume = *(int*)prop->data;
                            g_core_signal->notifyVolume(m_volume);
                            ShowText(QString(tr("volume : %1")).arg(*(int*)prop->data));
                        }
                    }
#endif
                }
                else if(QString(prop->name) == "speed")
                {
                    m_speed = *(double*)prop->data;
                    g_core_signal->notifySpeed(m_speed);
                    ShowText(QString(tr("speed : %1x")).arg(*(double*)prop->data));
                }
                else if(QString(prop->name) == "sid")
                {
                    if(prop->format == MPV_FORMAT_INT64)
                    {
//                        if(prop->format == MPV_FORMAT_INT64)
//                            m_sid = *(int*)prop->data;
                    }
                }
                else if(QString(prop->name) == "aid")
                {
                    if(prop->format == MPV_FORMAT_INT64)
                        m_aid = *(int*)prop->data;
                }
                else if(QString(prop->name) == "sub-visibility")
                {
                    //if(prop->format == MPV_FORMAT_FLAG)
                        //setSubtitleVisibility((bool)*(unsigned*)prop->data);
                }
                else if(QString(prop->name) == "mute")
                {
                    if(prop->format == MPV_FORMAT_FLAG)
                    {
                        g_core_signal->notifyMute((bool)*(unsigned*)prop->data);
                        ShowText((bool)*(unsigned*)prop->data ? tr("Mute") : tr("Cancel Mute"));
                    }
                }
                else if(QString(prop->name) == "core-idle")
                {
                    if(prop->format == MPV_FORMAT_FLAG)
                    {
                        //if(prop->format == MPV_FORMAT_FLAG)
                            //setSubtitleVisibility((bool)*(unsigned*)prop->data);
                    }
                }
                else if(QString(prop->name) == "paused-for-cache")
                {
                    if (prop->format == MPV_FORMAT_FLAG)
                    {
                        if ((bool)*(unsigned*)prop->data && g_playstate == Mpv::Playing) {
                            // 需要循环一直显示直到缓冲完
                            m_showBufferingTimer->start();
                        }
                        else {
                            m_showBufferingTimer->stop();
                            ShowText(QString());
                        }
                    }
                }
                else if (QString(prop->name) == "hwdec") {
                    KyInfo() << "hwdec change " << QString::fromLocal8Bit(mpv_get_property_string(m_mpvHandle, "hwdec"));
                }
                break;
            }
            case MPV_EVENT_IDLE:
                KyInfo() << "[mpv event] MPV_EVENT_IDLE";
                m_fileInfo.length = 0;
                g_playstate = Mpv::Idle;
                g_core_signal->notifyPlayState();
                g_core_signal->notifyCurrentTime(0);
                break;
                // these two look like they're reversed but they aren't. the names are misleading.
            case MPV_EVENT_START_FILE:
                KyInfo() << "[mpv event] MPV_EVENT_START_FILE";
                g_playstate = Mpv::Started;
                g_core_signal->notifyPlayState();
                break;
            case MPV_EVENT_FILE_LOADED:
                KyInfo() << "[mpv event] MPV_EVENT_FILE_LOADED";
                m_isLoaded = true;
                LoadFileInfo();
                SetProperties();
                m_isManualStop = false;
                g_playstate = Mpv::Loaded;
                g_core_signal->notifyPlayState();
                //加载之后上次停止时间需要设置为0
                g_sqlite->updateLastTime(m_playingFile, 0);

                Play();
                ShowText("");
                // 不要 break 有莫名其妙的问题，待查
            case MPV_EVENT_UNPAUSE:
                KyInfo() << "[mpv event] MPV_EVENT_UNPAUSE";
                // 防止重复设置状态
                if(g_playstate == Mpv::Playing || g_playstate < 0)
                    break;
                if(g_playstate == Mpv::Paused)
                    ShowText(tr("Playing"));
                g_playstate = Mpv::Playing;
                g_core_signal->notifyPlayState();
                break;
            case MPV_EVENT_PAUSE:
                KyInfo() << "[mpv event] MPV_EVENT_PAUSE";
                if (g_playstate == Mpv::Paused || g_playstate < 0)
                    break;
                g_playstate = Mpv::Paused;
                g_core_signal->notifyPlayState();
                if (m_duration >= 0)
                    QTimer::singleShot(100, [this](){ShowText(tr("Paused"));});
                break;
            case MPV_EVENT_END_FILE:
                KyInfo() << "[mpv event] MPV_EVENT_END_FILE";
                m_duration = -1;
                // 如果没有 loaded 说明文件打开失败
                if (!m_isLoaded) {
                    g_core_signal->notifyFileLoadedError(m_playingFile);
                    m_lastTime = 0;
                }
                m_condState.notify_one();
                m_isLoaded = false;
                m_subs.clear();
                m_mediaInfo = QString();
                g_playstate = Mpv::Stopped;
                g_core_signal->notifyPlayState();
                m_videoBitrate = 0;
                m_audioBitrate = 0;
                if(!m_isManualStop)
                {
                    if(m_lastTime != 0)
                        g_user_signal->playNext(false);
                }
                break;
            case MPV_EVENT_SHUTDOWN:
                KyInfo() << "[mpv event] MPV_EVENT_SHUTDOWN";
                QCoreApplication::quit();
                break;
            case MPV_EVENT_LOG_MESSAGE:
            {
//                mpv_event_log_message *message = static_cast<mpv_event_log_message*>(event->data);
                break;
            }
            case MPV_EVENT_TRACK_SWITCHED:
                KyInfo() << "[mpv event] MPV_EVENT_TRACK_SWITCHED";
                // 轨道id改变
                mpv_get_property(m_mpvHandle, "sid", MPV_FORMAT_INT64, &sid);
                if (sid > m_stracks)
                    sid = 0;

                g_core_signal->notifySubId(sid);
                if(sid != m_sid && sid >= 0)
                {
                    m_sid = sid;
                    if (sid == 0) {
                        ShowText(tr("subtitle : ").append("no"));
                    }
                    // 字幕改变
                    for(Mpv::Track tck : m_fileInfo.stracks)
                        if(tck.id == sid)
                            ShowText(tr("subtitle : ").append(tck.title));
                }
                mpv_get_property(m_mpvHandle, "aid", MPV_FORMAT_INT64, &aid);
                g_core_signal->notifyAudioId(aid);
                mpv_get_property(m_mpvHandle, "vid", MPV_FORMAT_INT64, &vid);
                g_core_signal->notifyVideoId(vid);
                m_vid = vid;
                m_aid = aid;
                break;
            default: // unhandled events
                break;
            }
        }
        return false;
    }
    return QObject::event(event);
}
