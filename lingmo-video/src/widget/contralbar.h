#ifndef CONTRALBAR_H
#define CONTRALBAR_H

#include <QWidget>
#include <QMutex>
#include "themewidget.h"
#include "core/mpvtypes.h"

class TimeSlider;
class FilletWidget;
class VideoPreview;
class FilletWidget;

class QLabel;
class QFrame;
class QSlider;
class QListWidget;
class QPushButton;
class QPropertyAnimation;

namespace Ui {
class ContralBar;
}

class ContralBar : public ThemeWidget
{
    Q_OBJECT

public:
    explicit ContralBar(QWidget *parent = nullptr);
    ~ContralBar();

    void setDuration(int duration);
    void setCurrentTime(int currentTime);
    void setPreviewSize(QSize size);
    void setMiniMode(bool b){m_isMiniMode = b;}
    void setHide();
    void setShow();

    void setUIMode(bool isTabletMode);
    void setMainWidgetClickPos(QPoint pos);

    void setBlackTheme();
    void setDefaultTheme();

public slots:
    void clearMark();
    void updateMarks();
    void addMark(int mark_pos, QString desc);
    void insertMark(int mark_pos, QString desc);      // 正在播放时插入书签
    void deleteMark(int mark_pos);

private:
    Ui::ContralBar *ui;
    QString m_theme;
    QString m_playingFile;
    QString m_highlight_color_name;
    double m_duration,
           m_currentTime;
    int64_t m_lastPreviewShowTime;
    int64_t m_last_seek_time;
    int m_seekTime;
    int m_mouseTime;
    int m_volume;
    int m_videoId;
    double m_speed;
    bool m_isVideo,
         m_isMute,
         m_isChangeTime,
         m_isFullScreen,
         m_isMouseEnter,
         m_isMiniMode,
         m_isSeeking,
         m_canChangeDuration,
         m_mouseOnSlider;

    bool m_isTabletMode;

    QTimer *m_seekTimer;
    QTimer *m_timerVolumeWidgetHide;
    QTimer *m_timerSpeedWidgetHide;
    QTimer *m_timerToolWidgetHide;

    FilletWidget *m_widget;
    QPushButton *m_nextButton;
    QPushButton *m_playPauseButton;
    QPushButton *m_preButton;
    QFrame *m_frame;
    QPushButton *m_fullscreenButton;
    QPushButton *m_speedButton;
    QPushButton *m_toolButton;
    QPushButton *m_volumeButton;
    QLabel *m_posLabel;
    TimeSlider *m_timeSlider;

    QSlider *m_volumeSlider;
    FilletWidget *m_volumeWidget;

    QListWidget *m_speedList;
    FilletWidget *m_speedWidget;

    QListWidget *m_toolList;
    FilletWidget *m_toolWidget;

    VideoPreview *m_videoPreview;

    QPropertyAnimation *m_showAnm;
    QPropertyAnimation *m_hideAnm;

    QMap<int, QString> m_markMap;
    QMutex m_mutexMark;

    void initGlobalSig();
    void initLayout();
    void initConnect();
    void initAnimation();
    void initSpeedList();
    void initToolList();
    void updateIcon();
    void resetSize();

signals:
    void sigFullScreen(bool);
    void sigCanHide(bool);

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void moveEvent(QMoveEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void showEvent(QShowEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void updateHightLightColor();

private slots:
    void slotFullScreen();
    void slotMute();
    void slotShowPreview(qint64 time);
    void slotPlayStateChange();
};

#endif // CONTRALBAR_H
