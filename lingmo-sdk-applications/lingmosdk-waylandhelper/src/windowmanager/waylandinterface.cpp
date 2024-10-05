/*
 * lingmo-sdk-waylandhelper's Library
 *
 * Copyright (C) 2023, LingmoSoft Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Zhen Sun <sunzhen1@lingmoos.cn>
 *
 */

#include "waylandinterface.h"

#include <QApplication>
#include <QDebug>
#include <KWayland/Client/registry.h>
#include <KWayland/Client/surface.h>
#include <QWindow>
#include <QEvent>
#include "wayland-plasma-window-management-client-protocol.h"
WaylandInterface::WaylandInterface(QObject *parent)
    :AbstractInterface(parent)
{
   m_connection = KWayland::Client::ConnectionThread::fromApplication(qApp);
        auto registry = new Registry(this);
        registry->create(m_connection->display());


        connect(registry, &KWayland::Client::Registry::plasmaShellAnnounced, this, [=](){
            const auto interface = registry->interface(KWayland::Client::Registry::Interface::PlasmaShell);
            if (interface.name != 0) {
                m_plasmaShell = registry->createPlasmaShell(interface.name, interface.version, this);
            }
        });

        connect(registry, &Registry::plasmaWindowManagementAnnounced, this, [=](){
            const auto interface = registry->interface(Registry::Interface::PlasmaWindowManagement);
            if (interface.name != 0) {
                m_windowManager = registry->createPlasmaWindowManagement(interface.name, interface.version, this);
            }
            if(m_windowManager)
            {
                connect(m_windowManager, &PlasmaWindowManagement::showingDesktopChanged,this,&AbstractInterface::isShowingDesktopChanged);
                connect(m_windowManager, &PlasmaWindowManagement::windowCreated, this, &WaylandInterface::windowCreatedProxy);
                connect(m_windowManager, &PlasmaWindowManagement::activeWindowChanged, this, [&]() noexcept {
                            auto w = m_windowManager->activeWindow();
                            if(w)
                            {
                                emit activeWindowChanged(w ? w->internalId() : 0);
                            }
                }, Qt::QueuedConnection);
                connect(m_windowManager, &PlasmaWindowManagement::windowCreated,this,
                    [this](PlasmaWindow *window)
                {
                    if (!m_windows.contains(window->internalId())) {
                        m_windows.insert(window->internalId(), this->requestInfo(window->internalId()));
                    }
                    emit windowAdded(window->internalId());
                });
            }
        });

        connect(registry, &KWayland::Client::Registry::plasmaVirtualDesktopManagementAnnounced,
                             [this, registry] (quint32 name, quint32 version) {
            m_virtualDesktopManagement = registry->createPlasmaVirtualDesktopManagement(name, version, this);
            if(m_virtualDesktopManagement)
            {
                connect(m_virtualDesktopManagement, &KWayland::Client::PlasmaVirtualDesktopManagement::desktopCreated, this,
                        [this](const QString &id, quint32 position) {
                    addDesktop(id, position);
                });

                connect(m_virtualDesktopManagement, &KWayland::Client::PlasmaVirtualDesktopManagement::desktopRemoved, this,
                        [this](const QString &id) {
                    m_desktops.removeAll(id);

                    if (m_currentDesktop == id) {
                        setCurrentDesktop(QString());
                    }
                });
            }
        });
        connect(registry, &KWayland::Client::Registry::shellAnnounced, this, [=](){
            const auto interface = registry->interface(KWayland::Client::Registry::Interface::Shell);
            if (interface.name != 0) {
                m_shell = registry->createShell(interface.name, interface.version, this);
            }
        });
        registry->setup();
        m_connection->roundtrip();
}

WaylandInterface::~WaylandInterface()
{
}

WindowInfo WaylandInterface::requestInfo(WindowId wid)
{
    WindowInfo windowInfo;

    auto w = windowFor(wid);

    if (w) {
        if (isPlasmaDesktop(w)) {
            windowInfo.setIsValid(true);
            windowInfo.setIsPlasmaDesktop(true);
            windowInfo.setWid(wid);
            //! Window Abilities
            windowInfo.setIsClosable(false);
            windowInfo.setIsFullScreenable(false);
            windowInfo.setIsGroupable(false);
            windowInfo.setIsMaximizable(false);
            windowInfo.setIsMinimizable(false);
            windowInfo.setIsMovable(false);
            windowInfo.setIsResizable(false);
            windowInfo.setIsShadeable(false);
            windowInfo.setIsVirtualDesktopsChangeable(false);
            //! Window Abilities

        } else if (isValidWindow(w)) {
            windowInfo.setIsValid(true);
            windowInfo.setWid(wid);
            windowInfo.setIsActive(w->isActive());
            windowInfo.setIsMinimized(w->isMinimized());
            windowInfo.setIsMaxVert(w->isMaximized());
            windowInfo.setIsMaxHoriz(w->isMaximized());
            windowInfo.setIsFullscreen(w->isFullscreen());
            windowInfo.setIsShaded(w->isShaded());
            windowInfo.setIsOnAllDesktops(w->isOnAllDesktops());
            windowInfo.setIsOnAllActivities(true);
            windowInfo.setHasSkipTaskbar(w->skipTaskbar());
            windowInfo.setHasSkipSwitcher(w->skipSwitcher());
            windowInfo.setIsKeepAbove(w->isKeepAbove());
            //! Window Abilities
            windowInfo.setIsClosable(w->isCloseable());
            windowInfo.setIsFullScreenable(w->isFullscreenable());
            windowInfo.setIsMaximizable(w->isMaximizeable());
            windowInfo.setIsMinimizable(w->isMinimizeable());
            windowInfo.setIsMovable(w->isMovable());
            windowInfo.setIsResizable(w->isResizable());
            windowInfo.setIsShadeable(w->isShadeable());
            windowInfo.setIsVirtualDesktopsChangeable(w->isVirtualDesktopChangeable());
            //! Window Abilities
            windowInfo.setDesktops(w->plasmaVirtualDesktops());
        }
    } else {
        windowInfo.setIsValid(false);
    }

    return windowInfo;

}

void WaylandInterface::requestActivate(WindowId wid)
{
    auto w = windowFor(wid);

    if (w) {
        w->requestActivate();
        m_connection->roundtrip();
        emit windowChanged(w->internalId());
    }
}

void WaylandInterface::requestClose(WindowId wid)
{
    auto w = windowFor(wid);

    if (w) {
        w->requestClose();
        m_connection->roundtrip();
    }
}

void WaylandInterface::requestToggleKeepAbove(WindowId wid)
{
    auto w = windowFor(wid);

    if (w) {
        w->requestToggleKeepAbove();
        m_connection->roundtrip();
        emit windowChanged(w->internalId());
    }
}

void WaylandInterface::requestToggleMinimized(WindowId wid)
{
    auto w = windowFor(wid);

    if (w) {
        w->requestToggleMinimized();
        m_connection->roundtrip();
    }
}

void WaylandInterface::requestToggleMaximized(WindowId wid)
{
    auto w = windowFor(wid);

    if (w) {
        w->requestToggleMaximized();
        m_connection->roundtrip();
    }
}

QIcon WaylandInterface::iconFor(WindowId wid)
{
    auto window = windowFor(wid);

    if (window) {
        return window->icon();
    }

    return QIcon();
}

QString WaylandInterface::titleFor(WindowId wid)
{
    auto window = windowFor(wid);

    if (window) {
        return window->title();
    }
    return QString();
}

QString WaylandInterface::windowGroupFor(WindowId wid)
{
    auto window = windowFor(wid);
    if (window)
    {
        m_connection->roundtrip();
        return window->appId();
    }
    else
        return QString();
}

quint32 WaylandInterface::pid(WindowId wid)
{
    quint32 pid = 0;
    auto window = windowFor(wid);
    if (window)
    {
        m_connection->roundtrip();
        return window->pid();
    }
    else
        return pid;
}

void WaylandInterface::showCurrentDesktop()
{
    if(m_windowManager)
    {
        m_windowManager->showDesktop();
        m_connection->roundtrip();
    }

}

void WaylandInterface::hideCurrentDesktop()
{
    if(m_windowManager)
    {
        m_windowManager->hideDesktop();
        m_connection->roundtrip();
    }
}

bool WaylandInterface::windowCanBeDragged(WindowId wid)
{
    auto w = windowFor(wid);

    if (w && isValidWindow(w)) {
        WindowInfo winfo = requestInfo(wid);
        return (winfo.isValid()
                && w->isMovable()
                && !winfo.isMinimized()
                && inCurrentDesktopActivity(winfo)
                && !winfo.isPlasmaDesktop());
    }

    return false;
}

bool WaylandInterface::windowCanBeMaximized(WindowId wid)
{
    auto w = windowFor(wid);

    if (w && isValidWindow(w)) {
        WindowInfo winfo = requestInfo(wid);
        return (winfo.isValid()
                && w->isMaximizeable()
                && !winfo.isMinimized()
                && inCurrentDesktopActivity(winfo)
                && !winfo.isPlasmaDesktop());
    }

    return false;
}

WindowId WaylandInterface::activeWindow()
{
    if (!m_windowManager) {
        return 0;
    }
    m_connection->roundtrip();
    auto wid = m_windowManager->activeWindow();
    return wid ? QVariant(wid->internalId()) : 0;
}

void WaylandInterface::setGeometry(QWindow *window, const QRect &rect)
{
    if(!window)
        return;
    if (!m_plasmaShell)
        return;

    auto surface = KWayland::Client::Surface::fromWindow(window);
    if (!surface)
        return;
    if(!m_surfaces.contains(window))
    {
        m_surfaces.insert(window,surface);
    }

    auto plasmaShellSurface = m_plasmaShell->createSurface(surface, window);
    if (!plasmaShellSurface)
        return;
    if(!m_plasmaShellSurfaces.contains(window))
    {
        m_plasmaShellSurfaces.insert(window,plasmaShellSurface);
    }
    plasmaShellSurface->setPosition(rect.topLeft());
    window->resize(rect.size());


    window->installEventFilter(this);
}

void WaylandInterface::setSkipTaskBar(QWindow *window, bool skip)
{
    if(!window)
        return;
    if (!m_plasmaShell)
        return;

    auto surface = KWayland::Client::Surface::fromWindow(window);
    if (!surface)
        return;
    if(!m_surfaces.contains(window))
        m_surfaces.insert(window,surface);
    auto plasmaShellSurface = m_plasmaShell->createSurface(surface, window);
    if (!plasmaShellSurface)
        return;
    if(!m_plasmaShellSurfaces.contains(window))
        m_plasmaShellSurfaces.insert(window,plasmaShellSurface);

    plasmaShellSurface->setSkipTaskbar(skip);

    window->installEventFilter(this);
}

void WaylandInterface::setSkipSwitcher(QWindow *window, bool skip)
{
    if(!window)
        return;
    if (!m_plasmaShell)
        return;

    auto surface = KWayland::Client::Surface::fromWindow(window);
    if (!surface)
        return;
    if(!m_surfaces.contains(window))
        m_surfaces.insert(window,surface);
    auto plasmaShellSurface = m_plasmaShell->createSurface(surface, window);
    if (!plasmaShellSurface)
        return;
    if(!m_plasmaShellSurfaces.contains(window))
        m_plasmaShellSurfaces.insert(window,plasmaShellSurface);

    plasmaShellSurface->setSkipSwitcher(skip);


    window->installEventFilter(this);
}

bool WaylandInterface::skipTaskBar(const WindowId &wid)
{
    auto window = windowFor(wid);

    return window ? window->skipTaskbar() : false;
}

bool WaylandInterface::skipSwitcher(const WindowId &wid)
{
    auto window = windowFor(wid);

    return window ? window->skipSwitcher() : false;
}

bool WaylandInterface::isShowingDesktop()
{
    bool flag = false;
    if(m_windowManager)
    {
        flag = m_windowManager->isShowingDesktop();
        m_connection->roundtrip();
    }
    return flag;
}

void WaylandInterface::setOnAllDesktops(const WindowId &wid)
{
    auto w = windowFor(wid);

    if (w && isValidWindow(w) && m_desktops.count() > 1) {
        if (w->isOnAllDesktops()) {
            w->requestEnterVirtualDesktop(m_currentDesktop);
        } else {
            const QStringList &now = w->plasmaVirtualDesktops();

            foreach (const QString &desktop, now) {
                w->requestLeaveVirtualDesktop(desktop);
            }
        }
    }
}

void WaylandInterface::setPanelTakefocus(QWindow *window,bool flag)
{
    if(!window)
        return;
    if (!m_plasmaShell)
        return;

    auto surface = KWayland::Client::Surface::fromWindow(window);
    if (!surface)
        return;
    if(!m_surfaces.contains(window))
    {
        m_surfaces.insert(window,surface);
    }

    auto plasmaShellSurface = m_plasmaShell->createSurface(surface, window);
    if (!plasmaShellSurface)
        return;
    if(!m_plasmaShellSurfaces.contains(window))
    {
        m_plasmaShellSurfaces.insert(window,plasmaShellSurface);
    }
    plasmaShellSurface->setPanelTakesFocus(flag);
    window->installEventFilter(this);
}

NET::WindowType WaylandInterface::windowType(WindowId wid)
{
    return NET::WindowType::Normal;
}

void WaylandInterface::demandAttention(const WindowId &wid)
{
    auto w = windowFor(wid);
    if (w) {
         org_kde_plasma_window_set_state(*w, ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_DEMANDS_ATTENTION, ORG_KDE_PLASMA_WINDOW_MANAGEMENT_STATE_DEMANDS_ATTENTION);
        m_connection->roundtrip();
    }
}

bool WaylandInterface::eventFilter(QObject *obj, QEvent *ev)
{
    auto window = qobject_cast<QWindow*>(obj);
    if(window && ev->type() == QEvent::Hide)
    {
        if(m_plasmaShellSurfaces.contains(window))
        {
            auto plasmaShellSurface = m_plasmaShellSurfaces.value(window);
            if(plasmaShellSurface)
            {
                plasmaShellSurface->release();
                plasmaShellSurface->destroy();
            }
            m_plasmaShellSurfaces.remove(window);
        }
        if(m_surfaces.contains(window))
        {
            auto surface = m_surfaces.value(window);
            if(surface)
            {
                surface->release();
                surface->destroy();
            }
            m_surfaces.remove(window);
        }

    }
    return QObject::eventFilter(obj,ev);
}

void WaylandInterface::updateWindow()
{
    PlasmaWindow* w = qobject_cast<PlasmaWindow*>(QObject::sender());
    if(w && !isPlasmaPanel(w))
    {
        m_connection->roundtrip();
        emit windowChanged(w->internalId());
    }
}

void WaylandInterface::windowUnmapped()
{
    PlasmaWindow *pW = qobject_cast<PlasmaWindow*>(QObject::sender());

    if (pW) {
        untrackWindow(pW);
        if (!m_windows.contains(pW->internalId())) {
            m_windows.remove(pW->internalId());
        }
        emit windowRemoved(pW->internalId());
    }
}

PlasmaWindow *WaylandInterface::windowFor(WindowId wid)
{
    auto it = std::find_if(m_windowManager->windows().constBegin(), m_windowManager->windows().constEnd(), [&wid](PlasmaWindow * w) noexcept {
            return w->isValid() && w->internalId() == wid;
});

    if (it == m_windowManager->windows().constEnd()) {
        return nullptr;
    }

    return *it;
}

bool WaylandInterface::isValidWindow(const PlasmaWindow *w)
{
    return w->isValid();
}

bool WaylandInterface::isPlasmaDesktop(const PlasmaWindow *w)
{
    if (!w || (w->appId() != QLatin1String("org.kde.plasmashell"))) {
        return false;
    }

    return AbstractInterface::isPlasmaDesktop(w->geometry());
}

bool WaylandInterface::isPlasmaPanel(const PlasmaWindow *w)
{
    if(w && w->appId() == QLatin1String("lingmo-panel"))
        return true;
    else
        return false;
}

void WaylandInterface::windowCreatedProxy(PlasmaWindow *w)
{
    if (!isValidWindow(w)) {
        return;
    }

    if ((w->appId() == QLatin1String("org.kde.plasmashell")) && isPlasmaPanel(w))
    {
        return;
    }
    else
    {
        trackWindow(w);
    }
}

void WaylandInterface::trackWindow(PlasmaWindow *w)
{
    if(!w || isPlasmaPanel(w)){
        return;
    }
    //兼容旧的windowchanged信号
    connect(w, &PlasmaWindow::titleChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::fullscreenChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::geometryChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::maximizedChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::minimizedChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::shadedChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::skipTaskbarChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::onAllDesktopsChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::parentWindowChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::iconChanged, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::plasmaVirtualDesktopEntered, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::plasmaVirtualDesktopLeft, this, &WaylandInterface::updateWindow);
    connect(w, &PlasmaWindow::unmapped, this, &WaylandInterface::windowUnmapped);

    //按照具体事件新增信号，since2.3
    connect(w, &PlasmaWindow::titleChanged, this, [=](){emit titleChanged(w->internalId());});
    connect(w, &PlasmaWindow::fullscreenChanged, this, [=](){emit fullscreenChanged(w->internalId());});
    connect(w, &PlasmaWindow::iconChanged, this, [=](){emit iconChanged(w->internalId());});
    connect(w, &PlasmaWindow::activeChanged, this, [=](){emit activeChanged(w->internalId());});
    connect(w, &PlasmaWindow::keepAboveChanged, this, [=](){emit keepAboveChanged(w->internalId());});
    connect(w, &PlasmaWindow::minimizedChanged, this, [=](){emit minimizedChanged(w->internalId());});
    connect(w, &PlasmaWindow::maximizedChanged, this, [=](){emit maximizedChanged(w->internalId());});
    connect(w, &PlasmaWindow::onAllDesktopsChanged, this, [=](){emit onAllDesktopsChanged(w->internalId());});
    connect(w, &PlasmaWindow::demandsAttentionChanged, this, [=](){emit demandsAttentionChanged(w->internalId());});
    connect(w, &PlasmaWindow::skipTaskbarChanged, this, [=](){emit skipTaskbarChanged(w->internalId());});
    connect(w, &PlasmaWindow::skipSwitcherChanged, this, [=](){emit skipSwitcherChanged(w->internalId());});
    connect(w, &PlasmaWindow::geometryChanged, this, [=](){emit geometryChanged(w->internalId());});
}

void WaylandInterface::untrackWindow(PlasmaWindow *w)
{
    if (!w) {
        return;
    }

    disconnect(w, &PlasmaWindow::activeChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::titleChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::fullscreenChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::geometryChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::maximizedChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::minimizedChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::shadedChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::skipTaskbarChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::onAllDesktopsChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::parentWindowChanged, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::plasmaVirtualDesktopEntered, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::plasmaVirtualDesktopLeft, this, &WaylandInterface::updateWindow);
    disconnect(w, &PlasmaWindow::unmapped, this, &WaylandInterface::windowUnmapped);


    disconnect(w, &PlasmaWindow::titleChanged, 0, 0);
    disconnect(w, &PlasmaWindow::iconChanged, 0, 0);
    disconnect(w, &PlasmaWindow::activeChanged, 0, 0);
    disconnect(w, &PlasmaWindow::keepAboveChanged, 0, 0);
    disconnect(w, &PlasmaWindow::keepBelowChanged, 0, 0);
    disconnect(w, &PlasmaWindow::minimizedChanged, 0, 0);
    disconnect(w, &PlasmaWindow::maximizedChanged, 0, 0);
    disconnect(w, &PlasmaWindow::onAllDesktopsChanged, 0, 0);
    disconnect(w, &PlasmaWindow::demandsAttentionChanged, 0, 0);
    disconnect(w, &PlasmaWindow::skipTaskbarChanged,  0, 0);
    disconnect(w, &PlasmaWindow::skipSwitcherChanged, 0, 0);
    disconnect(w, &PlasmaWindow::geometryChanged, 0, 0);
}

void WaylandInterface::setCurrentDesktop(QString desktop)
{
    if (m_currentDesktop == desktop) {
        return;
    }

    m_currentDesktop = desktop;
    emit currentDesktopChanged();
}

void WaylandInterface::addDesktop(const QString &id, quint32 position)
{
    if (m_desktops.contains(id)) {
        return;
    }

    m_desktops.append(id);

    const KWayland::Client::PlasmaVirtualDesktop *desktop = m_virtualDesktopManagement->getVirtualDesktop(id);

    QObject::connect(desktop, &KWayland::Client::PlasmaVirtualDesktop::activated, this,
                     [desktop, this]() {
        setCurrentDesktop(desktop->id());
    }
    );

    if (desktop->isActive()) {
        setCurrentDesktop(id);
    }
}
