#include "path.h"

#include <QLibraryInfo>
#include <QLocale>
#include <QRegExp>
#include <QFile>
#include <QDir>

#include <lingmo-log4qt.h>

QString Paths::m_app_path = "";
QString Paths::m_log_path = "";
QString Paths::m_config_path = "";

void Paths::setAppPath(QString path) {
    m_app_path = path;
}

QString Paths::previewPath()
{
    QString fulldir = configPath() + "/preview";
    QDir dir(fulldir);
    if (!dir.exists()) {
        dir.mkdir(fulldir);
    }
    return fulldir;
}

QString Paths::appPath() {
    return m_app_path;
}

QString Paths::translationPath()
{
    QString t_path;
    if (QDir("/usr/share/lingmo-video/translations").exists()) {
        t_path = "/usr/share/lingmo-video/translations";
        return t_path;
    }
    else {
        return appPath() + "/translations";
    }
}

QString Paths::shortcutsPath() {
    return appPath() + "/shortcuts";
}

QString Paths::qtTranslationPath() {
    return QLibraryInfo::location(QLibraryInfo::TranslationsPath);// /usr/share/qt5/translations
}

void Paths::setConfigPath(QString path) {
    m_config_path = path;
    QDir dir(m_config_path);
    if(!dir.exists())
    {
        dir.mkdir(m_config_path);
    }
}

void Paths::setLogPath(QString path)
{
    m_log_path = path;
    QDir dir(m_config_path);
    if(!dir.exists())
    {
        dir.mkdir(m_log_path);
    }
}

QString Paths::configPath() {
    if (!m_config_path.isEmpty()) {
        return m_config_path;
    } else {
        QDir dir(QDir::homePath() + "/.config/lingmo-video");
        if(!dir.exists())
        {
            dir.mkdir(QDir::homePath() + "/.config/lingmo-video");
        }
        return QDir::homePath() + "/.config/lingmo-video";
    }
}

QString Paths::iniPath() {
    return configPath();
}

QString Paths::logPath()
{
    if (!m_log_path.isEmpty()) {
        return m_log_path;
    } else {
        QDir dir(QDir::homePath() + "/.cache/lingmo-video");
        if(!dir.exists())
        {
            bool ret = dir.mkpath(QDir::homePath() + "/.cache/lingmo-video");
            if(!ret)
            {
                KyInfo() << "log path create error!";
            }
        }
        return QDir::homePath() + "/.cache/lingmo-video";
    }
}

QString Paths::subtitleStyleFile() {
    return configPath() + "/styles.ass";
}

