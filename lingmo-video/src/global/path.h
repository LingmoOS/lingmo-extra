#ifndef PATH_H
#define PATH_H

#include <QString>

class Paths {

public:

    static void setAppPath(QString path);
    static QString previewPath();
    static QString appPath();
    static QString translationPath();
    static QString shortcutsPath();
    static QString qtTranslationPath();
    //! Forces to use a different path for the config files
    static void setConfigPath(QString path);
    static void setLogPath(QString path);

    //! Return the path where smplayer should save its config files
    static QString configPath();

    //! Obsolete. Just returns configPath()
    static QString iniPath();

    static QString logPath();

    static QString subtitleStyleFile();

private:
    static QString m_app_path;
    static QString m_log_path;
    static QString m_config_path;
};

#endif // PATH_H
