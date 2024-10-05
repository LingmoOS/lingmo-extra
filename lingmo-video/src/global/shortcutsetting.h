#ifndef SHORTCUTSETTING_H
#define SHORTCUTSETTING_H

#include <QObject>
#include <QShortcut>
#include <QFunctionPointer>
#include "globalsignal.h"

class ShortCutSetting : public QObject
{
    Q_OBJECT
public:
    static ShortCutSetting *getInstance(QSettings *sets);
    ~ShortCutSetting();

    void initData();
    void makeAllValid();
    void makeAllInvalid();
    void registerAction(QString name, QAction *act);
    QString resetShort(QString name, QString s);

    std::map<void(*)(), QShortcut*> get_short_map(){return m_shortcut_map;}
signals:

private:
    /** **************************************************************
     * @param set_key:      配置文件中快捷键的key
     * @param desc:         快捷键描述，为翻译语言
     *                     （设置为objectname，之后可以通过objectname来设置界面）
     * @param default_short:快捷键组合
     *****************************************************************/
    inline QShortcut * newShortCut(QString set_key, QString desc, QString default_short){
        QShortcut *ret = nullptr;
        if(m_settings == nullptr)
            return nullptr;
        QString shortCutDesc = m_settings->value("action/"+set_key).toString();
        if(shortCutDesc.length() > 0)
        {
            ret = new QShortcut(QKeySequence(shortCutDesc), (QWidget*)parent(), nullptr, nullptr, Qt::ApplicationShortcut);
            ret->setProperty("setkey", set_key);
            ret->setObjectName(desc);
            m_name_map[desc] = ret;
            return ret;
        }
        if(m_settings->contains("action/"+set_key))
            default_short = "";
        ret = new QShortcut(QKeySequence(default_short), (QWidget*)parent(), nullptr, nullptr, Qt::ApplicationShortcut);
        m_settings->setValue("action/"+set_key, default_short);
        ret->setProperty("setkey", set_key);
        ret->setObjectName(desc);
        m_name_map[desc] = ret;
        return ret;
    }


private:
    static ShortCutSetting* instance;
    static QSettings *m_settings;
    explicit ShortCutSetting(QObject *parent = nullptr);
    void initShortCutFunc();

    std::map<void(*)(), QShortcut*> m_shortcut_map;
    std::map<QString, QAction*> m_action_map;
    std::map<QString, QShortcut*> m_name_map;

    // 所有快捷键事件，快捷键主要和右键菜单联系，QFunctionPointer - QShortCut - 右键菜单QAction
    QFunctionPointer m_exit;            // 关闭
    QFunctionPointer m_helpDoc;         // 打开帮助文档
    // 文件
    QFunctionPointer m_open_file;       // 打开文件  默认: Ctrl+O
    QFunctionPointer m_open_dir;        // 打开文件夹  默认: Ctrl+F
    QFunctionPointer m_open_url;        // 打开url  默认: Ctrl+U
    QFunctionPointer m_prev_file;       // 上一个文件  默认: Page Up
    QFunctionPointer m_next_file;       // 下一个文件  默认: Page Down

    // 播放
    QFunctionPointer m_play_pause;      // 播放/暂停  默认：Space
    QFunctionPointer m_speed_up;        // 加速播放  默认：Ctrl+Up
    QFunctionPointer m_speed_down;      // 减速播放  默认：Ctrl+Down
    QFunctionPointer m_speed_normal;    // 正常速度播放  默认：R
    QFunctionPointer m_forword;         // 快进  默认：Right
    QFunctionPointer m_backword;        // 快退  默认：Left
    QFunctionPointer m_forward_30;      // 30秒快进  默认：Ctrl+Right
    QFunctionPointer m_backword_30;     // 30秒快退  默认：Ctrl+Left
    QFunctionPointer m_insert_bookmark; // 插入书签  默认：B
    QFunctionPointer m_ib_notes;        // 插入与注释书签  默认：Alt+B

    // 图像
    QFunctionPointer m_fullscreen;      // 全屏  默认：Enter
    QFunctionPointer m_mini_mode;       // 迷你模式  默认：Shift+Enter
    QFunctionPointer m_to_top;          // 置顶  默认：T
    QFunctionPointer m_screenshot;      // 截图  默认：Alt+A
    QFunctionPointer m_cut;             // 截取  默认：Alt+S
    QFunctionPointer m_light_up;        // 增加亮度  默认 =
    QFunctionPointer m_light_down;      // 减小亮度  默认 -
    QFunctionPointer m_forward_rotate;  // 顺时针旋转90°  默认：E
    QFunctionPointer m_backward_rotate; // 逆时针旋转90°  默认：F
    QFunctionPointer m_horizontal_flip; // 水平翻转  默认：Ctrl+F
    QFunctionPointer m_vertical_flip;   // 垂直翻转  默认：Q
    QFunctionPointer m_image_boost;     // 画质增强  默认：A

    // 声音
    QFunctionPointer m_volume_up;       // 升高音量  默认：Up
    QFunctionPointer m_volume_down;     // 降低音量  默认：Down
    QFunctionPointer m_mute;            // 静音  默认：M
    QFunctionPointer m_audio_next;      // 切换音轨  默认：S
    QFunctionPointer m_default_channel; // 默认声道  默认：/
    QFunctionPointer m_left_channel;    // 左声道  默认：,
    QFunctionPointer m_right_channel;   // 右声道  默认：.

    // 字幕
    QFunctionPointer m_sub_load;        // 手动加载字幕  默认：Alt+0
    QFunctionPointer m_sub_earlier;     // 字幕提前0.5秒  默认：Shift+[
    QFunctionPointer m_sub_later;       // 字幕推迟0.5秒  默认：Shift+]
    QFunctionPointer m_sub_up;          // 字幕上移  默认：Ctrl+[
    QFunctionPointer m_sub_down;        // 字幕下移  默认：Ctrl+]
    QFunctionPointer m_sub_next;        // 字幕切换  默认：C

    // 其他
    QFunctionPointer m_play_list;       // 播放列表  默认：F3
    QFunctionPointer m_setup;           // 播放器设置  默认：F4

};

#endif // SHORTCUTSETTING_H
