#include "shortcutsetting.h"
#include <QDebug>
#include <QAction>
#include "global.h"

using namespace Global;

ShortCutSetting* ShortCutSetting::instance = new ShortCutSetting;
QSettings* ShortCutSetting::m_settings = nullptr;

QString restoreKeyString(QString key){
    key.replace(" + ", "+");
    key.replace("Enter", "Return");
    if(key.indexOf("空格") != -1)
        key.replace("空格", "Space");
    return key;
}

ShortCutSetting::ShortCutSetting(QObject *parent) : QObject(parent)
{
    initShortCutFunc();
}

ShortCutSetting *ShortCutSetting::getInstance(QSettings *sets)
{
    m_settings = sets;
    return instance;
}

ShortCutSetting::~ShortCutSetting()
{

}

void ShortCutSetting::initData()
{
    if(parent() == nullptr)
    {
        qDebug() << "please set parent!";
        return;
    }
    // 初始化的时候读配置文件获取功能对应快捷键
    m_shortcut_map[m_helpDoc]   = newShortCut("help documentation", tr("help documentation"), "F1");
    m_shortcut_map[m_exit]      = newShortCut("exit", tr("exit"), "Ctrl+Q");
    // 文件
    m_shortcut_map[m_open_file] = newShortCut("open file",  tr("open file"),    "Ctrl+O");
    m_shortcut_map[m_open_dir]  = newShortCut("open dir",   tr("open dir"),     "Ctrl+F");
    m_shortcut_map[m_open_url]  = newShortCut("open url",   tr("open url"),     "Ctrl+U");
    m_shortcut_map[m_prev_file] = newShortCut("prev file",  tr("prev file"),    "Page Up");
    m_shortcut_map[m_next_file] = newShortCut("next file",  tr("next file"),    "Page Down");

    // 播放
    m_shortcut_map[m_play_pause]        = newShortCut("play/pause",         tr("play/pause"),       "Space");
    m_shortcut_map[m_speed_up]          = newShortCut("speed up",           tr("speed up"),         "Ctrl+Up");
    m_shortcut_map[m_speed_down]        = newShortCut("speed down",         tr("speed down"),       "Ctrl+Down");
    m_shortcut_map[m_speed_normal]      = newShortCut("speed normal",       tr("speed normal"),     "R");
    m_shortcut_map[m_forword]           = newShortCut("forword",            tr("forword"),          "Right");
    m_shortcut_map[m_backword]          = newShortCut("backword",           tr("backword"),         "Left");
    m_shortcut_map[m_forward_30]        = newShortCut("forward 30s",        tr("forward 30s"),      "Ctrl+Right");
    m_shortcut_map[m_backword_30]       = newShortCut("backword 30s",       tr("backword 30s"),     "Ctrl+Left");
    m_shortcut_map[m_insert_bookmark]   = newShortCut("insert bookmark",    tr("insert bookmark"),  "B");
    m_shortcut_map[m_ib_notes]          = newShortCut("ib notes",           tr("ib notes"),         "Alt+B");

    // 图像
    m_shortcut_map[m_fullscreen]        = newShortCut("fullscreen",         tr("fullscreen"),       "Return");
    m_shortcut_map[m_mini_mode]         = newShortCut("mini mode",          tr("mini mode"),        "Shift+Return");
    m_shortcut_map[m_to_top]            = newShortCut("to top",             tr("to top"),           "T");
    m_shortcut_map[m_screenshot]        = newShortCut("screenshot",         tr("screenshot"),       "Alt+A");
    m_shortcut_map[m_cut]               = newShortCut("cut",                tr("cut"),              "Alt+S");
    m_shortcut_map[m_light_up]          = newShortCut("light up",           tr("light up"),         "=");
    m_shortcut_map[m_light_down]        = newShortCut("light down",         tr("light down"),       "-");
    m_shortcut_map[m_forward_rotate]    = newShortCut("forward rotate",     tr("forward rotate"),   "E");
    m_shortcut_map[m_backward_rotate]   = newShortCut("backward rotate",    tr("backward rotate"),  "F");
    m_shortcut_map[m_horizontal_flip]   = newShortCut("horizontal flip",    tr("horizontal flip"),  "Alt+W");
    m_shortcut_map[m_vertical_flip]     = newShortCut("vertical flip",      tr("vertical flip"),    "Alt+Q");
    m_shortcut_map[m_image_boost]       = newShortCut("image boost",        tr("image boost"),      "A");

    // 声音
    m_shortcut_map[m_volume_up]         = newShortCut("volume up",          tr("volume up"),        "Up");
    m_shortcut_map[m_volume_down]       = newShortCut("volume down",        tr("volume down"),      "Down");
    m_shortcut_map[m_mute]              = newShortCut("mute",               tr("mute"),             "M");
    m_shortcut_map[m_audio_next]        = newShortCut("audio next",         tr("audio next"),       "S");
    m_shortcut_map[m_default_channel]   = newShortCut("default channel",    tr("default channel"),  "/");
    m_shortcut_map[m_left_channel]      = newShortCut("left channel",       tr("left channel"),     ",");
    m_shortcut_map[m_right_channel]     = newShortCut("right channel",      tr("right channel"),    ".");

    // 字幕
    m_shortcut_map[m_sub_load]      = newShortCut("sub load",       tr("sub load"),     "Alt+0");
    if(isWayland)
    {
        m_shortcut_map[m_sub_earlier]   = newShortCut("sub earlier",    tr("sub earlier"),  "Shift+{");
        m_shortcut_map[m_sub_later]     = newShortCut("sub later",      tr("sub later"),    "Shift+}");
    }
    else
    {
        m_shortcut_map[m_sub_earlier]   = newShortCut("sub earlier",    tr("sub earlier"),  "Shift+[");
        m_shortcut_map[m_sub_later]     = newShortCut("sub later",      tr("sub later"),    "Shift+]");
    }
    m_shortcut_map[m_sub_up]        = newShortCut("sub up",         tr("sub up"),       "Ctrl+[");
    m_shortcut_map[m_sub_down]      = newShortCut("sub down",       tr("sub down"),     "Ctrl+]");
    m_shortcut_map[m_sub_next]      = newShortCut("sub next",       tr("sub next"),     "C");

    // 其他
    m_shortcut_map[m_play_list] = newShortCut("play list",  tr("play list"),    "F3");
    m_shortcut_map[m_setup]     = newShortCut("setup",      tr("setup"),        "F4");

    for(std::pair<void(*)(), QShortcut*> p : m_shortcut_map){
        if(p.second != nullptr)
            connect(p.second, &QShortcut::activated, p.first);
    }
}

/** **********************************************
 * 使快捷键生效
 * 说明 : 对应失效
 *************************************************/
void ShortCutSetting::makeAllValid()
{
    for(std::pair<void(*)(), QShortcut*> pair : m_shortcut_map)
    {
        if(pair.second == nullptr)
            continue;
        pair.second->setEnabled(true);
    }
}

/** **********************************************
 * 使快捷键失效
 * 说明 : wayland 环境下打开设置界面输入快捷键的时候会触发
 *       快捷键功能，从而导致没有办法获取输入的快捷键。
 *************************************************/
void ShortCutSetting::makeAllInvalid()
{
    for(std::pair<void(*)(), QShortcut*> pair : m_shortcut_map)
    {
        if(pair.second == nullptr)
            continue;
        pair.second->setEnabled(false);
    }
}

QString ShortCutSetting::resetShort(QString name, QString s)
{
    for(std::pair<void(*)(), QShortcut*> pair : m_shortcut_map)
    {
        // 防止快捷键 new 失败导致崩溃
        if(pair.second == nullptr)
            continue;
        if(pair.second->objectName() == name)
        {
            // 修改快捷键，写入配置文件
            QString rk = restoreKeyString(s);
            if(m_name_map.find(name) != m_name_map.end())
            {
                // 先获取老的 key 再设置新的 key
                QString last_key = m_name_map[name]->key().toString();

                m_settings->setValue("action/"+pair.second->property("setkey").toString(), rk);
                pair.second->setKey(QKeySequence(rk));

                m_name_map[name]->setKey(QKeySequence(rk));
                if(m_action_map.find(name) != m_action_map.end())
                    m_action_map[name]->setShortcut(QKeySequence(rk));
                return last_key;
            }
        }
    }
    return QString();
}

void ShortCutSetting::registerAction(QString name, QAction *act)
{
    m_action_map[name] = act;
}

void ShortCutSetting::initShortCutFunc()
{
    // 所有快捷键事件，后面可以改为QAction注册，目前是因为很多快捷键设计没有具体事件
    m_exit = [](){g_user_signal->exitApp();};
    m_helpDoc = [](){g_user_signal->openHelpDoc();};
    // 文件
    m_open_file = [](){g_user_signal->selectFile();};
    m_open_dir  = [](){g_user_signal->selectDir();};
    m_open_url  = [](){g_user_signal->openUrl();};
    m_prev_file = [](){g_user_signal->playPrev(true);};
    m_next_file = [](){g_user_signal->playNext(true);};

    // 播放
    m_play_pause        = [](){g_user_signal->play_pause();};
    m_speed_up          = [](){g_user_signal->setSpeedUp();};
    m_speed_down        = [](){g_user_signal->setSpeedDown();};
    m_speed_normal      = [](){g_user_signal->setSpeed(1.0);};
    m_forword           = [](){g_user_signal->forword(true);};
    m_backword          = [](){g_user_signal->backword(true);};
    m_forward_30        = [](){g_user_signal->forword(false);};
    m_backword_30       = [](){g_user_signal->backword(false);};
    m_insert_bookmark   = [](){if (!g_config->seamlessBrowsing.first) g_user_signal->addBookMark(" ");};
    m_ib_notes          = [](){;};

    // 图像
    m_fullscreen        = [](){g_user_signal->fullScreen();};
    if (!Global::isTablet)
        m_mini_mode     = [](){g_user_signal->changeShowMode();};
    m_screenshot        = [](){g_user_signal->screenShot(false);};
    m_cut               = [](){;};
    m_light_up          = [](){g_user_signal->brightnessUp();};
    m_light_down        = [](){g_user_signal->brightnessDown();};
    m_to_top            = [](){g_user_signal->setToTop();};
    m_forward_rotate    = [](){g_user_signal->clockwiseRotate();};
    m_backward_rotate   = [](){g_user_signal->counterClockwiseRotate();};
    m_horizontal_flip   = [](){g_user_signal->horizontallyFlip();};
    m_vertical_flip     = [](){g_user_signal->verticalFlip();};
    m_image_boost       = [](){;};

    // 声音
    m_volume_up         = [](){g_user_signal->setVolumeUp(10);};
    m_volume_down       = [](){g_user_signal->setVolumeDown(10);};
    if (!Global::isTablet)
        m_mute          = [](){g_user_signal->setMute();};
    m_audio_next        = [](){g_user_signal->setAudioNext();};
    m_default_channel   = [](){g_user_signal->setChannel(Mpv::Stereo);};
    m_left_channel      = [](){g_user_signal->setChannel(Mpv::Left);};
    m_right_channel     = [](){g_user_signal->setChannel(Mpv::Right);};

    // 字幕
    m_sub_load      = [](){g_user_signal->selectSub();};
    m_sub_earlier   = [](){g_user_signal->setSubForward();};
    m_sub_later     = [](){g_user_signal->setSubBackward();};
    m_sub_up        = [](){g_user_signal->setSubUp();};
    m_sub_down      = [](){g_user_signal->setSubDown();};
    m_sub_next      = [](){g_user_signal->setSubNext();};

    m_play_list     = [](){g_user_signal->showPlayList();};
    m_setup         = [](){g_user_signal->showSetup(0);};
}
