#include "kmenu.h"
#include <QPainter>
#include <QEvent>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QProcess>
#include <QGSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDesktopServices>
#include <QFileSystemWatcher>
#include <lingmo-log4qt.h>

#include "global/global.h"
#include "kaction.h"
#include "global/extensions.h"
#include "global/globalsignal.h"

#include <KF5/KWindowSystem/kwindoweffects.h>

using namespace Global;

KMenu::KMenu(QWidget *parent):
    QMenu(parent)
{
//    initStyle();
    setMouseTracking(true);
}

void KMenu::addAct(QAction *act)
{
    addAction(act);
}

void KMenu::initStyle()
{
    setMinimumWidth(MENUWIDTH);
    // 根据主题设置样式
    if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
        setLightTheme();
    else
        setBlackTheme();
    connect(g_gsettings, &QGSettings::changed, [&](QString key){
        // 如果不是跟随主题的话直接返回
        if (key == "styleName") {
            if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                setLightTheme();
            else
                setBlackTheme();
        }
    });
    connect(g_user_signal, &GlobalUserSignal::sigTheme, [&](int theme){
        switch (theme) {
        case 0:
            if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                setLightTheme();
            else
                setBlackTheme();
            break;
        case 1:
            setLightTheme();
            break;
        case 2:
            setBlackTheme();
            break;
        default:
            break;
        }
    });
}


void KMenu::setBlackTheme()
{
    if(!isWayland)
        return;
    setStyleSheet("QMenu{background-color:rgb(61,61,65);border-radius:8px;padding:4px 0px;margin:4px 4px;}"
                  "QMenu::item { \
                        color: rgb(210, 210, 210); \
                        background-color: transparent; \
                        border-radius: 4px; \
                        padding:6px 1px;    /*设置菜单项文字上下和左右的内边距，效果就是菜单中的条目左右上下有了间隔*/ \
                        padding-left: 25px;\
                        margin:0px 3px;     /*设置菜单项的外边距*/ \
                    } \
                    QMenu::separator { \
                        height: 1px; \
                        background: rgb(100,100,100); \
                        margin-left: 10px; \
                        margin-right: 10px; \
                    }\
                    QMenu::item:!enabled{color: rgb(155, 155, 155);}\
                    QMenu::item:selected { /* when user selects item using mouse or keyboard */ \
                        background-color: rgb(72,72,76);/*这一句是设置菜单项鼠标经过选中的样式*/ \
                    }");
}

void KMenu::setLightTheme()
{
    if(!isWayland)
        return;
    setStyleSheet("QMenu{background-color:rgb(255,255,255);border-radius:8px;padding:4px 0px;margin:4px 4px;}"
                  "QMenu::item { \
                        color: rgb(48,49,51); \
                        background-color: transparent; \
                        border-radius: 4px; \
                        padding:6px 1px;/*设置菜单项文字上下和左右的内边距，效果就是菜单中的条目左右上下有了间隔*/ \
                        padding-left: 25px;\
                        margin:0px 3px;/*设置菜单项的外边距*/ \
                    } \
                    QMenu::separator { \
                        height: 1px; \
                        background: rgb(100,100,100); \
                        margin-left: 10px; \
                        margin-right: 10px; \
                    } \
                    QMenu::item:!enabled{color: rgb(155, 155, 155);} \
                    QMenu::item:selected { /* when user selects item using mouse or keyboard */ \
                        background-color: rgb(247,247,247);/*这一句是设置菜单项鼠标经过选中的样式*/ \
                    }");
}

KRightClickMenu::KRightClickMenu(QWidget *parent):
    KMenu(parent)
{
    createOpenMenu();
    createSetTopAction();
    createPlayOrderMenu();
    createFrameMenu();
    createAudioMenu();
    createSubMenu();
    createPlayMenu();
    createPlayerSetAction();
    createMediaInfoAction();

//    addMenu(menu_open);
    addAction(act_openFile);
    addAction(act_openDirectory);
    addAction(act_openURL);
    addAction(act_setTop);
    addMenu(menu_playOrder);
    addMenu(menu_frame);
    addMenu(menu_audio);
    addMenu(menu_subtitle);
    addMenu(menu_play);
    addAction(act_playerSet);
    addAction(act_MediaInfo);
    initGlobalSig();

    // 初始化的时候设置一些按钮不可用，此些功能只有在播放时可用
    act_MediaInfo->setEnabled(false);
    act_showInfo->setEnabled(false);
    act_defaultChannel->setEnabled(false);
    act_stereo->setEnabled(false);
    act_leftChannel->setEnabled(false);
    act_rightChannel->setEnabled(false);
    act_playPause->setEnabled(false);
    act_playForward->setEnabled(false);
    act_playBackward->setEnabled(false);
}

/** ********************************************
* 创建打开菜单
***********************************************/
void KRightClickMenu::createOpenMenu()
{
//    menu_open = new KMenu(parentWidget());
//    menu_open->menuAction()->setText(tr("Open"));
    act_openFile = new KAction(QKeySequence(g_settings->value("action/open file").toString()), nullptr, "open_file");
    connect(act_openFile, &KAction::triggered, [this](){emit sigOpenFile();});
    act_openFile->change(tr("Open &File..."));
    g_shortcut->registerAction(tr("open file"), act_openFile);

    act_openDirectory = new KAction(QKeySequence(g_settings->value("action/open dir").toString()), nullptr, "open_directory");
    connect(act_openDirectory, &KAction::triggered, [this](){emit sigOpenDir();});
    act_openDirectory->change(tr("Open &Directory..."));
    g_shortcut->registerAction(tr("open dir"), act_openDirectory);

    act_openURL = new KAction(QKeySequence(g_settings->value("action/open url").toString()), nullptr, "open_url" );
    connect(act_openURL, &KAction::triggered, [this](){g_user_signal->openUrl();});
    act_openURL->change(tr("Open URL"));
    g_shortcut->registerAction(tr("open url"), act_openURL);

//    menu_open->addAction(act_openFile);
//    menu_open->addAction(act_openDirectory);
//    menu_open->addAction(act_openURL);
}

/** ********************************************
* 创建置顶动作
***********************************************/
void KRightClickMenu::createSetTopAction()
{
    act_setTop = new KAction(QKeySequence(g_settings->value("action/to top").toString()), nullptr, "to_top");
    act_setTop->setCheckable(true);
    g_shortcut->registerAction(tr("to top"), act_setTop);
    // 初始化的时候读取配置文件查看是否置顶
    bool ini_onTop = g_settings->value("General/is_on_top").toBool();
    act_setTop->setChecked(ini_onTop);
    if(ini_onTop)
        QTimer::singleShot(500, [&](){emit sigToTop(act_setTop->isChecked());});
    connect(act_setTop, &KAction::triggered, [this](){
        g_settings->setValue("General/is_on_top", act_setTop->isChecked());
        emit sigToTop(act_setTop->isChecked());
    });
    act_setTop->change(tr("ToTop"));
}

/** ********************************************
* 创建播放顺序菜单
***********************************************/
void KRightClickMenu::createPlayOrderMenu()
{
    menu_playOrder = new KMenu(parentWidget());
    menu_playOrder->menuAction()->setText(tr("Order"));

    act_oneLoop = new KAction(nullptr, "one_loop");
    act_oneLoop->change(tr("One Loop"));
    act_oneLoop->setData(ONE_LOOP);

    act_sequence = new KAction( nullptr, "sequence");
    act_sequence->change(tr("Sequence"));
    act_sequence->setData(SEQUENCE);

    act_listLoop = new KAction(nullptr, "list_loop" );
    act_listLoop->change(tr("List loop"));
    act_listLoop->setData(LIST_LOOP);

    act_random = new KAction(nullptr, "random" );
    act_random->change(tr("Random"));
    act_random->setData(RANDOM);

    act_oneLoop->setCheckable(true);
    act_sequence->setCheckable(true);
    act_listLoop->setCheckable(true);
    act_random->setCheckable(true);

    menu_playOrder->addAction(act_oneLoop);
//    menu_playOrder->addAction(act_sequence);
    menu_playOrder->addAction(act_listLoop);
    menu_playOrder->addAction(act_random);

    group_playOrder = new QActionGroup(nullptr);
    group_playOrder->addAction(act_oneLoop);
    group_playOrder->addAction(act_sequence);
    group_playOrder->addAction(act_listLoop);
    group_playOrder->addAction(act_random);
    connect(group_playOrder, &QActionGroup::triggered, [&](QAction *act){
        if(act == act_oneLoop)
            g_user_signal->setPlayOrder(ONE_LOOP);
        else if(act == act_sequence)
            g_user_signal->setPlayOrder(SEQUENCE);
        else if(act == act_listLoop)
            g_user_signal->setPlayOrder(LIST_LOOP);
        else if(act == act_random)
            g_user_signal->setPlayOrder(RANDOM);
    });

    int playOrderIndex = g_settings->value("General/play_order").toInt();
    g_user_signal->setPlayOrder((PlayOrder)playOrderIndex);
    int i = 0;
    for(auto act : menu_playOrder->actions())
    {
        if(i == playOrderIndex)
        {
            act->setChecked(true);
            break;
        }
        i++;
    }
}

/** ********************************************
* 创建视频画面菜单
***********************************************/
void KRightClickMenu::createFrameMenu()
{
    menu_frame = new KMenu(parentWidget());
    menu_frame->menuAction()->setText(tr("Frame"));

// ==================================画面比例==================================

    act_defaultFrame = new KAction(nullptr, "default_frame");
    connect(act_defaultFrame, &KAction::triggered, [this](){emit sigDefaultFrame();});
    act_defaultFrame->change(tr("Default frame"));

    act_4div3Frame = new KAction(nullptr, "4_div_3");
    connect(act_4div3Frame, &KAction::triggered, [this](){emit sig4Div3Frame();});
    act_4div3Frame->change(tr("4:3"));

    act_16div9Frame = new KAction(nullptr, "16_div_9");
    connect(act_16div9Frame, &KAction::triggered, [this](){emit sig16Div9Frame();});
    act_16div9Frame->change(tr("16:9"));

    act_fullFrame = new KAction(nullptr, "full_frame");
    connect(act_fullFrame, &KAction::triggered, [this](){emit sigFullFrame();});
    act_fullFrame->change(tr("Full frame"));

    group_frameScale = new QActionGroup(nullptr);
    group_frameScale->addAction(act_defaultFrame);
    group_frameScale->addAction(act_4div3Frame);
    group_frameScale->addAction(act_16div9Frame);
    group_frameScale->addAction(act_fullFrame);
    connect(group_frameScale, &QActionGroup::triggered, [&](QAction *act){
        if(act == act_defaultFrame)
            g_user_signal->setAspect(Mpv::AUTO);
        else if(act == act_4div3Frame)
            g_user_signal->setAspect(Mpv::DIV_4_3);
        else if(act == act_16div9Frame)
            g_user_signal->setAspect(Mpv::DIV_16_9);
        else if(act == act_fullFrame)
            g_user_signal->setAspect(Mpv::FULL);
    });

    act_defaultFrame->setCheckable(true);
    act_4div3Frame->setCheckable(true);
    act_16div9Frame->setCheckable(true);
    act_fullFrame->setCheckable(true);
    act_defaultFrame->setChecked(true);
// ==================================画面比例==================================
    act_restoreFrame =  new KAction(QKeySequence(g_settings->value("action/" + tr("restore frame")).toString()), nullptr, "restore_frame");
    connect(act_restoreFrame, &KAction::triggered, [this](){
        g_user_signal->restoreFrame();
        act_defaultFrame->setChecked(true);
        emit sigRestoreFrame();
    });
    act_restoreFrame->change(tr("restore frame"));

    act_alongRotate = new KAction(QKeySequence(g_settings->value("action/" + tr("forward rotate")).toString()), nullptr, "along_rotate");
    connect(act_alongRotate, &KAction::triggered, [this](){
        g_user_signal->clockwiseRotate();
        emit sigAlongRotate();
    });
    act_alongRotate->change(tr("Along rotate"));
    g_shortcut->registerAction(tr("forward rotate"), act_alongRotate);

    act_inverseRotate = new KAction(QKeySequence(g_settings->value("action/" + tr("backward rotate")).toString()), nullptr, "inverse_rotate");
    connect(act_inverseRotate, &KAction::triggered, [this](){
        g_user_signal->counterClockwiseRotate();
        emit sigInverseRotate();
    });
    act_inverseRotate->change(tr("Inverse rotate"));
    g_shortcut->registerAction(tr("backward rotate"), act_inverseRotate);

    act_flipHorizontally = new KAction(QKeySequence(g_settings->value("action/" + tr("horizontal flip")).toString()), nullptr, "horizontally_flip");
    connect(act_flipHorizontally, &KAction::triggered, [this](){g_user_signal->horizontallyFlip();});
    act_flipHorizontally->change(tr("Horizontally flip"));
    g_shortcut->registerAction(tr("horizontal flip"), act_flipHorizontally);

    act_flipVertically = new KAction(QKeySequence(g_settings->value("action/" + tr("vertical flip")).toString()), nullptr, "vertically_flip");
    connect(act_flipVertically, &KAction::triggered, [this](){g_user_signal->verticalFlip();});
    act_flipVertically->change(tr("Vertically flip"));
    g_shortcut->registerAction(tr("vertical flip"), act_flipHorizontally);

    act_showInfo = new KAction(nullptr, "show_profile");
    connect(act_showInfo, &KAction::triggered, [this](bool checked){
        g_user_signal->showInfo(checked);
    });
    act_showInfo->change(tr("Show profile"));
    act_showInfo->setCheckable(true);

    menu_frame->addAction(act_defaultFrame);
    menu_frame->addAction(act_4div3Frame);
    menu_frame->addAction(act_16div9Frame);
    menu_frame->addAction(act_fullFrame);
    menu_frame->addSeparator();
    menu_frame->addAction(act_restoreFrame);
    menu_frame->addAction(act_alongRotate);
    menu_frame->addAction(act_inverseRotate);
    menu_frame->addAction(act_flipHorizontally);
    menu_frame->addAction(act_flipVertically);
    menu_frame->addAction(act_showInfo);
}

/** ********************************************
* 创建声音菜单
***********************************************/
void KRightClickMenu::createAudioMenu()
{
    menu_audio = new KMenu(parentWidget());
    menu_audio->menuAction()->setText(tr("Audio"));

    act_audioTrack = new KMenu(parentWidget());
    act_audioTrack->menuAction()->setText(tr("AudioTrack"));
    act_audioTrack->setEnabled(false);

    group_audio = new QActionGroup(nullptr);
    connect(group_audio, &QActionGroup::triggered, [&](QAction* act){
        g_user_signal->setAudioId(act->data().toInt());
    });
    addAudioTrack("aaa", -1);

    act_audioChannel = new KMenu(parentWidget());
    act_audioChannel->menuAction()->setText(tr("AudioChannel"));

    act_defaultChannel = new KAction(QKeySequence("/"), nullptr, "defalut");
    connect(act_defaultChannel, &KAction::triggered, [this](){
        emit sigDefalutChannel();
        g_user_signal->setChannel(Mpv::Default);
    });
    act_defaultChannel->change(tr("Default"));

    act_stereo = new KAction(QKeySequence("/"), nullptr, "stereo");
    connect(act_stereo, &KAction::triggered, [this](){
        emit sigStereo();
        g_user_signal->setChannel(Mpv::Stereo);
    });
    act_stereo->change(tr("Stereo"));

    act_leftChannel = new KAction(QKeySequence(","), nullptr, "left_channel");
    connect(act_leftChannel, &KAction::triggered, [this](){
        emit sigLeftChannel();
        g_user_signal->setChannel(Mpv::Left);
    });
    act_leftChannel->change(tr("Left channel"));

    act_rightChannel = new KAction(QKeySequence("."), nullptr, "right_channel");
    connect(act_rightChannel, &KAction::triggered, [this](){
        emit sigRightChannel();
        g_user_signal->setChannel(Mpv::Right);
    });
    act_rightChannel->change(tr("Right channel"));

    {
        // 设置声道选择可选
        act_stereo->setCheckable(true);
        act_leftChannel->setCheckable(true);
        act_rightChannel->setCheckable(true);

        act_audioChannel->addAction(act_stereo);
        act_audioChannel->addAction(act_leftChannel);
        act_audioChannel->addAction(act_rightChannel);

        group_channel = new QActionGroup(nullptr);
        group_channel->addAction(act_stereo);
        group_channel->addAction(act_leftChannel);
        group_channel->addAction(act_rightChannel);

        // 通过配置文件看设置什么声道
        switch (g_config->audioChannel.first) {
        case 0:
            act_stereo->setChecked(true);
            break;
        case 1:
            act_leftChannel->setChecked(true);
            break;
        case 2:
            act_rightChannel->setChecked(true);
            break;
        default:
            break;
        }
    }

    act_audioSet = new KAction(nullptr, "audio_set");
    connect(act_audioSet, &KAction::triggered, [this](){g_user_signal->showSetup(4);});
    act_audioSet->change(tr("Audio set"));

    menu_audio->addMenu(act_audioTrack);
    menu_audio->addMenu(act_audioChannel);
    menu_audio->addAction(act_audioSet);

}

/** ********************************************
* 创建字幕菜单
***********************************************/
void KRightClickMenu::createSubMenu()
{
    menu_subtitle = new KMenu(parentWidget());
    menu_subtitle->menuAction()->setText(tr("Subtitle"));

    act_loadSubtitle = new KAction(QKeySequence(g_settings->value("action/sub load").toString()), nullptr, "load_subtitle");
    g_shortcut->registerAction(tr("sub load"), act_loadSubtitle);
    connect(act_loadSubtitle, &KAction::triggered, [this](){
        if (!act_loadSubtitle->isEnabled())
            return;
        Extensions e;
#if 1
        QString fileName;
        {
            QWidget *parent_widget = nullptr;
            if (isWayland)
                parent_widget = parentWidget();
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
                qDebug() << list + mntUrlList;
                fd.setSidebarUrls(list + mntUrlList);
                fd.update();
            });
            connect(&fd, &QFileDialog::finished, &fd, [=, &list, &fd](){
                fd.setSidebarUrls(list);
            });
            fd.setSidebarUrls(list + mntUrlList);
            fd.setDirectory(QDir(g_config->subDir.first));
            fd.setWindowTitle(tr("Video Player Choose a file"));

            QString setFilterS = tr("Subtitles") + (" (%1) ");
            setFilterS = setFilterS.arg(e.subtitles().forFilter());
            fd.setNameFilters(QStringList() << setFilterS);
            fd.setOption(QFileDialog::HideNameFilterDetails);

            g_shortcut->makeAllInvalid();
            if(fd.exec() == QFileDialog::Accepted)
            {
                fileName = fd.selectedFiles().at(0);
                qDebug() << fileName;
            }
            g_shortcut->makeAllValid();
        }
#else

        QString fileName = QFileDialog::getOpenFileName(
            nullptr, tr("Video Player Choose a file"),
            g_config->subDir.first,
            tr("Subtitles") + e.subtitles().forFilter()+ ";;" +
            tr("All files") +" (*.*)" );
#endif
        if(fileName == "")
            return;
        g_user_signal->addSub(fileName);
    });
    act_loadSubtitle->change(tr("Load subtitle"));
    menu_subtitle->addAction(act_loadSubtitle);

    menu_subtitleSelect = new KMenu(parentWidget());
    menu_subtitleSelect->setToolTipsVisible(true);
    menu_subtitleSelect->menuAction()->setText(tr("Subtitle select"));
    menu_subtitle->addMenu(menu_subtitleSelect);

    act_noSubtitle = new KAction(nullptr, "no_subtitle");
    connect(act_noSubtitle, &KAction::triggered, [this](){emit sigNoSubtitle();});
    act_noSubtitle->change(tr("No subtitle"));
    act_noSubtitle->setCheckable(true);
    act_noSubtitle->setChecked(true);
    menu_subtitleSelect->addAction(act_noSubtitle);

    group_sub = new QActionGroup(nullptr);
    group_sub->addAction(act_noSubtitle);
    act_noSubtitle->setData(0);
    connect(group_sub, &QActionGroup::triggered, [&](QAction* act){
        g_user_signal->setSubId(act->data().toInt());
    });

    act_matchSubtitle = new KAction(nullptr, "match_subtitle");
    connect(act_matchSubtitle, &KAction::triggered, [this](){emit sigMatchSubtitle();});
    act_matchSubtitle->change(tr("Match subtitle"));
//    menu_subtitle->addAction(act_matchSubtitle);       // 匹配字幕暂时隐藏

    act_searchSubtitle = new KAction(nullptr, "search_subtitle");
    connect(act_searchSubtitle, &KAction::triggered, [this](){emit sigSearchSubtitle();});
    act_searchSubtitle->change(tr("Search subtitle"));
//    menu_subtitle->addAction(act_searchSubtitle);      // 搜索字幕暂时隐藏

    act_subtitleSet = new KAction(nullptr, "subtitle_set");
    connect(act_subtitleSet, &KAction::triggered, [this](){g_user_signal->showSetup(3);});
    act_subtitleSet->change(tr("Subtitle set"));
    menu_subtitle->addAction(act_subtitleSet);
}

/** ********************************************
 * 创建播放控制菜单
 ***********************************************/
void KRightClickMenu::createPlayMenu()
{
    menu_play = new KMenu(parentWidget());
    menu_play->menuAction()->setText(tr("Play"));

    act_playPause = new KAction(QKeySequence(g_settings->value("action/play/pause").toString()), nullptr, "play_pause");
    connect(act_playPause, &KAction::triggered, [this](){g_user_signal->play_pause();});
    act_playPause->change(tr("Play/Pause"));
    menu_play->addAction(act_playPause);
    g_shortcut->registerAction(act_playPause->text(), act_playPause);

    act_volumeUp = new KAction(QKeySequence(g_settings->value("action/volume up").toString()), nullptr, "volume_up");
    connect(act_volumeUp, &KAction::triggered, [this](){g_user_signal->setVolumeUp(10);});
    act_volumeUp->change(tr("volume up"));
    menu_play->addAction(act_volumeUp);
    g_shortcut->registerAction(act_volumeUp->text(), act_volumeUp);

    act_volumeDown = new KAction(QKeySequence(g_settings->value("action/volume down").toString()), nullptr, "volume_down");
    connect(act_volumeDown, &KAction::triggered, [this](){g_user_signal->setVolumeDown(10);});
    act_volumeDown->change(tr("volume down"));
    menu_play->addAction(act_volumeDown);
    g_shortcut->registerAction(act_volumeDown->text(), act_volumeDown);

    act_playForward = new KAction(QKeySequence("Right"), nullptr, "play_forward");
    connect(act_playForward, &KAction::triggered, [this](){g_user_signal->forword(true);});
    act_playForward->change(tr("forward"));
    menu_play->addAction(act_playForward);
    g_shortcut->registerAction(act_playForward->text(), act_playForward);

    act_playBackward = new KAction(QKeySequence("Left"), nullptr, "play_backward");
    connect(act_playBackward, &KAction::triggered, [this](){g_user_signal->backword(true);});
    act_playBackward->change(tr("backward"));
    menu_play->addAction(act_playBackward);
    g_shortcut->registerAction(act_playBackward->text(), act_playBackward);
}

/** ********************************************
* 创建播放器设置动作
***********************************************/
void KRightClickMenu::createPlayerSetAction()
{
    act_playerSet = new KAction(QKeySequence(g_settings->value("action/setup").toString()), nullptr, "player_set");
    connect(act_playerSet, &KAction::triggered, [this](){g_user_signal->showSetup(0);});
    act_playerSet->change(tr("setup"));
    g_shortcut->registerAction(act_playerSet->text(), act_playerSet);
}

/** ********************************************
* 创建媒体信息动作
***********************************************/
void KRightClickMenu::createMediaInfoAction()
{
    act_MediaInfo = new KAction(nullptr, "media_info");
    connect(act_MediaInfo, &KAction::triggered, [this](){emit sigMediaInfo();});
    act_MediaInfo->change(tr("Media info"));
}

void KRightClickMenu::initGlobalSig()
{
    connect(g_user_signal, &GlobalUserSignal::sigChannel, [this](Mpv::Channel c){
        switch (c) {
        case Mpv::Stereo:
            act_stereo->setChecked(true);
            break;
        case Mpv::Left:
            act_leftChannel->setChecked(true);
            break;
        case Mpv::Right:
            act_rightChannel->setChecked(true);
            break;
        default:
            break;
        }
    });
    connect(g_user_signal, &GlobalUserSignal::sigSelectSub, act_loadSubtitle, &KAction::trigger);
    connect(g_user_signal, &GlobalUserSignal::sigToTop, act_setTop, &KAction::trigger);
    connect(g_core_signal, &GlobalCoreSignal::sigTracksChange, this, &KRightClickMenu::loadTracks);
    connect(g_core_signal, &GlobalCoreSignal::sigStateChange, [&](){
        if(g_playstate <= 0)
            reInit();
        else if(g_playstate == Mpv::Playing)
            enableActions();
        else if(g_playstate == Mpv::Started)
            g_user_signal->setAspect(Mpv::AUTO);
    });
    connect(g_core_signal, &GlobalCoreSignal::sigSubIdChange, [&](int sub_id){
        for(QAction *act:group_sub->actions())
            if(act->data().toInt() == sub_id)
                act->setChecked(true);
    });
    connect(g_core_signal, &GlobalCoreSignal::sigAudioIdChange, [&](int audio_id){
        for(QAction *act:group_audio->actions())
            if(act->data().toInt() == audio_id)
                act->setChecked(true);
    });
    connect(g_core_signal, &GlobalCoreSignal::sigVideoIdChange, [&](int video_id){
        if (video_id >= 0) {
            act_loadSubtitle->setEnabled(true);
            menu_subtitleSelect->setEnabled(true);
        }
    });
    connect(g_core_signal, &GlobalCoreSignal::sigFileInfoChange, [&](Mpv::FileInfo fi){
        disableVideoMenu();
        if (fi.video_params.codec != "") {
            if (fi.video_params.codec.indexOf("jpeg") < 0 &&
                fi.video_params.codec.indexOf("png") < 0 ) {
                enableVideoMenu();
            }
        }
    });
    connect(g_user_signal, &GlobalUserSignal::sigPlayOrder, [&](PlayOrder order){
        for(QAction *act:menu_playOrder->actions())
            if(act->data().toInt() == order)
                act->setChecked(true);
    });
}

void KRightClickMenu::addSubtitle(QString name, int id)
{
    QAction *sub = new QAction(name);
    // 显示的名称需要有最大宽度
    QFontMetrics fontWidth(sub->font());//得到每个字符的宽度
    QString show_name = fontWidth.elidedText(name, Qt::ElideRight, 200);//最大宽度200像素

    sub->setText(show_name);
    sub->setToolTip(name);
    sub->setData(id);
    sub->setCheckable(true);
    group_sub->addAction(sub);
    menu_subtitleSelect->addAct(sub);
}

void KRightClickMenu::addAudioTrack(QString name, int id)
{
    QAction *audio = new QAction(name);
    audio->setData(id);
    audio->setCheckable(true);
    group_audio->addAction(audio);
    act_audioTrack->addAct(audio);
}

void KRightClickMenu::loadTracks(QList<Mpv::Track> tracks)
{
    // 删除右键菜单中的轨道全部重新加载
    for(QAction *act : menu_subtitleSelect->actions())
    {
        if(act == act_noSubtitle)
            continue;
        menu_subtitleSelect->removeAction(act);
        group_sub->removeAction(act);
        delete act;
    }

    foreach (QAction *act, act_audioTrack->actions())
    {
        act_audioTrack->removeAction(act);
        group_audio->removeAction(act);
        delete act;
    }

    for(Mpv::Track track : tracks)
    {
        if(track.type == "video")
            continue;
        else if(track.type == "audio")
        {
            act_audioTrack->setEnabled(true);
            addAudioTrack(QString("audio ").append(QString::number(track.id)), track.id);
        }
        else if(track.type == "sub")
            addSubtitle(track.title, track.id);
    }
}

/** ********************************************
* 视频播放之后重新初始化视频相关菜单项
***********************************************/
void KRightClickMenu::reInit()
{
    for(QAction *act : menu_subtitleSelect->actions())
    {
        if(act == act_noSubtitle)
            continue;
        menu_subtitleSelect->removeAction(act);
        group_sub->removeAction(act);
        delete act;
    }
    act_noSubtitle->setChecked(true);

    foreach (QAction *act, act_audioTrack->actions())
    {
        act_audioTrack->removeAction(act);
        group_audio->removeAction(act);
        delete act;
    }
    act_defaultFrame->setChecked(true);

    act_MediaInfo->setEnabled(false);
    act_showInfo->setEnabled(false);
    act_defaultChannel->setEnabled(false);
    act_stereo->setEnabled(false);
    act_leftChannel->setEnabled(false);
    act_rightChannel->setEnabled(false);
    act_playPause->setEnabled(false);
    act_playForward->setEnabled(false);
    act_playBackward->setEnabled(false);
    act_audioTrack->setEnabled(false);
    act_loadSubtitle->setEnabled(false);
    menu_subtitleSelect->setEnabled(false);

    disableVideoMenu();
}

void KRightClickMenu::enableActions()
{
    act_MediaInfo->setEnabled(true);
    act_showInfo->setEnabled(true);
    act_defaultChannel->setEnabled(true);
    act_stereo->setEnabled(true);
    act_leftChannel->setEnabled(true);
    act_rightChannel->setEnabled(true);
    act_playPause->setEnabled(true);
    act_playForward->setEnabled(true);
    act_playBackward->setEnabled(true);
}

// 设置视频相关选项不生效
void KRightClickMenu::disableVideoMenu()
{
    act_4div3Frame->setEnabled(false);
    act_16div9Frame->setEnabled(false);
    act_fullFrame->setEnabled(false);
    act_alongRotate->setEnabled(false);
    act_inverseRotate->setEnabled(false);
    act_flipHorizontally->setEnabled(false);
    act_flipVertically->setEnabled(false);
}

void KRightClickMenu::enableVideoMenu()
{
    if ((g_config->videoOutputType() == GlobalConfig::VO_WID &&
            (g_config->videoOutput.first == "vdpau" ||
            g_config->videoOutput.first == "vaapi")) ||
            g_config->videoDecoder.first == "vdpau" ||
            g_config->videoDecoder.first == "vaapi") {
        // vdpau 和 vaapi 不支持视频修改比例
        act_4div3Frame->setEnabled(false);
        act_16div9Frame->setEnabled(false);
        act_fullFrame->setEnabled(false);
    }
    else {
        act_4div3Frame->setEnabled(true);
        act_16div9Frame->setEnabled(true);
        act_fullFrame->setEnabled(true);
    }

    // 某些显卡一些画面设置不生效
    if (g_config->hardwareType() == GlobalConfig::GlenFly_VAAPI && g_config->videoOutput.first != "x11") {
        act_alongRotate->setEnabled(false);
        act_inverseRotate->setEnabled(false);
        act_flipHorizontally->setEnabled(false);
        act_flipVertically->setEnabled(false);
    }
    else {
        act_alongRotate->setEnabled(true);
        act_inverseRotate->setEnabled(true);
        act_flipHorizontally->setEnabled(true);
        act_flipVertically->setEnabled(true);
    }
}

#if 0
ListLoopMenu::ListLoopMenu(QWidget *parent):
    KMenu(parent)
{
    if(isWayland)
        setFixedWidth(160);
    // 内容较少直接在构造初始化
    act_oneLoop = new KAction(nullptr, "one_loop");
    act_oneLoop->change(tr("One Loop"));
    act_oneLoop->setCheckable(false);
    connect(act_oneLoop, &KAction::triggered, [this](){g_user_signal->setPlayOrder(ONE_LOOP);});

    act_listLoop = new KAction(nullptr, "list_loop" );
    act_listLoop->change(tr("List loop"));
    act_listLoop->setCheckable(false);
    connect(act_listLoop, &KAction::triggered, [this](){g_user_signal->setPlayOrder(LIST_LOOP);});

    act_random = new KAction(nullptr, "random" );
    act_random->change(tr("Random"));
    act_random->setCheckable(false);
    connect(act_random, &KAction::triggered, [this](){g_user_signal->setPlayOrder(RANDOM);});

    act_sequence = new KAction( nullptr, "sequence");
    act_sequence->change(tr("Sequence"));
    act_sequence->setCheckable(false);
    connect(act_sequence, &KAction::triggered, [this](){g_user_signal->setPlayOrder(SEQUENCE);});

    addAct(act_oneLoop);
    addAct(act_listLoop);
    addAct(act_random);
//    addAct(act_sequence);

    // 根据主题设置样式
    if(g_settings->value("General/follow_system_theme").toBool())
    {
        if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
            setLightTheme();
        else
            setBlackTheme();
    }
    else
    {
        if(g_settings->value("General/theme").toInt() == 0)
            setLightTheme();
        else
            setBlackTheme();
    }
    connect(g_gsettings, &QGSettings::changed, [&](QString key){
        // 如果不是跟随主题的话直接返回
        if(key == "styleName")
            if(g_settings->value("General/follow_system_theme").toBool())
                if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                    setLightTheme();
                else
                    setBlackTheme();
    });

    connect(g_user_signal, &GlobalUserSignal::sigTheme, [&](int theme){
        switch (theme) {
        case 0:
            if(g_gsettings->get("style-name").toString() == STYLE_LINGMO_LIGHT)
                setLightTheme();
            else
                setBlackTheme();
            break;
        case 1:
            setLightTheme();
            break;
        case 2:
            setBlackTheme();
            break;
        default:
            break;
        }
    });
}

void ListLoopMenu::setBlackTheme()
{
    act_oneLoop->setIcon(QIcon(":/ico/media-playlist-repeat-one-h.png"));
    act_listLoop->setIcon(QIcon(":/ico/media-playlist-repeat-loop-h.png"));
    act_random->setIcon(QIcon(":/ico/media-playlist-repeat-radom-h.png"));
    act_sequence->setIcon(QIcon(":/ico/media-playlist-repeat-list-h.png"));

    if(!isWayland)
        return;

    setStyleSheet("QMenu{background-color:rgb(61,61,65);border-radius:8px;padding:4px 0px;margin:4px 4px;}"
                  "QMenu::item { \
                        color: rgb(210, 210, 210); \
                        background-color: transparent; \
                        border-radius: 4px; \
                        padding:8px 5px;/*设置菜单项文字上下和左右的内边距，效果就是菜单中的条目左右上下有了间隔*/ \
                        margin:0px 3px;/*设置菜单项的外边距*/ \
                    } \
                    QMenu::item:selected { /* when user selects item using mouse or keyboard */ \
                        background-color: rgba(55,144,250,191);/*这一句是设置菜单项鼠标经过选中的样式*/ \
                    }");
}

void ListLoopMenu::setLightTheme()
{
    act_oneLoop->setIcon(QIcon(":/ico_light/media-playlist-repeat-one-h.png"));
    act_listLoop->setIcon(QIcon(":/ico_light/media-playlist-repeat-loop-h.png"));
    act_random->setIcon(QIcon(":/ico_light/media-playlist-repeat-radom-h.png"));
    act_sequence->setIcon(QIcon(":/ico_light/media-playlist-repeat-list-h.png"));

    if(!isWayland)
        return;

    setStyleSheet("QMenu{background-color:rgb(255,255,255);border-radius:8px;padding:4px 0px;margin:4px 4px;}"
                  "QMenu::item { \
                        color: rgb(38, 38, 38); \
                        background-color: transparent; \
                        border-radius: 4px; \
                        padding:8px 5px;/*设置菜单项文字上下和左右的内边距，效果就是菜单中的条目左右上下有了间隔*/ \
                        margin:0px 3px;/*设置菜单项的外边距*/ \
                    } \
                    QMenu::item:selected { /* when user selects item using mouse or keyboard */ \
                        background-color: rgba(55,144,250,191);/*这一句是设置菜单项鼠标经过选中的样式*/ \
                    }");
}
#endif

TitleMenu::TitleMenu(QWidget *parent):
    KMenu(parent)
{
    createOneLevelAction();
    createThemeMenu();
    createPrivacyMenu();
    createHelpMenu();
    createSetupMenu();

    // 有些功能暂没有实现
//    addAct(act_uploadToCloud);
//    addMenu(menu_theme);
    addMenu(menu_privacy);
    addMenu(menu_setup);
//    addMenu(menu_help);
    addAct(act_f1);
    addAct(act_about);
    addAct(act_quit);
}

TitleMenu::~TitleMenu()
{

}

void TitleMenu::createOneLevelAction()
{
    act_uploadToCloud = new KAction(nullptr, "upload2Cloud");
    act_uploadToCloud->change(tr("Upload to cloud"));

    act_about = new KAction(nullptr, "about");
    connect(act_about, &KAction::triggered, [this](){g_user_signal->showAbout();});
    act_about->change(tr("About"));

    act_quit = new KAction(nullptr, "quit");
    connect(act_quit, &KAction::triggered, [this](){emit sigQuit();});
    act_quit->change(tr("Quit"));

}

/** *******************************************
* 初始主题选择菜单
***********************************************/
void TitleMenu::createThemeMenu()
{
    menu_theme = new KMenu;
    menu_theme->menuAction()->setText(tr("Theme"));
    group_themeGroup = new QActionGroup(nullptr);

    act_followSystem = new KAction(nullptr, "follow_system");
    connect(act_followSystem, &KAction::triggered, [this](){
        g_settings->setValue("General/follow_system_theme", 1);
        g_user_signal->setTheme(0);
    });
    act_followSystem->change(tr("Follow system"));
    act_followSystem->setCheckable(true);

    act_lightTheme = new KAction(nullptr, "light_theme");
    connect(act_lightTheme, &KAction::triggered, [this](){
        g_user_signal->setTheme(1);
        g_settings->setValue("General/follow_system_theme", 0);
    });
    act_lightTheme->change(tr("Light theme"));
    act_lightTheme->setCheckable(true);

    act_blackTheme = new KAction(nullptr, "black_theme");
    connect(act_blackTheme, &KAction::triggered, [this](){
        g_user_signal->setTheme(2);
        g_settings->setValue("General/follow_system_theme", 0);
    });
    act_blackTheme->change(tr("Black theme"));
    act_blackTheme->setCheckable(true);

    group_themeGroup->addAction(act_followSystem);
    group_themeGroup->addAction(act_lightTheme);
    group_themeGroup->addAction(act_blackTheme);

    menu_theme->addAction(act_followSystem);
    menu_theme->addAction(act_lightTheme);
    menu_theme->addAction(act_blackTheme);

    if(g_settings->value("General/follow_system_theme").toBool())
        act_followSystem->setChecked(true);
    else if(g_settings->value("General/theme").toInt() == 0)
        act_lightTheme->setChecked(true);
    else
        act_blackTheme->setChecked(true);

    // 默认先设置为跟随系统，当前先不做手动切换主题功能
    act_followSystem->setChecked(true);
    g_settings->setValue("General/follow_system_theme", true);
}

/** *******************************************
* 初始化隐私菜单
***********************************************/
void TitleMenu::createPrivacyMenu()
{
    menu_privacy = new KMenu;
    menu_privacy->menuAction()->setText(tr("Privacy"));

    act_clearMark = new KAction(nullptr, "clear_mark");
    connect(act_clearMark, &KAction::triggered, [this](){
        QMessageBox::StandardButton standar_button;
        QWidget* parent_widget = nullptr;
        if (isWayland)
            parent_widget = parentWidget();

        QMessageBox box(QMessageBox::Question,
                        tr("Video Player"),
                        tr("Are you sure you want to clear the list?"),
                        QMessageBox::Yes | QMessageBox::No,
                        parent_widget,
                        Qt::Dialog);

        if (isWayland)
            kdk::LingmoStyleHelper::self()->removeHeader(&box);
        box.show();
        standar_button = (QMessageBox::StandardButton)box.exec();
        if (standar_button == QMessageBox::Yes) {
            if(g_playstate > 0) {
                QMessageBox box1(QMessageBox::Question,
                                tr("Video Player"),
                                tr("The file being played will be stopped."),
                                QMessageBox::Yes | QMessageBox::No,
                                parent_widget);

                if (isWayland)
                    kdk::LingmoStyleHelper::self()->removeHeader(&box1);
                box1.show();
                standar_button = (QMessageBox::StandardButton)box1.exec();
            }
            if (standar_button == QMessageBox::Yes) {
                g_user_signal->clearPlayList();
            }
        }
    });
    act_clearMark->change(tr("Clear mark"));

    act_noMarkPlay = new KAction(nullptr, "no_mark");
    connect(act_noMarkPlay, &KAction::triggered, [this](bool _checked){
        g_config->seamlessBrowsing.second = _checked;
        g_config->flushChange();

        // 告诉别人设置成无痕了
        g_user_signal->sigNoMarkMode(_checked);
    });
    act_noMarkPlay->change(tr("No mark"));
    act_noMarkPlay->setCheckable(true);
    act_noMarkPlay->setChecked(g_config->seamlessBrowsing.first);

    menu_privacy->addAction(act_clearMark);
    menu_privacy->addAction(act_noMarkPlay);
}

/** *******************************************
* 初始化帮助菜单
***********************************************/
void TitleMenu::createHelpMenu()
{
    menu_help = new KMenu();
    menu_help->menuAction()->setText(tr("Help"));

    act_checkUpdate = new KAction(nullptr, "check_update");
    connect(act_checkUpdate, &KAction::triggered, [this](){;});
    act_checkUpdate->setText(tr("Check update"));

    act_f1 = new KAction(nullptr, "help");
    connect(act_f1, &KAction::triggered, [this](){
        g_user_signal->openHelpDoc();
    });
    act_f1->change(tr("Help"));

    act_advice_feedback = new KAction(nullptr, "advice_feedback");
    connect(act_advice_feedback, &KAction::triggered, [this](){
        QThread::create([](){
            QDesktopServices::openUrl(QUrl(QString(AdvideFeedback).toLatin1()));;
        })->start();
    });
    act_advice_feedback->setText(tr("Advice and feedback"));

    act_officialWebsite = new KAction(nullptr, "official_website");
    connect(act_officialWebsite, &KAction::triggered, [this](){
        QDesktopServices::openUrl(QUrl(QString(OfficialWebsite).toLatin1()));
    });
    act_officialWebsite->setText(tr("Official website"));

    // 更新暂未实现
//    menu_help->addAct(act_checkUpdate);
    menu_help->addAct(act_f1);
//    menu_help->addAct(act_advice_feedback);
    menu_help->addAct(act_officialWebsite);
}

/** *******************************************
* 初始化设置菜单
***********************************************/
void TitleMenu::createSetupMenu()
{
    menu_setup = new KMenu();
    menu_setup->menuAction()->setText(tr("Setup"));

    act_systemSetup = new KAction(nullptr, "system_setup");
    connect(act_systemSetup, &KAction::triggered, [this](){g_user_signal->showSetup(0);});
    act_systemSetup->setText(tr("System setup"));

    act_playSetup = new KAction(nullptr, "play_setup");
    connect(act_playSetup, &KAction::triggered, [this](){g_user_signal->showSetup(1);});
    act_playSetup->setText(tr("Play setup"));

    act_screenshotSetup = new KAction(nullptr, "screenshot_setup");
    connect(act_screenshotSetup, &KAction::triggered, [this](){g_user_signal->showSetup(2);});
    act_screenshotSetup->setText(tr("Screenshot setup"));

    act_subtitleSetup = new KAction(nullptr, "subtitle_setup");
    connect(act_subtitleSetup, &KAction::triggered, [this](){g_user_signal->showSetup(3);});
    act_subtitleSetup->setText(tr("Subtitle setup"));

    act_audioSetup = new KAction(nullptr, "audio_setup");
    connect(act_audioSetup, &KAction::triggered, [this](){g_user_signal->showSetup(4);});
    act_audioSetup->setText(tr("Audio setup"));

    act_decoderSetup = new KAction(nullptr, "decoder_setup");
    connect(act_decoderSetup, &KAction::triggered, [this](){g_user_signal->showSetup(5);});
    act_decoderSetup->setText(tr("Decoder setup"));

    act_shortcutSetup = new KAction(nullptr, "shortcut_setup");
    connect(act_shortcutSetup, &KAction::triggered, [this](){g_user_signal->showSetup(6);});
    act_shortcutSetup->setText(tr("Shortcut setup"));

    menu_setup->addAct(act_systemSetup);
    menu_setup->addAct(act_playSetup);
    menu_setup->addAct(act_screenshotSetup);
    menu_setup->addAct(act_subtitleSetup);
    menu_setup->addAct(act_audioSetup);
    menu_setup->addAct(act_decoderSetup);
    menu_setup->addAct(act_shortcutSetup);
}

PlayListItemMenu::PlayListItemMenu(QWidget *parent) :
    KMenu(parent)
{
    createAction();
}

PlayListItemMenu::~PlayListItemMenu()
{
    delete act_sortByName;
    delete act_sortByType;
    delete menuSort;

    delete act_removeCurrent;
    delete act_removeInvalid;
    delete act_clearList;
    delete act_openFolder;
}

void PlayListItemMenu::createAction()
{
    act_removeCurrent = new KAction(nullptr, "remove_selected");
    act_removeCurrent->change(tr("Remove selected"));
    connect(act_removeCurrent, &KAction::triggered, [this](){emit sigRemoveSelect();});

    act_removeInvalid = new KAction(nullptr, "remove_invalid");
    act_removeInvalid->change(tr("Remove invalid"));
    connect(act_removeInvalid, &KAction::triggered, [this](){emit sigRemoveInvalid();});

    act_clearList = new KAction(nullptr, "clear_list");
    act_clearList->change(tr("Clear list"));
    connect(act_clearList, &KAction::triggered, [this](){emit sigClearList();});

    act_openFolder = new KAction(nullptr, "open_folder");
    act_openFolder->change(tr("Open folder"));
    connect(act_openFolder, &KAction::triggered, [this](){emit sigOpenFolder();});

    menuSort = new KMenu;
    menuSort->menuAction()->setText(tr("Sort"));

    act_sortByName = new KAction(nullptr, "sort_by_name");
    act_sortByName->change(tr("Sort by name"));

    act_sortByType = new KAction(nullptr, "sort_by_type");
    act_sortByType->change(tr("Sort by type"));

    menuSort->addAct(act_sortByName);
    menuSort->addAct(act_sortByType);

    addAct(act_removeCurrent);
    addAct(act_removeInvalid);
    addAct(act_clearList);
//    addMenu(menuSort);        // 排序功能没做呢
    addAct(act_openFolder);
}
