#ifndef MPVCORE_H
#define MPVCORE_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QSet>

#include <mutex>
#include <condition_variable>

#include "mpvtypes.h"
#include "mediahandle.h"

#include <mpv/client.h>
#include <mpv/opengl_cb.h>
#include <mpv/qthelper.hpp>

using namespace KMedia;

#define MPV_REPLY_COMMAND 1
#define MPV_REPLY_PROPERTY 2
#define OMX_DEC_MAX_WIDTH 4096
#define OMX_DEC_MAX_HEIGHT 4096

class PlayXWidget;
class PlayGLWidget;

class MpvCore : public QObject
{
    Q_OBJECT
public:
    explicit MpvCore(QWidget *pw, QObject *parent = nullptr);

signals:
    void sigShowText(QString);

public slots:
    void Open(QString file, int start = 0);
    void Play();
    void Pause();
    void Stop();
    void Seek(int pos, bool relative = false, bool osd = false);
    void Restart();
    void Mute(bool);
    void Volume(int);
    void Channel(Mpv::Channel c);
    void Speed(double);
    void SpeedUp();
    void SpeedDown();
    void BrightnessUp();
    void BrightnessDown();
    void SubId(int);
    void AudioId(int);
    void AudioNext();
    void NextFrame();
    void PrevFrame();
    void Aspect(Mpv::VideoAspect);
    void SetAspect(QString);
    void RestoreFrame();
    void ClockwiseRotate();
    void CounterClockwiseRotate();
    void FlipHorizontally();
    void FlipVertically();
    void AddSub(QString);
    void AddSubs(QStringList);
    void SubMoveUp();
    void SubMoveDown();
    void SubNext();
    void SubForward();
    void SubBackward();
    void SubFontSize(int);
    void SubFontFamily(QString);
    void AddBookMark(QString);
    void ScreenShot(bool);
    void ScreenshotFormat(QString);
    void ScreenshotDirectory(QString);
    void ShowInfo(bool);
    void ShowText(QString text, int duration = 3000);
    void VideoDecoder(QString);
    void VideoOutput(QString);
    void DecodeThreads(int);

    int getVid(){return m_vid;}
    int getRotate(){return m_rotate;}
    bool getRestartState() {return m_restartOkState;}
    QString getMediaInfo();

private:
    std::mutex              m_mtxFileInfo,
                            m_mtxTracks,
                            m_mtxStop;
    std::condition_variable m_condState;

    mpv::qt::Handle m_mpvHandle;
    Mpv::FileInfo   m_fileInfo;
    Mpv::PlayState  m_playState = Mpv::Idle;

    QWidget *m_playWidget;

    QTimer *m_showInfoTimer;
    QTimer *m_showBufferingTimer;

    QString m_playingFile;
    QString m_mediaInfo;
    QString m_screenshotDir;

    QVector<QString> m_subs;
    QSet<int> m_marks;

    double m_fps;
    double m_speed;
    double m_avsync;
    double m_subDelay;
    double m_lastTime; // 不同于 lastStopTime,此变量表示进度改变上一个时刻的时间
    double m_duration;

    int64_t m_vid;
    int64_t m_aid;
    int64_t m_sid;
    int64_t m_subPos;
    int m_lastStopTime;
    int m_rotate;
    int m_seekTime;
    int m_volume;
    int m_brightness;
    int m_videoBitrate;
    int m_audioBitrate;
    int m_vtracks;
    int m_atracks;
    int m_stracks;

    bool m_mute;
    bool m_isVideo;
    bool m_isLoaded;
    bool m_osdShow;
    bool m_isHFlip;
    bool m_isVFlip;
    bool m_needSeek;
    bool m_showProfile;
    bool m_isManualStop;
    bool m_isSinkMuteChange;
    bool m_isSinkVolumeChange;
    bool m_isPauseWhenNeedSeek;
    bool m_canSaveScreenShot;
    bool m_restartOkState;
    bool m_changeVolumeForce;

private slots:
    void initMpvHandle();
    void initGlobalSig();
    void initVolumeDBus();
    void LoadTracks();
    void LoadChapters();
    void LoadVideoParams();
    void LoadAudioParams();
    void LoadMetadata();
    void LoadOptions();
    void LoadFileInfo();
    void SetProperties();
    void SetOption(QString key, QString val);
    void slotVolumeChange(QString, int, bool);

    QVariant Command(const QVariant& params);
    int AsyncCommand(const char *args[]);

protected:
    bool event(QEvent *event);
};

#endif // MPVCORE_H
