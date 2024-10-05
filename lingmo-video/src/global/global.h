#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <QString>
#include <QSettings>
#include <QGSettings>

#include "core/sqlitehandle.h"
#include "core/mpvtypes.h"
#include "globalconfig.h"
#include "shortcutsetting.h"

#include <windowmanager/windowmanager.h>
#include <lingmostylehelper/lingmostylehelper.h>

// Some global objects
#define MENUWIDTH 180
#define MENU_BAR_BTN_SIZE 30
#define CONTRAL_BAR_BTN_SIZE 16
#define THEME_BUTTON_SIZE 36
#define THEME_BUTTON_SIZE_TABLET 48

class QSettings;
class QGSettings;
class ShortCutSetting;

//using namespace Control;
#define LINGMO_USER_GUIDE_PATH       "/"
#define LINGMO_USER_GUIDE_SERVICE    "com.lingmoUserGuide.hotel"
#define LINGMO_USER_GUIDE_INTERFACE  "com.guide.hotel"

#define PERSONALISE_SHEME   "org.lingmo.control-center.personalise"
#define ORG_LINGMO_STYLE      "org.lingmo.style"
#define STYLE_LINGMO_DEFAULT  "lingmo-default"
#define STYLE_LINGMO_LIGHT    "lingmo-light"
#define STYLE_LINGMO_BLACK    "lingmo-dark"
#define ICO_DIR_DEFAULT     "ico_light"
#define ICO_DIR_DBLACK      "ico"
#define FOLLOW_SYS_THEME    true

namespace Global {

// 错误类型
enum KERROR_TYPE{
    NO_ERROR = 0
};

extern Mpv::PlayState   g_playstate;
extern QGSettings       *g_gsettings;
extern QGSettings       *g_gsettings_control_center;
//! Read and store application settings
extern QSettings        *g_settings;
extern SqliteHandle     *g_sqlite;
extern GlobalConfig     *g_config;
extern ShortCutSetting  *g_shortcut;
extern bool isWayland;
extern bool isTablet;

void initGlobal();
}

#endif
