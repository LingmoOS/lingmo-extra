#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

class QAction;

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit SystemTrayIcon(QObject *parent = nullptr);
    ~SystemTrayIcon();

    void initIcon();
    void initMenu();

signals:
    void sigQuit();

private:
    QMenu *menu;
    QAction *actQuit;
};

#endif // SYSTEMTRAYICON_H
