#include "mainwindow.h"

#include <QEvent>
#include <QTimer>
#include <QWindow>
#include <QThread>
#include <QProcess>
#include <QMimeData>
#include <QDBusReply>
#include <QApplication>
#include <QDesktopWidget>
#include <QDBusInterface>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QDBusConnection>
#include <QFileSystemWatcher>
#include <QWindowStateChangeEvent>

#include <KF5/KWindowSystem/kwindowsystem.h>
#include <KWindowEffects>
#include <X11/Xatom.h>

#include "core/dbusadapter.h"

#include "widget/kmenu.h"
#include "widget/homepage.h"
#include "widget/contralbar.h"
#include "widget/musicwidget.h"
#include "widget/titlewidget.h"
#include "widget/playxwidget.h"
#include "widget/playglwidget.h"
#include "widget/urleditwidget.h"
#include "widget/minimodeshade.h"
#include "widget/systemtrayicon.h"
#include "widget/playlistwidget.h"
#include "widget/mediainfodialog.h"
#include "widget/setup/setupdialog.h"

#include "global/global.h"
#include "global/functions.h"
#include "global/translator.h"
#include "global/extensions.h"

#include <kaboutdialog.h>
#include <kinputdialog.h>
#include <lingmo-log4qt.h>
#include <fcntl.h>

#include <X11/Xlib.h>
#include <QX11Info>

using namespace kdk;
using namespace Global;

MainWindow::MainWindow(QStringList list, QWidget *parent) :
    QWidget(parent),
    m_exit_flag(false),
    m_is_mini_mode(false),
    m_is_maximized(false),
    m_is_fullscreen(false),
    m_stacked_widget(nullptr),
    m_home_page(nullptr),
    m_title_menu(nullptr),
    m_title_widget(nullptr),
    m_music_widget(nullptr),
    m_playlist_widget(nullptr),
    m_mini_mode_shade(nullptr),
    m_can_hide_all(true),
    m_need_play_when_s34(false),
    m_window_id(0)
{
    setAcceptDrops(true);

    g_shortcut->setParent(this);
    g_shortcut->initData();

    connect(kdk::WindowManager::self(), &kdk::WindowManager::windowAdded, this, [=](const kdk::WindowId& window_id){
        if (getpid() == kdk::WindowManager::getPid(window_id) && m_window_id == 0)
        {
            m_window_id = window_id.toULongLong();
        }
    });

    Single(list);
    initDBus();

    initLayout();

    initGlobalSig();
    initCore();
    initMenu();
    initTrayIcon();

    // 需要show出来之后才能播放，不然播放会失败，原因是 QOpenGLWidget 没有 show 的时候渲染会报错。
    QTimer::singleShot(1000, [list, this](){
        if (list.size() > 0)
        {
            if (list.at(0).startsWith("http"))
                slotPlayFile(list.at(0), 0);
            else
                g_user_signal->addFiles(list);
        }
        else {
            // 播放宣传片
            QString pf = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation).append("/银河灵墨操作系统V10-sp1宣传视频.mp4");
            QFileInfo fi(pf);
            if (fi.exists())
                g_user_signal->addFiles(QStringList() << pf);
        }
    });
}

MainWindow::~MainWindow()
{

}

void MainWindow::lingmo_video_play_request(QStringList file_list)
{
    // 此处相当于双击打开，需要active一下
    if (isHidden()) {
        // wayland 每次 show 的时候都要先去除边框
        if (isWayland)
            kdk::LingmoStyleHelper::self()->removeHeader(this);
        show();
        slotToTop(m_is_top);
    }
    else if (!isActiveWindow()) {
        kdk::WindowManager::activateWindow(m_window_id);
    }
    if (file_list.size() > 0)
    {
        // 播放
        if (file_list.at(0).startsWith("http"))
            slotPlayFile(file_list.at(0), 0);
        else
            g_user_signal->addFiles(file_list);
    }
}

void MainWindow::onPrepareForSleep(bool isSleep)
{
    if (isSleep) {
        if (g_playstate > 0) {
            if(g_config->keepStateWhenWakeup.first && g_playstate == Mpv::Playing)
                m_need_play_when_s34 = true;
            m_core->Pause();
        }
    }
    else {
        // 唤醒之后要重新，不然显示会有问题，共性问题，此方法规避（引入问题有点多，只对有问题显卡进行处理，但是现在没有问题的列表，遇到之后再做处理）
        if (true) {

        }
        else {
            m_core->Restart();
        }
        if (m_need_play_when_s34) {
            // 判断是否锁屏了，如果锁屏等解锁后再播放
            if(m_is_screen_locked)
                return;
            if (true) {
                m_core->Play();
            }
            else {
                // 跳转之后才能继续播放,不然跳转之后就又暂停了
                QThread::create([this]{
                    int try_times = 9;
                    while (!m_core->getRestartState() || try_times--) {
                        QThread::msleep(200);
                    }
                    m_core->Play();
                })->start();
            }
            m_need_play_when_s34 = false;
        }
    }
}

void MainWindow::onScreenLock()
{
    m_is_screen_locked = true;
    // 只有 wayland 才需要如此
    g_shortcut->makeAllInvalid();
    if(g_playstate == Mpv::Playing && m_need_play_when_s34 == false)
    {
        m_core->Pause();
        if(g_config->keepStateWhenWakeup.first)
            m_need_play_when_s34 = true;
    }
}

void MainWindow::onScreenUnlock()
{
    m_is_screen_locked = false;
    g_shortcut->makeAllValid();
    if(m_need_play_when_s34)
    {
        m_core->Play();
        if(g_config->keepStateWhenWakeup.first)
            m_need_play_when_s34 = false;
    }
}

void MainWindow::onWiredControl(QString data)
{
    int key = data.split(':').first().toInt();
    int num = data.split(':').last().toInt();
    if(num == 1)
        g_user_signal->play_pause();
    else if(num == 2 || key == 163)
        g_user_signal->playNext(true);
    else if(num == 3 || key == 165)
        g_user_signal->playPrev(true);
}

void MainWindow::slotPrepareForSwitchuser()
{
    if (m_core)
        m_core->Pause();
}

void MainWindow::inputDeviceGet(QString device)
{
    // 接收到耳机插拔信号就暂停
    KyInfo() << "playState : " << g_playstate << ", out device change " << device;
    if(g_playstate == Mpv::Playing) {
        m_core->Pause();
    }
}

void MainWindow::uiModeChange(bool isTabletMode)
{
    // ui 模式改变（电脑模式/平板模式）
    Global::isTablet = isTabletMode;
    m_contral_bar->setUIMode(isTabletMode);
    m_title_widget->setUIMode(isTabletMode);
}

void MainWindow::initLayout()
{
    setMinimumSize(NormalModeSize);
    resize(NormalModeSize);
    kdk::LingmoStyleHelper::self()->removeHeader(this);

    KWindowEffects::enableBlurBehind(winId(), true);
    setWindowTitle(tr("Video Player"));
    setMouseTracking(true);

    m_stacked_widget = new QStackedWidget(this);
    m_home_page = new HomePage;

    if (g_config->videoOutputType() == GlobalConfig::VO_WID) {
        m_play_widget = new PlayXWidget;
        QPalette pal(m_play_widget->palette());
        pal.setColor(QPalette::Background, Qt::black);
        m_play_widget->setAutoFillBackground(true);
        m_play_widget->setPalette(pal);
        connect(dynamic_cast<PlayXWidget*>(m_play_widget), &PlayXWidget::mousePressed, this, &MainWindow::slotPlayWidgetClicked);
    }
    else {
        m_play_widget = new PlayGLWidget;
        connect(dynamic_cast<PlayGLWidget*>(m_play_widget), &PlayGLWidget::mousePressed, this, &MainWindow::slotPlayWidgetClicked);
    }

    m_stacked_widget->addWidget(m_home_page);
    m_stacked_widget->addWidget(m_play_widget);
    m_stacked_widget->setCurrentIndex(0);

    m_music_widget = new MusicWidget(m_play_widget);
    m_music_widget->installEventFilter(m_play_widget);
    m_music_widget->hide();

    initTitleWidget();
    initContralBar();
    initMiniModeShade();
    initPlayListWidget();
    initMediaInfo();
}

void MainWindow::initTrayIcon()
{
    m_tray_icon = new SystemTrayIcon(this);
    connect(m_tray_icon, &SystemTrayIcon::sigQuit, this, &MainWindow::appQuit);
    connect(m_tray_icon, &SystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason){
        switch (reason) {
        case QSystemTrayIcon::Trigger:
            if (isHidden() || !isActiveWindow()) {
                kdk::LingmoStyleHelper::self()->removeHeader(this);
                setWindowState(windowState() & ~Qt::WindowMinimized);
                show();
                kdk::WindowManager::activateWindow(m_window_id);
                slotToTop(m_is_top);
            }
            break;
        default:
            break;
        }
    });
    m_tray_icon->show();
}

void MainWindow::initDBus()
{
    m_dbus_interface = new QDBusInterface("org.gnome.SessionManager",
                                         "/org/gnome/SessionManager",
                                         "org.gnome.SessionManager",
                                         QDBusConnection::sessionBus());

    QDBusConnection sessionBus = QDBusConnection::sessionBus();
    if(sessionBus.registerService("org.lingmo.lingmovideo"))
    {
        sessionBus.registerObject("/org/lingmo/lingmovideo",this,
                                  QDBusConnection::ExportAllContents);
        KyInfo() << "初始化DBUS成功";
    }
    else
    {
        KyInfo("init dbus error");
    }

    //S3 S4策略
    QDBusConnection::systemBus().connect(QString("org.freedesktop.login1"),
                                         QString("/org/freedesktop/login1"),
                                         QString("org.freedesktop.login1.Manager"),
                                         QString("PrepareForShutdown"), this,
                                         SLOT(onPrepareForSleep(bool)));
    QDBusConnection::systemBus().connect(QString("org.freedesktop.login1"),
                                         QString("/org/freedesktop/login1"),
                                         QString("org.freedesktop.login1.Manager"),
                                         QString("PrepareForSleep"), this,
                                         SLOT(onPrepareForSleep(bool)));

    // 锁屏
    QDBusConnection::sessionBus().connect(QString("org.lingmo.ScreenSaver"),
                                         QString("/"),
                                         QString("org.lingmo.ScreenSaver"),
                                         QString("lock"), this,
                                         SLOT(onScreenLock()));
    QDBusConnection::sessionBus().connect(QString("org.lingmo.ScreenSaver"),
                                         QString("/"),
                                         QString("org.lingmo.ScreenSaver"),
                                         QString("unlock"), this,
                                         SLOT(onScreenUnlock()));

    // 耳机插拔
    QDBusConnection::sessionBus().connect(QString(""),
                                         QString("/"),
                                         QString("org.lingmo.media"),
                                         QString("sinkPortChanged"), this,
                                         SLOT(inputDeviceGet(QString)));

    // 线控
    QDBusConnection::systemBus().connect(QString(),
                                         QString("/"),
                                         QString("com.monitorkey.interface"),
                                         QString("monitorkey"), this,
                                         SLOT(onWiredControl(QString)));

    //切换用户
    QDBusConnection::sessionBus().connect(QString("org.gnome.SessionManager"),
                                          QString("/org/gnome/SessionManager"),
                                          QString("org.gnome.SessionManager"),
                                          QString("PrepareForSwitchuser"), this,
                                          SLOT(slotPrepareForSwitchuser()));

    //切换模式
    QDBusConnection::sessionBus().connect(QString("com.lingmo.statusmanager.interface"),
                                          QString("/"),
                                          QString("com.lingmo.statusmanager.interface"),
                                          QString("mode_change_signal"), this,
                                          SLOT(uiModeChange(bool)));

    // 因为重写了 closeEvent，影响注销
    QGuiApplication::setFallbackSessionManagementEnabled(false);

    DbusAdapter *dbs_adapter = new DbusAdapter;

}

void MainWindow::initMenu()
{
    m_right_click_menu = new KRightClickMenu;

    connect(m_right_click_menu, SIGNAL(sigOpenFile()), this, SLOT(slotOpenFile()));
    connect(m_right_click_menu, SIGNAL(sigOpenDir()), this, SLOT(slotOpenDir()));
    connect(m_right_click_menu, SIGNAL(sigOpenUrl()), this, SLOT(slotOpenUrl()));
    connect(m_right_click_menu, SIGNAL(sigToTop(bool)), this, SLOT(slotToTop(bool)));
    connect(m_right_click_menu, SIGNAL(sigPlayOrder(int)),this, SLOT(slotPlayOrder(int)));
    connect(m_right_click_menu, &KRightClickMenu::sigDefaultFrame, [this](){
        m_video_aspect = Mpv::AUTO;
    });
    connect(m_right_click_menu, &KRightClickMenu::sig4Div3Frame, [this](){
        m_video_aspect = Mpv::DIV_4_3;
    });
    connect(m_right_click_menu, &KRightClickMenu::sig16Div9Frame, [this](){
        m_video_aspect = Mpv::DIV_16_9;
    });
    connect(m_right_click_menu, &KRightClickMenu::sigFullFrame, [this](){
        m_video_aspect = Mpv::FULL;
//        updateAspect();
    });

    connect(m_right_click_menu, &KRightClickMenu::sigRestoreFrame, [this](){
        m_video_aspect = Mpv::AUTO;
    });

    connect(m_right_click_menu, &KRightClickMenu::sigAlongRotate, [this](){
        // 旋转后需要判断是否时满屏画面，如果是的话需要改变比例
//        if(m_video_aspect == Mpv::FULL)
//            updateAspect();
    });
    connect(m_right_click_menu, &KRightClickMenu::sigInverseRotate, [this](){
//        if(m_video_aspect == Mpv::FULL)
//            updateAspect();
    });
    connect(m_right_click_menu, &KRightClickMenu::sigMediaInfo, [this](){
        if (g_playstate < 0)
            return;
        m_dialog_media_info->setTitle(m_file_info.media_title);
        m_dialog_media_info->setType(m_file_info.file_type);
        m_dialog_media_info->setSize(m_file_info.file_size);
        m_dialog_media_info->setDuration(Functions::timeToStr(m_file_info.length, false));
        m_dialog_media_info->setPath(m_file_info.file_path);
        m_dialog_media_info->updateDate();
        connect(g_core_signal, &GlobalCoreSignal::sigFileInfoChange, [this](Mpv::FileInfo file_info){
            m_file_info = file_info;
            m_dialog_media_info->setData(m_core->getMediaInfo());
            m_dialog_media_info->setTitle(m_file_info.media_title);
            m_dialog_media_info->setType(m_file_info.file_type);
            m_dialog_media_info->setSize(m_file_info.file_size);
            m_dialog_media_info->setDuration(Functions::timeToStr(m_file_info.length, false));
            m_dialog_media_info->setPath(m_file_info.file_path);
            m_dialog_media_info->updateDate();
        });
        kdk::LingmoStyleHelper::self()->removeHeader(m_dialog_media_info);
        m_dialog_media_info->show();
        KyInfo() << m_dialog_media_info->size() << m_dialog_media_info->geometry().center()  ;
        m_dialog_media_info->move(geometry().center() - QPoint(m_dialog_media_info->width()/2, m_dialog_media_info->height()/2));
    });
}

void MainWindow::initCore()
{
    m_core = new MpvCore(m_play_widget);
    connect(m_core, &MpvCore::sigShowText, m_music_widget, &MusicWidget::showOsdText);
}

void MainWindow::initTitleWidget()
{
    m_title_menu = new TitleMenu;
    connect(m_title_menu, &TitleMenu::sigQuit, this, &MainWindow::slotQuit);

    m_title_widget = new TitleWidget(this);
    m_title_widget->setUIMode(isTablet);
    m_title_widget->raise();
    m_title_widget->move(0, 0);
    m_title_widget->setTitle(tr("Video Player"));

    connect(m_title_widget, &TitleWidget::sigMaxSize,    this, &MainWindow::slotChangeMaxState);
    connect(m_title_widget, &TitleWidget::sigMiniSize,   this, &MainWindow::slotChangeMiniState);
    connect(m_title_widget, &TitleWidget::sigClose,      this, &MainWindow::slotClose);
    connect(m_title_widget, &TitleWidget::sigShowMenu,   this, &MainWindow::slotShowTitleMenu);
    connect(m_title_widget, &TitleWidget::sigMiniMode,   this, &MainWindow::slotShowMiniMode);
    connect(m_title_widget, &TitleWidget::sigCanHide, [this](bool canHide){m_can_hide_all = canHide;});
}

void MainWindow::initPlayListWidget()
{
    m_playlist_widget = new PlayListWidget(this);
    m_playlist_widget->raise();
    m_playlist_widget->move(width()-16, 0);

    // 标题栏跟随列表展开改变大小
    connect(m_playlist_widget, &PlayListWidget::sigMove, this, [this](int distance){
        m_title_widget->setGeometry(0, 0, width() - distance, m_title_widget->height());
    });

    // 列表显示的时候隐藏标题栏和控制栏
    connect(g_user_signal, &GlobalUserSignal::sigShowPlayList, [&](){
        if(m_stacked_widget->currentIndex() == 1) {
            // 防止列表弹出的时候鼠标在播放界面上移动导致标题栏和控制栏弹出
            m_play_widget->setMouseTracking(false);
            QTimer::singleShot(400, [this](){m_play_widget->setMouseTracking(true);});

            m_can_hide_all = true;
            hideAll(true);
        }
    });

    // 正在播放的文件改变，书签需要刷新
    connect(m_playlist_widget->getPlayList(), &PlayList::sigPlayingFileMarkUpdate, [this](QVector<MarkItem> marks){
        m_contral_bar->clearMark();
        foreach (MarkItem item, marks) {
            m_contral_bar->addMark(item.m_markPos, item.m_describe);
        }
        m_contral_bar->updateMarks();
    });

    connect(m_playlist_widget->getPlayList(), &PlayList::sigDeleteMark, [this](int mark_pos){
        m_contral_bar->deleteMark(mark_pos);
    });

    // 正在播放时插入书签，需要刷新进度条书签
    connect(m_playlist_widget->getPlayList(), &PlayList::sigInsertMark, [this](MarkItem mark){
        m_contral_bar->insertMark(mark.m_markPos, mark.m_describe);
    });
}

void MainWindow::initMiniModeShade()
{
    m_mini_mode_shade = new MiniModeShade(this);
    m_mini_mode_shade->hide();

    connect(m_mini_mode_shade, &MiniModeShade::sigClose,      this, &MainWindow::close);
    connect(m_mini_mode_shade, &MiniModeShade::sigShowNormal, this, &MainWindow::slotShowNormalMode);
    connect(m_mini_mode_shade, &MiniModeShade::sigPlayPause, [&](){
        g_user_signal->sigPlayPause();
    });
}

void MainWindow::initContralBar()
{
    m_contral_bar = new ContralBar(this);
    m_contral_bar->setUIMode(isTablet);
    m_contral_bar->hide();
    m_contral_bar->raise();

    connect(m_contral_bar, &ContralBar::sigFullScreen, this, &MainWindow::slotShowFullScreen);
    connect(m_contral_bar, &ContralBar::sigCanHide, [this](bool canHide){m_can_hide_all = canHide;});
}

void MainWindow::initMediaInfo()
{
    QWidget* show_parent = nullptr;
    if (isWayland)
        show_parent = this;
    // 媒体信息弹窗
    m_dialog_media_info = new MediaInfoDialog(show_parent);
    m_dialog_media_info->setModal(true);
}

void MainWindow::updateAspect()
{
    double s;
    if(m_core->getRotate() / 90 % 2 == 0)
        s = (double)width() / (double)height();
    else
        s = (double)height() / (double)width();
    m_core->SetAspect(QString::number(s));
}

void MainWindow::resetLayout()
{
    if (!m_is_mini_mode && !m_is_maximized && !m_is_fullscreen)
        m_normal_rect = geometry();

    m_title_widget->setGeometry(0, 0, width(), m_title_widget->height());
    if (Global::isTablet)
        m_contral_bar->setGeometry(120, height()-72, width()-240, 60);
    else
        m_contral_bar->setGeometry(120, height()-72, width()-240, 48);
    m_contral_bar->setPreviewSize(size());

    m_music_widget->setGeometry(0, 0, width(), height());
    m_play_widget->setGeometry(0, 0, width(), height());
    m_stacked_widget->setGeometry(0, 0, width(), height());

    m_playlist_widget->updateHideIcon();
    m_playlist_widget->move(width() - 16, 0);
    m_playlist_widget->resize(m_playlist_widget->width(), height());

    m_title_widget->raise();

    if (m_stacked_widget->currentIndex() == 1)
        hideAll(true);

    if (m_mini_mode_shade)
        m_mini_mode_shade->resize(size());
}

void MainWindow::hideAll(bool hide)
{
    if (hide && !m_music_widget->isHidden())
        return;
    // NEEDFIX:如果是默认音乐图标也不要隐藏
    if (m_stacked_widget->currentIndex() == 0)
        return;
    if (hide)
    {
        if(m_can_hide_all)
        {
            m_contral_bar->setHide();
            m_title_widget->setHide();
            if (!m_playlist_widget->isShow())
                m_playlist_widget->setShowButton(false);
            // 播放列表是否需要隐藏 ?
        }
        else
        {
            // 鼠标要显示出来
            setCursor(Qt::ArrowCursor);
            m_play_widget->setCursor(Qt::ArrowCursor);
            m_title_widget->setCursor(Qt::ArrowCursor);
        }
    }
    else
    {
        if (!m_playlist_widget->isShow()) {
            if (m_stacked_widget->currentIndex() == 1)
                m_contral_bar->setShow();
            if (!m_is_mini_mode)
                m_playlist_widget->setShowButton(true);
            m_title_widget->setShow();
            m_title_widget->raise();
        }
    }
}

void MainWindow::appQuit()
{
    // 判断退出时是否需要清空播放列表，无痕播放关闭的时候也清空列表
    if (g_config->clearListWhenExit.first || g_config->seamlessBrowsing.first)
        g_user_signal->clearPlayList();
    m_exit_flag = true;
    if (g_playstate < 0) {
        exit(0);
    }
    m_core->Stop();
}

void MainWindow::Single(QStringList filelist)
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString lockPath = homePath.at(0) + "/.config/lingmo-video-lock";
    int fd = open(lockPath.toUtf8().data(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    if (fd < 0) { exit(1); }
    if (lockf(fd, F_TLOCK, 0))
    {
        QDBusInterface interface( "org.lingmo.lingmovideo",
                                  "/org/lingmo/lingmovideo",
                                  "org.lingmo.lingmovideo.play",
                                  QDBusConnection::sessionBus());
        QDBusReply<int> reply = interface.call( "lingmo_video_play_request", filelist);
        qDebug() << "file path is " << filelist;
        if ( reply.isValid() && reply.value() == 0)
        {
            KyInfo() << "call lingmo_video_play_request ok";
        }
        else
        {
            KyInfo() << "fail";
        }
        KyInfo("The app is already running");
        exit(0);
    }
}

void MainWindow::windowStateChange()
{
#if 0
    long compositor = (windowState() & Qt::WindowMaximized || windowState() & Qt::WindowFullScreen) ? 1 : 2;
    QTimer::singleShot(500, this, [&, compositor](){
        if (isWayland)
            return;
        Atom _NET_WM_BYPASS_COMPOSITOR = XInternAtom(QX11Info::display(), "_NET_WM_BYPASS_COMPOSITOR", false);
        XChangeProperty(QX11Info::display(), this->winId(), _NET_WM_BYPASS_COMPOSITOR, XA_CARDINAL,
                        32, XCB_PROP_MODE_REPLACE, (const unsigned char*)&compositor, 1);
    });
#endif

    if (windowState() & Qt::WindowMaximized)
        m_title_widget->updateMaxButtonStatus(true);
    else
        m_title_widget->updateMaxButtonStatus(false);
    m_is_active = windowState() & Qt::WindowActive;
}

void MainWindow::slotQuit()
{
    if(g_config->clearListWhenExit.first || g_config->seamlessBrowsing.first)
        g_user_signal->clearPlayList();
    m_exit_flag = true;
    if (g_playstate < 0) {
        exit(0);
    }
    m_core->Stop();
}

void MainWindow::slotClose()
{
    close();
}

void MainWindow::slotShowMiniMode()
{
    if (g_config->videoOutputType() == GlobalConfig::VO_WID)
        return;
    if (m_is_mini_mode || isFullScreen())
        return;

    if (m_is_maximized) {
        showNormal();
        m_is_maximized = false;
        m_title_widget->updateMaxButtonStatus(false);
    }

    m_is_mini_mode = true;
    m_contral_bar->setMiniMode(true);
    m_title_widget->setMiniMode(true);
    m_title_widget->hide();
    m_contral_bar->hide();
    m_playlist_widget->hide();

    QTimer::singleShot(20, [this](){
        setMinimumSize(MiniModeSize);
        resize(MiniModeSize);
        dynamic_cast<PlayGLWidget*>(m_play_widget)->setMouseUsed(false);
        if (m_mini_mode_shade) {
            m_mini_mode_shade->show();
            m_mini_mode_shade->raise();
        }
    });
}

void MainWindow::slotShowNormalMode()
{
    if (g_config->videoOutputType() == GlobalConfig::VO_WID)
        return;
    if (!m_is_mini_mode)
        return;
    setMinimumSize(NormalModeSize);
    setMaximumSize(99999, 99999);
    setGeometry(m_normal_rect);

    m_contral_bar->setMiniMode(false);
    m_title_widget->setMiniMode(false);
    m_playlist_widget->show();
    if (!m_music_widget->isHidden()) {
        m_title_widget->show();
        m_contral_bar->show();
    }

    dynamic_cast<PlayGLWidget*>(m_play_widget)->setMouseUsed(true);

    if (m_mini_mode_shade)
        m_mini_mode_shade->hide();

    m_is_mini_mode = false;
}

void MainWindow::slotShowFullScreen(bool full)
{
    if (m_is_fullscreen == full)
        return;
    if (m_is_mini_mode)
        return;
    if (full)
    {
        if (isWayland) {
            setWindowState(windowState() | Qt::WindowFullScreen);
        }
        else {
            KWindowSystem::setState(winId(), NET::FullScreen);
        }
//        setBypassCompositor(m_parentWidget->winId(), 1);
        m_title_widget->setButtonState(false);
        m_is_fullscreen = true;
    }
    else
    {
        if (isWayland) {
            setWindowState(windowState() & ~Qt::WindowFullScreen);
        }
        else {
            KWindowSystem::clearState(winId(), NET::FullScreen);
        }
//        setBypassCompositor(m_parentWidget->winId(), 2);
        m_title_widget->setButtonState(true);
        m_is_fullscreen = false;
    }
    m_can_hide_all = true;
    hideAll(true);
}

void MainWindow::slotShowTitleMenu()
{
    m_title_menu->exec(QPoint(m_title_widget->getMenuBtnX()+mapToGlobal(QPoint(0,0)).x(),
                             mapToGlobal(QPoint(0,0)).y()+m_title_widget->height()-5));
}

void MainWindow::slotChangeMaxState()
{
    // 如果是全屏的话退出全屏
    if (m_is_fullscreen) {
        slotShowFullScreen(false);
        return;
    }

    if (windowState() & Qt::WindowMaximized) {
        showNormal();
        m_is_maximized = false;
    }
    else {
        kdk::WindowManager::maximizeWindow(m_window_id);
        m_is_maximized = true;
    }
}

void MainWindow::slotChangeMiniState()
{
    kdk::WindowManager::minimizeWindow(m_window_id);
}

void MainWindow::slotOpenHelpDoc()
{
    QString serviceName = LINGMO_USER_GUIDE_SERVICE + QString("%1%2").arg("_").arg(QString::number(getuid()));
    QDBusInterface *iface = new QDBusInterface(serviceName,
                                                     LINGMO_USER_GUIDE_PATH,
                                                     LINGMO_USER_GUIDE_INTERFACE,
                                                     QDBusConnection::sessionBus(),
                                                     this);
    iface->call(QString("showGuide"), "lingmo-video");
    delete iface;
}

void MainWindow::slotPlayStateChange()
{
    static int last_state = Mpv::Idle;
    // 如果是 Paused -> Playing 不要进行切换
    if(g_playstate == Mpv::Playing && last_state != Mpv::Paused) {
        m_stacked_widget->setCurrentIndex(1);
        m_title_widget->setHomePage(false);
    }
    last_state = g_playstate;

    // 停止之后画面比例还原（如果需要保持比例需要重新做功能）
    if (g_playstate < 0) {
        m_video_aspect = Mpv::AUTO;
    }

    if (g_playstate == Mpv::Playing)
    {
        if(m_core->getVid() >= 0) {
            m_music_widget->hide();
        }
        else {
            m_music_widget->show();
            m_music_widget->raise();
        }
    }

    // 如果正在播放，阻止锁屏
    if (m_dbus_interface->isValid()) {
        if (g_playstate == Mpv::Playing)
        {
            QDBusMessage reply = m_dbus_interface->call(QDBus::Block, "Inhibit", "lingmo-video", (quint32)0, "video is playing", (quint32)8);
            m_inhibit_value = reply.arguments().takeFirst().toUInt();
        }
        else
        {
            m_dbus_interface->call("Uninhibit", m_inhibit_value);
        }
    }
    else {
        KyInfo("org.gnome.SessionManager is invalid!");
    }

    // 停止后是否需要退出
    if (g_playstate == Mpv::Idle && m_exit_flag) {
        exit(0);
    }
}

void MainWindow::slotThemeChange(int theme)
{
    switch (theme) {
    case 0:
        g_settings->setValue("General/follow_system_theme", true);
        break;
    case 1:
        g_settings->setValue("General/follow_system_theme", false);
        g_settings->setValue("General/theme", 0);
        break;
    case 2:
        g_settings->setValue("General/follow_system_theme", false);
        g_settings->setValue("General/theme", 1);
        break;
    default:
        break;
    }
}

void MainWindow::slotShowDefault()
{
    if (m_is_mini_mode) {
        slotShowNormalMode();
    }
    // 当前默认为拼音拼播放界面
    m_music_widget->hide();
    m_title_widget->setTitle(tr("Video Player"));
    m_title_widget->setShow();
    hideAll(false);
    m_contral_bar->hide();
    m_stacked_widget->setCurrentIndex(0);
    m_title_widget->setHomePage(true);
}

void MainWindow::slotChangePause()
{
    if(m_playlist_widget->isShow())
        m_playlist_widget->slotHide();

    if(g_playstate == Mpv::Playing)
        m_core->Pause();
    else if(g_playstate == Mpv::Paused)
        m_core->Play();
}

/**
 * @brief       : 打开文件
 * @param [in]  :
 * @param [out] :
 * @return      :
 */
void MainWindow::slotOpenFile()
{
    Extensions e;
    QString last_path = g_settings->value("History/last_path").toString();
    if(last_path == "")
        last_path = QDir::homePath();
    QStringList files;
    {
        QWidget *parent_widget = nullptr;
        if (isWayland)
            parent_widget = this;
        QFileDialog fd(parent_widget);
        fd.setModal(true);
        QList<QUrl> list = fd.sidebarUrls();
        int sidebarNum = 8;
        QString home = QDir::homePath().section("/", -1, -1);
        QString mnt = "/media/" + home + "/";
        QDir mntDir(mnt);
        mntDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        QFileInfoList filist = mntDir.entryInfoList();
        QList<QUrl> mntUrlList;
        for(int i=0; i < sidebarNum && i < filist.size(); i++) {
            QFileInfo fi = filist.at(i);
            //华为990、9a0需要屏蔽最小系统挂载的目录
            if (fi.fileName() == "2691-6AB8")
                 continue;
            mntUrlList << QUrl("file://" + fi.filePath());
        }
        QFileSystemWatcher fsw(&fd);
        fsw.addPath("/media/" + home + "/");
        connect(&fsw, &QFileSystemWatcher::directoryChanged, &fd, [=, &sidebarNum, &mntUrlList, &list, &fd](const QString path){
            QDir wmn_dir(path);
            wmn_dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            QFileInfoList wfilist = wmn_dir.entryInfoList();
            mntUrlList.clear();
            for(int i=0; i < sidebarNum && i < wfilist.size(); ++i) {
                QFileInfo fi = wfilist.at(i);
                //华为990、9a0需要屏蔽最小系统挂载的目录
                if (fi.fileName() == "2691-6AB8")
                     continue;
                mntUrlList << QUrl("file://" + fi.filePath());
            }
            fd.setSidebarUrls(list + mntUrlList);
            fd.update();
        });
        connect(&fd, &QFileDialog::finished, &fd, [=, &list, &fd](){
            fd.setSidebarUrls(list);
        });
        fd.setSidebarUrls(list + mntUrlList);
        fd.setDirectory(QDir(last_path));
        fd.setWindowTitle(tr("Video Player Choose a file"));

        QString setFilterM = tr("Multimedia") + (" (%1) ");
        setFilterM = setFilterM.arg(e.allPlayable().forFilter());

        QString setFilterV = tr("Video") + (" (%1) ");
        setFilterV = setFilterV.arg(e.video().forFilter());

        QString setFilterA = tr("Audio") + (" (%1) ");
        setFilterA = setFilterA.arg(e.audio().forFilter());

        fd.setNameFilters(QStringList()
                          << setFilterM
                          << setFilterV
                          << setFilterA);

        fd.setFileMode(QFileDialog::ExistingFiles);
        fd.setViewMode(QFileDialog::Detail);
        fd.setOption(QFileDialog::HideNameFilterDetails);

        g_shortcut->makeAllInvalid();
        kdk::LingmoStyleHelper::self()->removeHeader(&fd);
        if(fd.exec() == QFileDialog::Accepted)
        {
            files = fd.selectedFiles();
            qDebug() << files;
        }
        g_shortcut->makeAllValid();
    }

    if(files.size() <= 0)
        return;
    g_settings->setValue("History/last_path", files.first().left(files.first().lastIndexOf('/') + 1));

    g_user_signal->addFiles(files);
}

void MainWindow::slotOpenDir()
{
    QString last_path = g_settings->value("History/last_path").toString();
    // 打开文件夹
    QString url;
    {
        QWidget *parent_widget = nullptr;
        if (isWayland)
            parent_widget = this;
        QFileDialog fd(parent_widget);
        fd.setModal(true);
        int sidebarNum = 8;
        QList<QUrl> list = fd.sidebarUrls();
        QString home = QDir::homePath().section("/", -1, -1);
        QString mnt = "/media/" + home + "/";
        QDir mntDir(mnt);
        mntDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        QFileInfoList filist = mntDir.entryInfoList();
        QList<QUrl> mntUrlList;
        for(int i=0; i < sidebarNum && i < filist.size(); i++) {
            QFileInfo fi = filist.at(i);
            //华为990、9a0需要屏蔽最小系统挂载的目录
            if (fi.fileName() == "2691-6AB8")
                 continue;
            mntUrlList << QUrl("file://" + fi.filePath());
        }
        QFileSystemWatcher fsw(&fd);
        fsw.addPath("/media/" + home + "/");
        connect(&fsw, &QFileSystemWatcher::directoryChanged, &fd, [=, &sidebarNum, &mntUrlList, &list, &fd](const QString path){
            QDir wmnDir(path);
            wmnDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            QFileInfoList wfilist = wmnDir.entryInfoList();
            mntUrlList.clear();
            for(int i=0; i < sidebarNum && i < wfilist.size(); ++i) {
                QFileInfo fi = wfilist.at(i);
                //华为990、9a0需要屏蔽最小系统挂载的目录
                if (fi.fileName() == "2691-6AB8")
                     continue;
                mntUrlList << QUrl("file://" + fi.filePath());
            }
            fd.setSidebarUrls(list + mntUrlList);
            fd.update();
        });
        connect(&fd, &QFileDialog::finished, &fd, [=, &list, &fd](){
            fd.setSidebarUrls(list);
        });
        fd.setSidebarUrls(list + mntUrlList);
        fd.setDirectory(QDir(last_path));
        fd.setWindowTitle(tr("Video Player Choose a directory"));
        fd.setFileMode(QFileDialog::DirectoryOnly);

        g_shortcut->makeAllInvalid();
        kdk::LingmoStyleHelper::self()->removeHeader(&fd);
        if(fd.exec() == QFileDialog::Accepted)
        {
            url = fd.selectedUrls().at(0).toString(QUrl::DecodeReserved);
            if(url.startsWith("file")) {
                url.remove(0, 7);
            }
        }
        g_shortcut->makeAllValid();
    }

    if (!url.isEmpty()) {
        g_user_signal->addDir(url);
    }
    g_settings->setValue("History/last_path", url);
}

void MainWindow::slotOpenUrl()
{
    URLEditWidget url_edit;
    url_edit.setModal(true);
    if (url_edit.exec() > 0)
    {
        QString url = url_edit.getUrl();
        slotPlayFile(url, 0);
    }
}

void MainWindow::slotPlayFile(QString file, int pos)
{
    if (file == "")
        return;

    if (!file.startsWith("http"))
    {
        QFileInfo fi(file);
        if (!fi.exists())
            return;
    }

    // 开始播放隐藏所有
    m_stacked_widget->setCurrentIndex(1);
//    m_stacked_widget->setCurrentWidget(m_play_widget);
    m_title_widget->setHomePage(false);
    m_core->Open(file, pos);
    hideAll(true);
}

void MainWindow::slotPlayWidgetClicked()
{
#if 0
    if(m_playlist_widget->isShow())
        m_playlist_widget->slotHide();
    else
#endif
    {
        if(m_playlist_widget->isShow()) {
            m_playlist_widget->slotHide();
        }
        else {
            if(g_playstate == Mpv::Playing)
                m_core->Pause();
            else if(g_playstate == Mpv::Paused)
                m_core->Play();
        }
    }
}

void MainWindow::slotToTop(bool isTop)
{
    m_is_top = isTop;
    if (kdk::WindowManager::getwindowInfo(m_window_id).isKeepAbove() != isTop)
        kdk::WindowManager::keepWindowAbove(m_window_id);
}

void MainWindow::initGlobalSig()
{
    connect(g_user_signal, &GlobalUserSignal::sigShowStopFrame,     this, &MainWindow::slotShowDefault);
    connect(g_user_signal, &GlobalUserSignal::sigExit,              this, &MainWindow::close);
    connect(g_user_signal, &GlobalUserSignal::sigPlayWidgetClicked, this, &MainWindow::slotChangePause);
    connect(g_user_signal, &GlobalUserSignal::sigSelectFile,        this, &MainWindow::slotOpenFile);
    connect(g_user_signal, &GlobalUserSignal::sigSelectDir,         this, &MainWindow::slotOpenDir);
    connect(g_user_signal, &GlobalUserSignal::sigOpenUrl,           this, &MainWindow::slotOpenUrl);
    // 直接让 mpvcore 去播放有问题，必须先切换到播放页面
    connect(g_user_signal, &GlobalUserSignal::sigOpen,              this, &MainWindow::slotPlayFile);

    connect(g_user_signal, &GlobalUserSignal::sigPlayPause, [&](){
        if(g_playstate == Mpv::Playing)
            m_core->Pause();
        else if(g_playstate == Mpv::Paused)
            m_core->Play();
    });

    connect(g_user_signal, &GlobalUserSignal::sigOpenHelpDoc,   this, &MainWindow::slotOpenHelpDoc);
    connect(g_user_signal, &GlobalUserSignal::sigHideBar,       this, &MainWindow::hideAll);
    connect(g_core_signal, &GlobalCoreSignal::sigStateChange,   this, &MainWindow::slotPlayStateChange);
    connect(g_core_signal, &GlobalCoreSignal::sigFileInfoChange, [&](Mpv::FileInfo fi){
        int tmp_duration = m_file_info.length;
        m_file_info = fi;
        m_file_info.length = tmp_duration;
        m_current_file = fi.file_path;
        m_title_widget->setTitle(fi.file_path.split("/").back());
    });

    connect(g_user_signal, &GlobalUserSignal::sigChangeShowMode, [this](){
        if (m_stacked_widget->currentIndex() == 0)
            return;
        if (m_is_mini_mode)
            slotShowNormalMode();
        else
            slotShowMiniMode();
    });

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if 1
    connect(g_user_signal, &GlobalUserSignal::sigTheme, this, &MainWindow::slotThemeChange);
#endif

    connect(g_user_signal, &GlobalUserSignal::sigRightMenuShow, [&](){
//        if (!m_is_active && isWayland)
//            return;
        if (QGuiApplication::focusWindow())
            m_right_click_menu->exec(QCursor::pos(QGuiApplication::focusWindow()->screen()) + QPoint(1,1));
    });

#if 0
    connect(g_user_signal, &GlobalUserSignal::sigShowPlayList, [&](){
        if(!m_playlist_widget->isShow() && ui->stackedWidget->currentIndex() == 1) {
            slotHideAll(true);
        }
    });
#endif

    connect(g_core_signal, &GlobalCoreSignal::sigDuration, [&](QString, double duration){
        m_file_info.length = (int)duration;
    });

    connect(g_user_signal, &GlobalUserSignal::sigShowSetup, [&](int index){
        QWidget* show_parent = nullptr;
        if (isWayland)
            show_parent = this;
        SetUpDialog setup_dialog(show_parent);
        KyInfo() << setup_dialog.windowState() << " | " << setup_dialog.windowFlags();
        setup_dialog.setModal(true);
        setup_dialog.setIndex(index);
        kdk::LingmoStyleHelper::self()->removeHeader(&setup_dialog);
        setup_dialog.show();
        setup_dialog.exec();
    });

    connect(g_user_signal, &GlobalUserSignal::sigShowAbout, [&](){
        QWidget* show_parent = nullptr;
        if (isWayland)
            show_parent = this;
        KAboutDialog about_dialog(show_parent);
        about_dialog.setModal(true);
        about_dialog.setAppIcon(QIcon::fromTheme(qAppName()));
        about_dialog.setAppName(tr("Video Player"));
        about_dialog.setAppVersion(tr("version:").append(Functions::getVersion()));
        kdk::LingmoStyleHelper::self()->removeHeader(&about_dialog);
        about_dialog.show();
        about_dialog.exec();

#if 0
        AboutDialog aboutDialog;
        aboutDialog.move(this->geometry().center() - aboutDialog.geometry().center());

        if(isWayland)
        {
            LINGMODecorationManager::getInstance()->removeHeaderBar(aboutDialog.windowHandle());
            LINGMODecorationManager::getInstance()->setCornerRadius(aboutDialog.windowHandle(), 12, 12, 12, 12);
            aboutDialog.setFixedSize(aboutDialog.size());
        }
        aboutDialog.exec();
#endif
    });
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::WindowStateChange:
        windowStateChange();
        break;
    case QEvent::ContextMenu:
        // 主界面也能右键呼出菜单
        if (QGuiApplication::focusWindow()) {
            g_user_signal->showRightMenu();
        }
        break;
    case QEvent::MouseMove:
        setCursor(Qt::ArrowCursor);
        break;
    case QEvent::MouseButtonPress:
        // 告诉控制栏坐标
        m_contral_bar->setMainWidgetClickPos(static_cast<QMouseEvent*>(event)->pos());
        break;
    default:
        break;
    }

    if (((isWayland && event->type() == QEvent::WindowActivate) || (!isWayland && event->type() == QEvent::Show)) && m_need_play_when_min) {
        m_core->Play();
        m_need_play_when_min = false;
    }

    // wayland 最小化的时候没有 Hide 事件
    if (event->type() == QEvent::Hide && g_config->pauseWhenMini.first && g_playstate == Mpv::Playing) {
        m_core->Pause();
        m_need_play_when_min = true;
    }

    return QWidget::event(event);
}

void MainWindow::enterEvent(QEvent *event)
{
    hideAll(false);
    return QWidget::enterEvent(event);
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    if (!m_is_mini_mode && !m_is_maximized && !m_is_fullscreen)
        m_normal_rect = geometry();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    resetLayout();
    return QWidget::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 关闭的时候最小化到系统托盘 就是隐藏了就行
    if(g_config->miniToTray.first)
    {
        // 隐藏的时候有必要还原大小，不然再次显示会有问题
        if (isFullScreen()) {
            g_user_signal->fullScreen();
            QThread::msleep(300);
        }
        hide();
        event->ignore();
        resetLayout();
        return;
    }

    // 判断退出时是否需要清空播放列表
    if(g_config->clearListWhenExit.first || g_config->seamlessBrowsing.first)
        g_user_signal->clearPlayList();
    m_core->Stop();
    QThread::usleep(100000);

    // 关闭之前取消阻止锁屏
    m_dbus_interface->call("Uninhibit", m_inhibit_value);
    QWidget::closeEvent(event);
    exit(0);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
#if 0
    if (event->button() == Qt::LeftButton) {
        m_pressPos = event->pos();
        m_isMove = true;
    }else
#endif

    return QWidget::mousePressEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 按 esc 退出全屏
    if(event->key() == Qt::Key_Escape) {
        if(isFullScreen())
            g_user_signal->fullScreen();
    }
    return QWidget::keyPressEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> listUrl = event->mimeData()->urls();
    QStringList filelist;
    Extensions e;
    QRegExp rx_video(e.video().forRegExp());
    QRegExp rx_audio(e.audio().forRegExp());
    rx_video.setCaseSensitivity(Qt::CaseInsensitive);
    rx_audio.setCaseSensitivity(Qt::CaseInsensitive);
    for (QUrl url : listUrl)
    {
        QString path = url.path();

        // 不要 file:// 前缀只要绝对路径
        if(path.startsWith("file:"))
            path.remove(0, 7);

        // 拖入文件需要做类型判断
        QFileInfo fi(path);
        if (fi.isDir()) {
            // 如果是文件夹的话添加文件夹
            g_user_signal->addDir(path);
        }
        else if (rx_video.indexIn(fi.suffix()) > -1 || rx_audio.indexIn(fi.suffix()) > -1) {
            filelist << path;
        }
    }
    if(filelist.count() == 0)
        return;
    g_user_signal->addFiles(filelist);
}
