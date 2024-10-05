#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QStackedWidget>

#include "core/mpvcore.h"

#define NormalModeSize   QSize(960, 540)
#define MiniModeSize    QSize(400, 225)

class HomePage;
class ContralBar;
class PlayXWidget;
class PlayGLWidget;
class TitleMenu;
class SystemTrayIcon;
class KRightClickMenu;
class TitleWidget;
class MusicWidget;
class PlayListWidget;
class MiniModeShade;
class MediaInfoDialog;
class QDBusInterface;
class QLabel;

class MainWindow : public QWidget
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.lingmo.lingmovideo.play")

public:
    explicit MainWindow(QStringList list, QWidget *parent = nullptr);
    ~MainWindow();

signals:

private:
    QStackedWidget  *m_stacked_widget;
    HomePage        *m_home_page;        // 主界面
    TitleMenu       *m_title_menu;       // 标题栏菜单
    KRightClickMenu *m_right_click_menu; // 右键菜单
    TitleWidget     *m_title_widget;     // 标题栏
    ContralBar      *m_contral_bar;      // 控制栏
    MusicWidget     *m_music_widget;     // 音乐播放界面
    PlayListWidget  *m_playlist_widget;  // 播放列表界面
    SystemTrayIcon  *m_tray_icon;

    MediaInfoDialog *m_dialog_media_info;
    QString m_current_file;

    MiniModeShade   *m_mini_mode_shade;   // mini 模式遮罩
    QWidget *m_play_widget;

    QRect m_normal_rect;

    Mpv::FileInfo       m_file_info;
    Mpv::VideoAspect    m_video_aspect;      // 视频比例
    MpvCore *m_core;

    bool m_is_top;
    bool m_is_active;
    bool m_is_mini_mode;
    bool m_is_maximized;
    bool m_is_fullscreen;
    bool m_is_tablet_mode;
    bool m_is_screen_locked = false;
    bool m_exit_flag;
    bool m_can_hide_all;
    bool m_need_play_when_min;
    bool m_need_play_when_s34;

    QDBusInterface *m_dbus_interface;
    quint32 m_inhibit_value;             // 阻止锁屏cookie
    quint64 m_window_id;

public slots:
    void lingmo_video_play_request(QStringList filelist);

private slots:
    // s3 s4 处理
    void onPrepareForSleep(bool isSleep);
#if 0
    void onPrepareForShutdown(bool Shutdown);
#endif
    void onScreenLock();
    void onScreenUnlock();
    void onWiredControl(QString data);
    void slotPrepareForSwitchuser();

    // 耳机插拔
    void inputDeviceGet(QString device);
    void uiModeChange(bool isTabletMode);

    void slotQuit();
    void slotClose();
    void slotShowMiniMode();
    void slotShowNormalMode();
    void slotShowFullScreen(bool);
    void slotShowTitleMenu();
    void slotChangeMaxState();
    void slotChangeMiniState();
    void slotOpenHelpDoc();

    void slotPlayStateChange();

    void slotThemeChange(int theme);
    void slotShowDefault();
    void slotChangePause();
    void slotOpenFile();
    void slotOpenDir();
    void slotOpenUrl();
    void slotPlayFile(QString file, int pos);
    void slotPlayWidgetClicked();
    void slotToTop(bool is_top = true);

private:
    void initGlobalSig();
    void initLayout();
    void initDBus();
    void initMenu();
    void initCore();

    void initTrayIcon();
    void initTitleWidget();
    void initPlayListWidget();
    void initMiniModeShade();
    void initContralBar();
    void initMediaInfo();

    void updateAspect();
    void resetLayout();
    void hideAll(bool);
    void appQuit();

    void Single(QStringList filelist);
    void windowStateChange();

    bool event(QEvent *event) override;
    void enterEvent(QEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    // 以下实现文件拖入播放
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // MAINWINDOW_H
