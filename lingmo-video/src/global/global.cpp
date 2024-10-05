#include "global.h"

#include <QSettings>
#include <QDBusReply>
#include <QGSettings>
#include <QApplication>
#include <QDBusInterface>
#include <QFile>
#include <QDebug>
#include "path.h"

#include <lingmosdk/lingmosdk-system/libkysysinfo.h>

QSettings       *Global::g_settings     = nullptr;
SqliteHandle    *Global::g_sqlite       = nullptr;
GlobalConfig    *Global::g_config       = nullptr;
QGSettings      *Global::g_gsettings    = nullptr;
ShortCutSetting *Global::g_shortcut     = nullptr;
QGSettings      *Global::g_gsettings_control_center = nullptr;
Mpv::PlayState  Global::g_playstate     = Mpv::Idle;
bool Global::isWayland = false;
bool Global::isTablet = false;
//KPlayControl *Global::k_playcontrol = KPlayControl::getInstanece();

using namespace Global;

void Global::initGlobal() {

    // g_settings
    QString filename = Paths::iniPath() + "/lingmo-video3.ini";
    QString dbname = Paths::iniPath() + "/lingmo-video3.db";

    g_settings = new QSettings(filename, QSettings::IniFormat);
    g_settings->setIniCodec("UTF-8");
    // 运行环境
    if(!g_settings->contains("General/display_env"))
    {
        if((QString(qgetenv("XDG_SESSION_TYPE")) == "wayland"))
            g_settings->setValue("General/display_env", "wayland");
        else
            g_settings->setValue("General/display_env", "x11");
    }
    isWayland   = QString(qgetenv("XDG_SESSION_TYPE")) == "wayland";

    g_gsettings = new QGSettings(ORG_LINGMO_STYLE);
    if (QGSettings::isSchemaInstalled(PERSONALISE_SHEME))
        g_gsettings_control_center = new QGSettings(PERSONALISE_SHEME);

    g_shortcut  = ShortCutSetting::getInstance(g_settings);
    g_sqlite    = SqliteHandle::getInstance(dbname);
    g_config    = GlobalConfig::getInstance();

    QDBusInterface dbus("com.lingmo.statusmanager.interface",
                        "/",
                        "com.lingmo.statusmanager.interface",
                        QDBusConnection::sessionBus(),
                        nullptr);
    QDBusReply<bool> tmp_val = dbus.call("get_current_tabletmode");
    isTablet = tmp_val.value();
}
