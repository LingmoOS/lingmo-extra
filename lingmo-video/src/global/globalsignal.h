#ifndef GLOBALSIGNAL_H
#define GLOBALSIGNAL_H

#include <QFileDialog>
#include <QObject>
#include "core/mpvtypes.h"
#include "extensions.h"
#include "global.h"

//using namespace Global;

enum PlayOrder{
    ONE_LOOP,   /*单曲循环*/
    LIST_LOOP,  /*列表循环*/
    RANDOM,     /*随即播放*/
    SEQUENCE    /*顺序播放*/
};

/** ******************************
 * 用户全局信号，主要用来发送主动信号
 *********************************/
class GlobalUserSignal : public QObject
{
    Q_OBJECT
public:
    static GlobalUserSignal* getInstance();

    void showStopFrame()                        {emit sigShowStopFrame();}
    void openHelpDoc()                          {emit sigOpenHelpDoc();}
    void exitApp()                              {emit sigExit();}
    void setTheme(int theme)                    {emit sigTheme(theme);}
    void showRightMenu()                        {emit sigRightMenuShow();}
    void playWidgetClick()                      {emit sigPlayWidgetClicked();}
    void changeShowMode()                       {emit sigChangeShowMode();}
    // 控制栏 右键菜单
    void open(QString file, int pos)            {emit sigOpen(file, pos);}
    void stop()                                 {emit sigStop();}
    void play()                                 {emit sigPlay();}
    void pause()                                {emit sigPause();}
    void play_pause()                           {emit sigPlayPause();}
    void nFrame()                               {emit sigNFrame();}
    void pFrame()                               {emit sigPFrame();}
    void progressUp(int sec)                    {emit sigProgressUp(sec);}
    void progressDown(int sec)                  {emit sigProgressDown(sec);}
    void forword(bool b)                        {emit sigForword(b);}
    void backword(bool b)                       {emit sigBackword(b);}
    void seek(int seek_time)                    {emit sigSeek(seek_time);}
    void restart()                              {emit sigRestart();}
    // 右键菜单
    void selectFile()                           {emit sigSelectFile();}
    void selectDir()                            {emit sigSelectDir();}
    void selectSub()                            {emit sigSelectSub();}
    void openUrl()                              {emit sigOpenUrl();}
    void addSub(QString file)                   {emit sigAddSub(file);}
    void setSubId(int id)                       {emit sigSubId(id);}
    void setSubNext()                           {emit sigSubNext();}
    void setSubUp()                             {emit sigSubUp();}
    void setSubDown()                           {emit sigSubDown();}
    void setSubForward()                        {emit sigSubForward();}
    void setSubBackward()                       {emit sigSubBackward();}
    // 设置界面
    void setSubSize(int size)                   {emit sigSubSize(size);}
    void setSubFont(QString family)             {emit sigSubFont(family);}
    void setChannel(Mpv::Channel cl)            {emit sigChannel(cl);}
    void setSpeed(double s)                     {emit sigSpeed(s);}
    void setSpeedUp()                           {emit sigSpeedUp();}
    void setSpeedDown()                         {emit sigSpeedDown();}
    void setVolume(int v)                       {emit sigVolume(v);}
    void setVolumeUp(int up)                    {emit sigVolumeUp(up);}
    void setVolumeDown(int down)                {emit sigVolumeDown(down);}
    void setMute(bool mute)                     {emit sigMute(mute);}
    void setMute()                              {emit sigMute();}
    void setAudioId(int id)                     {emit sigAudioId(id);}
    void setAudioNext()                         {emit sigAudioNext();}
    void setToTop()                             {emit sigToTop();}
    void setAspect(Mpv::VideoAspect va)         {emit sigAspect(va);}
    void restoreFrame()                         {emit sigRestoreFrame();}
    void horizontallyFlip()                     {emit sigHorizontallyFlip();}
    void verticalFlip()                         {emit sigVerticalFlip();}
    void clockwiseRotate()                      {emit sigClockwiseRotate();}
    void counterClockwiseRotate()               {emit sigCounterClockwiseRotate();}
    void brightnessUp()                         {emit sigBrightnessUp();}
    void brightnessDown()                       {emit sigBrightnessDown();}
    void fullScreen()                           {emit sigFullScreen();}
    void addDir(QString dir)                    {emit sigAddDir(dir);}
    void addFiles(QStringList files)            {emit sigAddFiles(files);}
    void playNext(bool manual)                  {emit sigPlayNext(manual);}
    void playPrev(bool manual)                  {emit sigPlayPrev(manual);}
    void setPlayOrder(PlayOrder order)          {emit sigPlayOrder(order);}
    void changePlayOrder()                      {emit sigChangePlayOrder();}
    void addListItem(QString file, int duration){emit sigListItemChange(file, duration, true);}
    void deleteListItem(QString file)           {emit sigListItemChange(file, 0, false);}
    void showSetup(int index)                   {emit sigShowSetup(index);}
    void showAbout()                            {emit sigShowAbout();}
    void showPlayList()                         {emit sigShowPlayList();}
    void addBookMark(QString desc)              {emit sigAddBookMark(desc);}
    void screenShot(bool withSub)               {emit sigScreenShot(withSub);}
    void screenShotDir(QString dir)             {emit sigScreenShotDir(dir);}
    void screenShotFormat(QString format)       {emit sigScreenShotFormat(format);}
    void hideBar(bool hide)                     {emit sigHideBar(hide);}
    void clearPlayList()                        {emit sigClearPlayList();}
    void showInfo(bool isShow)                  {emit sigShowInfo(isShow);}
    void setNoMarkMode(bool noMark)             {emit sigNoMarkMode(noMark);}
    void setVideoDecoder(QString decoder)       {emit sigVideoDecoder(decoder);}
    void setVideoOutput(QString vo)             {emit sigVideoOutput(vo);}
    void setVideoDecodeThreads(int threads)     {emit sigVideoDecodeThread(threads);}
    void setAudioDecoder(QString decoder)       {emit sigVideoDecoder(decoder);}

signals:
    void sigShowStopFrame();            // 显示停止后的默认界面
    void sigOpenHelpDoc();              // 打开帮助文档
    void sigExit();                     // 退出
    void sigChangeShowMode();           // 切换显示模式（ mini，normal ）
    void sigPlayWidgetClicked();        // 播放界面被单击
    void sigSelectFile();               // 选择一个文件
    void sigSelectDir();                // 选择一个目录
    void sigOpenUrl();                  // 打开 Url
    void sigTheme(int);                 // 设置主题 (0:跟随系统/1:亮色主题/2:暗色主题)
    void sigRightMenuShow();            // 显示右键菜单
    void sigOpen(QString, int);         // 打开文件
    void sigStop();                     // 停止播放
    void sigPlay();                     // 播放
    void sigPlayPause();                // 播放暂停 给不知道播放状态的人用
    void sigSeek(int);                  // 跳转
    void sigRestart();                  // 重新播放
    void sigProgressUp(int);            // 进度加
    void sigProgressDown(int);          // 进度减
    void sigBackword(bool);             // 快退 true:10秒 false:30秒
    void sigForword(bool);              // 快进 true:10秒 false:30秒
    void sigPFrame();                   // 上一帧
    void sigNFrame();                   // 下一帧
    void sigPause();                    // 暂停
    void sigSpeed(double);              // 速度
    void sigSpeedUp();                  // 加速播放
    void sigSpeedDown();                // 减速播放
    void sigVolume(int);                // 声音
    void sigVolumeUp(int);              // 声音增加
    void sigVolumeDown(int);            // 声音减小
    void sigMute(bool);                 // 设置静音
    void sigMute();                     // 告诉控制栏让它去设置静音
    void sigAudioId(int);               // 设置音轨
    void sigAspect(Mpv::VideoAspect);   // 画面比例
    void sigRestoreFrame();             // 画面还原
    void sigHorizontallyFlip();         // 水平翻转
    void sigVerticalFlip();             // 垂直翻转
    void sigClockwiseRotate();          // 顺时针旋转90
    void sigCounterClockwiseRotate();   // 逆时针旋转90
    void sigBrightnessUp();             // 亮度增加
    void sigBrightnessDown();           // 亮度减小
    void sigToTop();                    // 置顶
    void sigAddDir(QString);            // 添加文件夹
    void sigAddFiles(QStringList);      // 添加多个文件
    void sigSelectSub();                // 选择字幕
    void sigAddSub(QString);            // 加载字幕
    void sigSubSize(int size);          // 字幕字体大小
    void sigSubFont(QString family);    // 字幕字体
    void sigSubId(int);                 // 修改字幕id
    void sigSubNext();                  // 切换到下一个字幕
    void sigSubUp();                    // 字幕上移
    void sigSubDown();                  // 字幕下移
    void sigSubForward();               // 字幕快进0.5秒
    void sigSubBackward();              // 字幕推迟0.5秒
    void sigChannel(Mpv::Channel);      // 设置声道
    void sigAudioNext();                // 切换音轨
    void sigPlayNext(bool);             // 播放下一个（自动播放下一个也发送这个信号）
    void sigPlayPrev(bool);             // 播放上一个
    void sigPlayOrder(PlayOrder);       // 设置播放顺序
    void sigChangePlayOrder();          // 切换播放顺序
    void sigShowSetup(int);             // 弹出设置界面
    void sigShowAbout();                // 弹出设置界面
    void sigShowPlayList();             // 显示播放列表
    void sigFullScreen();               // 全屏播放(true：全屏  false：非全屏)
    void sigAddBookMark(QString desc);  // 添加书签
    void sigScreenShot(bool);           // 截图
    void sigScreenShotDir(QString);     // 截图保存路径
    void sigScreenShotFormat(QString);  // 截图保存格式
    void sigHideBar(bool);              // 隐藏控制栏(标题栏) true:隐藏 false:显示
    void sigShowInfo(bool);             // osd显示profile true:显示 false:隐藏
    void sigClearPlayList();            // 清空播放列表
    void sigVideoDecoder(QString);      // 切换视频解码器
    void sigVideoOutput(QString);       // 切换视频输出驱动
    void sigVideoDecodeThread(int);     // 解码线程修改
    void sigNoMarkMode(bool);           // 设置无痕模式
    void sigListItemChange(QString, int, bool); // 设置播放列表

private:
    explicit GlobalUserSignal(QObject *parent = nullptr);
    static GlobalUserSignal* instance;
};

/** ******************************
 * mpv全局信号，主要用来发送被动信号
 *********************************/
class GlobalCoreSignal : public QObject
{
    Q_OBJECT
public:
    static GlobalCoreSignal* getInstance();

    void notifyMute(bool m)                     {emit sigMuteChange(m);}
    void notifySubId(int id)                    {emit sigSubIdChange(id);}
    void notifySpeed(double s)                  {emit sigSpeedChange(s);}
    void notifyTracks(QList<Mpv::Track> tracks) {emit sigTracksChange(tracks);}
    void notifyVolume(int v)                    {emit sigVolumeChange(v);}
    void notifyAudioId(int id)                  {emit sigAudioIdChange(id);}
    void notifyVideoId(int id)                  {emit sigVideoIdChange(id);}
    void notifyFileInfo(Mpv::FileInfo fi)       {emit sigFileInfoChange(fi);}
    void notifyPlayState()                      {emit sigStateChange();}
    void notifyCurrentTime(double time)         {emit sigCurrentTime(time);}
    void notifyFileNotExist(QString file)       {emit sigFileNotExist(file);}
    void notify10FrameUseTime(qint64 use_time)  {emit sig10FrameUseTime(use_time);}
    void notifyFileLoadedError(QString file)    {emit sigFileLoadedError(file);}
    void notifyDuration(QString file, double duration)
    {emit sigDuration(file, duration);}
    void notifyMarkAdded(QString file, int pos, QString desc, QString view)
    {emit sigMarkAdded(file, pos, desc, view);}

signals:
    void sigFileLoadedError(QString);
    void sigFileNotExist(QString);
    void sigFileInfoChange(Mpv::FileInfo);
    void sigStateChange();
    void sigCurrentTime(double);
    void sigDuration(QString, double);
    void sigVolumeChange(int);
    void sigSpeedChange(double);
    void sigMuteChange(bool);
    void sigTracksChange(QList<Mpv::Track>);
    void sigSubIdChange(int);
    void sigVideoIdChange(int);
    void sigAudioIdChange(int);
    void sig10FrameUseTime(qint64);
    void sigMarkAdded(QString, int, QString, QString);

private:
    explicit GlobalCoreSignal(QObject *parent = nullptr);
    static GlobalCoreSignal* instance;
};

#define g_user_signal GlobalUserSignal::getInstance()
#define g_core_signal GlobalCoreSignal::getInstance()

#endif // GLOBALSIGNAL_H
