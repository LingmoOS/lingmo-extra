#ifndef KMenu_H
#define KMenu_H

#include <QMenu>
#include "core/mpvtypes.h"

#define OfficialWebsite "https://www.lingmoos.cn"
#define AdvideFeedback  "mailto:product@lingmoos.cn"

class KAction;

class KMenu : public QMenu
{
    Q_OBJECT
public:
    KMenu(QWidget *parent = nullptr);
    void addAct(QAction* act);

private:
    void initStyle();

    void setBlackTheme();
    void setLightTheme();

};


/** *******************************************
* 播放列表右键菜单
***********************************************/
class PlayListItemMenu : public KMenu
{
    Q_OBJECT
public:
    explicit PlayListItemMenu(QWidget *parent = nullptr);
    ~PlayListItemMenu();

signals:
    void sigRemoveSelect();
    void sigRemoveInvalid();
    void sigClearList();
    void sigOpenFolder();

private:
    void createAction();

private:
    KMenu *menuSort;                // 排序
    KAction *act_sortByName;        // 按名称排序
    KAction *act_sortByType;        // 按类型排序

    KAction *act_removeCurrent;     // 移除选中项
    KAction *act_removeInvalid;     // 移除无效文件
    KAction *act_clearList;         // 清空播放列表
    KAction *act_openFolder;        // 打开所在文件夹
};

/** *******************************************
* 标题栏菜单
***********************************************/
class TitleMenu : public KMenu
{
    Q_OBJECT
public:
    explicit TitleMenu(QWidget *parent = nullptr);
    ~TitleMenu();

signals:
    void sigQuit();

private:
    void createOneLevelAction();
    void createThemeMenu();
    void createPrivacyMenu();
    void createHelpMenu();
    void createSetupMenu();

private:
    KAction *act_uploadToCloud;         // 上传云端
    KMenu   *menu_theme;                // 主题
    QActionGroup *group_themeGroup;     // 主题事件组
    KAction *act_followSystem;          // 跟随系统
    KAction *act_lightTheme;            // 浅色主题
    KAction *act_blackTheme;            // 深色主题

    KMenu   *menu_privacy;              // 隐私
    KAction *act_clearMark;             // 清除痕迹
    KAction *act_noMarkPlay;            // 开启无痕播放

    KMenu   *menu_help;                 // 帮助
    KAction *act_checkUpdate;           // 检查更新
    KAction *act_advice_feedback;       // 建议和反馈
    KAction *act_officialWebsite;       // 官网
    KAction *act_f1;                    // f1 用户手册

    KAction *act_about;                 // 关于
    
    KMenu   *menu_setup;                // 设置
    KAction *act_systemSetup;           // 系统设置
    KAction *act_playSetup;             // 播放设置
    KAction *act_screenshotSetup;       // 截图设置
    KAction *act_subtitleSetup;         // 字幕设置
    KAction *act_audioSetup;            // 声音设置
    KAction *act_decoderSetup;          // 解码器设置
    KAction *act_shortcutSetup;         // 快捷键设置

    KAction *act_quit;                  // 退出
};

#if 0
/** *******************************************
* 列表循环菜单
***********************************************/
class ListLoopMenu : public KMenu
{
    Q_OBJECT
public:
    explicit ListLoopMenu(QWidget *parent = nullptr);

private:
    KAction *act_oneLoop;       // 单曲循环
    KAction *act_sequence;      // 顺序播放
    KAction *act_listLoop;      // 列表循环
    KAction *act_random;        // 随机播放

    void setBlackTheme();
    void setLightTheme();

};
#endif

/** *******************************************
* 右键菜单
***********************************************/
class KRightClickMenu : public KMenu
{
    Q_OBJECT
public:

    explicit KRightClickMenu(QWidget *parent = nullptr);

    void createOpenMenu();
    void createSetTopAction();
    void createPlayOrderMenu();
    void createFrameMenu();
    void createAudioMenu();
    void createSubMenu();
    void createPlayMenu();
    void createPlayerSetAction();
    void createMediaInfoAction();

private:
    KMenu *menu_open;             // 打开菜单
    KAction *act_openFile;        // 打开文件
    KAction *act_openDirectory;   // 打开文件夹
    KAction *act_openURL;         // 打开网址

    KAction *act_setTop;          // 置顶

    KMenu *menu_playOrder;         // 播放顺序菜单
    QActionGroup *group_playOrder; // 播放顺序事件组
    KAction *act_oneLoop;          // 单曲循环
    KAction *act_sequence;         // 顺序播放
    KAction *act_listLoop;         // 列表循环
    KAction *act_random;           // 随机播放

    KMenu *menu_frame;             // 画面菜单

    QActionGroup *group_frameScale;// 画面比例事件组
    KAction *act_defaultFrame;     // 默认比例
    KAction *act_4div3Frame;       // 4:3
    KAction *act_16div9Frame;      // 16:9
    KAction *act_fullFrame;        // 满屏

    KAction *act_restoreFrame;     // 画面还原
    KAction *act_alongRotate;      // 顺时针旋转90度
    KAction *act_inverseRotate;    // 逆时针旋转90度
    KAction *act_flipHorizontally; // 水平翻转
    KAction *act_flipVertically;   // 垂直翻转
    KAction *act_showInfo;         // osd 显示 profile

    KMenu *menu_audio;             // 声音菜单
    KMenu *act_audioTrack;         // 音轨菜单
    QActionGroup *group_audio;     // 所有音轨
    KMenu *act_audioChannel;       // 声道菜单
    QActionGroup *group_channel;   // 声道事件组
    KAction *act_defaultChannel;   // 默认声道
    KAction *act_stereo;           // 立体声
    KAction *act_leftChannel;      // 左声道
    KAction *act_rightChannel;     // 右声道
    KAction *act_audioSet;         // 声音设置

    KMenu *menu_subtitle;          // 字幕菜单
    KAction *act_loadSubtitle;     // 载入字幕
    KMenu *menu_subtitleSelect;    // 字幕选择菜单
    QActionGroup *group_sub;       // 所有字幕
    KAction *act_noSubtitle;       // 无字幕
    QVector<KAction*> vec_subList; // 所有字幕，动态添加
    KAction *act_subtitleSet;      // 字幕设置
    KAction *act_matchSubtitle;    // 匹配字幕
    KAction *act_searchSubtitle;   // 搜索字幕

    KMenu *menu_play;              // 播放菜单
    KAction *act_playPause;        // 播放/暂停
    KAction *act_volumeUp;         // 音量+
    KAction *act_volumeDown;       // 音量-
    KAction *act_playForward;      // 快进
    KAction *act_playBackward;     // 快退

    KAction *act_playerSet;        // 播放器设置
    KAction *act_MediaInfo;        // 影片信息

private:
    bool isShowInfo = false;

signals:
    void sigOpenFile();
    void sigOpenDir();
    void sigOpenUrl();
    void sigToTop(bool);
    void sigPlayOrder(int);
    void sigOneLoop();
    void sigListLoop();
    void sigSequence();
    void sigRandom();
    void sigDefaultFrame();
    void sig4Div3Frame();
    void sig16Div9Frame();
    void sigFullFrame();
    void sigRestoreFrame();
    void sigAlongRotate();
    void sigInverseRotate();
    void sigFlipHorizontally();
    void sigFlipVertically();
    void sigAudioTrack(int);
    void sigDefalutChannel();
    void sigStereo();
    void sigLeftChannel();
    void sigRightChannel();
    void sigAudioSet();
    void sigLoadSubtitle();
    void sigNoSubtitle();
    void sigSubtitleSet();
    void sigMatchSubtitle();
    void sigSearchSubtitle();
    void sigPlayPause();
    void sigVolumeUp();
    void sigVolumeDown();
    void sigPlayForward();
    void sigPlayBackward();
    void sigPlayerSet();
    void sigMediaInfo();

private:
    void initGlobalSig();

private slots:
    void addSubtitle(QString name, int id);
    void addAudioTrack(QString name, int id);
    void loadTracks(QList<Mpv::Track> tracks);
    void reInit();
    void enableActions();
    void disableVideoMenu();
    void enableVideoMenu();
};

#endif // KMenu_H
