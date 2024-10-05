#include "systemtrayicon.h"

#include <QMenu>
#include <QAction>

SystemTrayIcon::SystemTrayIcon(QObject *parent) :
    QSystemTrayIcon(parent)
{
    initMenu();
    initIcon();
    setToolTip(tr("Video Player"));
}

SystemTrayIcon::~SystemTrayIcon()
{

}

void SystemTrayIcon::initIcon()
{
    setIcon(QIcon::fromTheme("lingmo-video"));
}

void SystemTrayIcon::initMenu()
{
    menu = new QMenu;
    actQuit = new QAction(tr("Quit"));
    connect(actQuit, &QAction::triggered, [&](){emit sigQuit();});
    menu->addAction(actQuit);
    setContextMenu(menu);
}
