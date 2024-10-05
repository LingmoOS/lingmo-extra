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

#include "xcbinterface.h"

#include <QRect>
#include <QIcon>
#include <QWidget>
#include <QtX11Extras/QX11Info>
#include <KWindowSystem>
#include <KWindowInfo>
#include <kiconloader.h>
#include <QWindow>
#include "kwindowinfo.h"
#include <QDebug>
XcbInterface::XcbInterface(QObject *parent)
    :AbstractInterface(parent)
{
    m_currentDesktop = QString::number(KWindowSystem::self()->currentDesktop());
    connect(KWindowSystem::self(),&KWindowSystem::showingDesktopChanged,this,&AbstractInterface::isShowingDesktopChanged);
    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this, &AbstractInterface::activeWindowChanged);
    connect(KWindowSystem::self(), &KWindowSystem::windowAdded, this, [=](WindowId wid){
        if (!m_windows.contains(wid)) {
            m_windows.insert(wid, this->requestInfo(wid));
        }
        emit windowAdded(wid);});

    connect(KWindowSystem::self(), &KWindowSystem::windowRemoved, this, [=](WindowId wid){
        m_windows.remove(wid);
        emit windowRemoved(wid);
    });

    connect(KWindowSystem::self(), &KWindowSystem::currentDesktopChanged, this, [&](int desktop) {
        m_currentDesktop = QString::number(desktop);
        emit currentDesktopChanged();
    });

    connect(KWindowSystem::self()
            , static_cast<void (KWindowSystem::*)(WId, NET::Properties, NET::Properties2)>
            (&KWindowSystem::windowChanged)
            , this, &XcbInterface::windowChangedProxy);
    connect(KWindowSystem::self(), &KWindowSystem::activeWindowChanged, this, &AbstractInterface::activeChanged);
}

XcbInterface::~XcbInterface()
{
}

WindowInfo XcbInterface::requestInfo(WindowId wid)
{
    const KWindowInfo winfo{wid.value<WId>(), NET::WMFrameExtents
                | NET::WMWindowType
                | NET::WMGeometry
                | NET::WMDesktop
                | NET::WMState
                | NET::WMName
                | NET::WMVisibleName,
                NET::WM2WindowClass
                | NET::WM2Activities
                | NET::WM2AllowedActions
                | NET::WM2TransientFor};

    bool isDesktop{false};
    if (winfo.windowClassName() == "plasmashell" && isPlasmaDesktop(winfo.geometry())) {
        isDesktop = true;
        this->setPlasmaDesktop(wid);
    }

    WindowInfo windowInfo;

    if (!winfo.valid())
    {
        windowInfo.setIsValid(false);
    }
    else if (isValidWindow(winfo) && !isDesktop)
    {
        windowInfo.setIsValid(true);
        windowInfo.setWid(wid);
        windowInfo.setIsPlasmaDesktop(false);
        windowInfo.setIsActive(KWindowSystem::activeWindow() == wid.value<WId>());
        windowInfo.setIsMinimized(winfo.hasState(NET::Hidden));
        windowInfo.setIsMaxVert(winfo.hasState(NET::MaxVert));
        windowInfo.setIsMaxHoriz(winfo.hasState(NET::MaxHoriz));
        windowInfo.setIsFullscreen(winfo.hasState(NET::FullScreen));
        windowInfo.setIsShaded(winfo.hasState(NET::Shaded));
        windowInfo.setIsOnAllDesktops(winfo.onAllDesktops());
        windowInfo.setIsOnAllActivities(winfo.activities().empty());
        windowInfo.setIsKeepAbove(winfo.hasState(NET::KeepAbove));
        windowInfo.setHasSkipTaskbar(winfo.hasState(NET::SkipTaskbar));
        windowInfo.setIsDemandAttention(winfo.hasState(NET::DemandsAttention));

        //! Window Abilities
        windowInfo.setIsClosable(winfo.actionSupported(NET::ActionClose));
        windowInfo.setIsFullScreenable(winfo.actionSupported(NET::ActionFullScreen));
        windowInfo.setIsMaximizable(winfo.actionSupported(NET::ActionMax));
        windowInfo.setIsMinimizable(winfo.actionSupported(NET::ActionMinimize));
        windowInfo.setIsMovable(winfo.actionSupported(NET::ActionMove));
        windowInfo.setIsResizable(winfo.actionSupported(NET::ActionResize));
        windowInfo.setIsShadeable(winfo.actionSupported(NET::ActionShade));
        windowInfo.setIsVirtualDesktopsChangeable(winfo.actionSupported(NET::ActionChangeDesktop));
        //! Window Abilities

        windowInfo.setDesktops({QString::number(winfo.desktop())});
    }
    else if (m_desktopId == wid)
    {
        windowInfo.setIsValid(true);
        windowInfo.setIsPlasmaDesktop(true);
        windowInfo.setWid(wid);
        windowInfo.setHasSkipTaskbar(true);

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
    }
    return windowInfo;
}

void XcbInterface::requestActivate(WindowId wid)
{
    KWindowSystem::activateWindow(wid.toInt());
}

void XcbInterface::requestClose(WindowId wid)
{
    WindowInfo wInfo = requestInfo(wid);

    if (!wInfo.isValid() || wInfo.isPlasmaDesktop()) {
        return;
    }

    NETRootInfo ri(QX11Info::connection(), NET::CloseWindow);
    ri.closeWindowRequest(wInfo.wid().toUInt());
}

void XcbInterface::requestToggleKeepAbove(WindowId wid)
{
    WindowInfo wInfo = requestInfo(wid);

    if (!wInfo.isValid() || wInfo.isPlasmaDesktop()) {
        return;
    }

    NETWinInfo ni(QX11Info::connection(), wid.toUInt(), QX11Info::appRootWindow(), NET::WMState, NET::Properties2());

    if (wInfo.isKeepAbove()) {
        ni.setState(NET::States(), NET::StaysOnTop);
    } else {
        ni.setState(NET::StaysOnTop, NET::StaysOnTop);
    }
}

void XcbInterface::requestToggleMinimized(WindowId wid)
{
    WindowInfo wInfo = requestInfo(wid);

     if (!wInfo.isValid() || wInfo.isPlasmaDesktop() || !inCurrentDesktopActivity(wInfo)) {
         return;
     }

     if (wInfo.isMinimized()) {
         bool onCurrent = wInfo.isOnDesktop(m_currentDesktop);

         KWindowSystem::unminimizeWindow(wid.toUInt());
         if (onCurrent) {
             KWindowSystem::forceActiveWindow(wid.toUInt());
         }
     } else {
         KWindowSystem::minimizeWindow(wid.toUInt());
     }
}

void XcbInterface::requestToggleMaximized(WindowId wid)
{
    WindowInfo wInfo = requestInfo(wid);

    if (!windowCanBeMaximized(wid) || !inCurrentDesktopActivity(wInfo)) {
        return;
    }

    bool restore = wInfo.isMaxHoriz() && wInfo.isMaxVert();

    if (wInfo.isMinimized()) {
        KWindowSystem::unminimizeWindow(wid.toUInt());
    }

    NETWinInfo ni(QX11Info::connection(), wid.toInt(), QX11Info::appRootWindow(), NET::WMState, NET::Properties2());

    if (restore) {
        ni.setState(NET::States(), NET::Max);
    } else {
        ni.setState(NET::Max, NET::Max);
    }
}

QIcon XcbInterface::iconFor(WindowId wid)
{
    QIcon icon;

    icon.addPixmap(KWindowSystem::icon(wid.value<WId>(), KIconLoader::SizeSmall, KIconLoader::SizeSmall, false));
    icon.addPixmap(KWindowSystem::icon(wid.value<WId>(), KIconLoader::SizeSmallMedium, KIconLoader::SizeSmallMedium, false));
    icon.addPixmap(KWindowSystem::icon(wid.value<WId>(), KIconLoader::SizeMedium, KIconLoader::SizeMedium, false));
    icon.addPixmap(KWindowSystem::icon(wid.value<WId>(), KIconLoader::SizeLarge, KIconLoader::SizeLarge, false));
    icon.addPixmap(KWindowSystem::icon(wid.value<WId>(), KIconLoader::SizeHuge, KIconLoader::SizeHuge, false));
    icon.addPixmap(KWindowSystem::icon(wid.value<WId>(), KIconLoader::SizeEnormous, KIconLoader::SizeEnormous, false));

    return icon;
}

QString XcbInterface::titleFor(WindowId wid)
{
    const KWindowInfo winfo{wid.value<WId>(), NET::WMName};
    if(winfo.valid())
        return winfo.name();
    else
        return QString();
}

QString XcbInterface::windowGroupFor(WindowId wid)
{
    KWindowInfo winfo(wid.toULongLong(), NET::WMName, NET::WM2WindowClass);
    if(winfo.valid())
        return winfo.windowClassClass();
    else
        return QString();

}

void XcbInterface::showCurrentDesktop()
{
    KWindowSystem::setShowingDesktop(true);
}

void XcbInterface::hideCurrentDesktop()
{
    KWindowSystem::setShowingDesktop(false);
}

quint32 XcbInterface::pid(WindowId wid)
{
     quint64 pid = 0;
     const KWindowInfo winfo{wid.value<WId>(), NET::WMName};
     if(!winfo.valid())
         return pid;
     else
     {
         pid = winfo.pid();
     }
     return pid;
}

WindowId XcbInterface::activeWindow()
{
    WId wid = KWindowSystem::self()->activeWindow();
    return QVariant::fromValue(wid);
}

bool XcbInterface::windowCanBeDragged(WindowId wid)
{
    WindowInfo winfo = requestInfo(wid);
    return (winfo.isValid()
            && !winfo.isMinimized()
            && winfo.isMovable()
            && inCurrentDesktopActivity(winfo)
            && !winfo.isPlasmaDesktop());
}

bool XcbInterface::windowCanBeMaximized(WindowId wid)
{
    WindowInfo winfo = requestInfo(wid);
    return (winfo.isValid()
            && !winfo.isMinimized()
            && winfo.isMaximizable()
            && inCurrentDesktopActivity(winfo)
            && !winfo.isPlasmaDesktop());
}

void XcbInterface::setGeometry(QWindow *window, const QRect &rect)
{
    if(window)
        window->setGeometry(rect);
}

void XcbInterface::setSkipTaskBar(QWindow *window, bool skip)
{
    if(skip)
        KWindowSystem::setState(window->winId(),NET::SkipTaskbar);
    else
        KWindowSystem::clearState(window->winId(),NET::SkipTaskbar);
}

void XcbInterface::setSkipSwitcher(QWindow *window, bool skip)
{
    if(skip)
        KWindowSystem::setState(window->winId(),NET::SkipSwitcher);
    else
        KWindowSystem::clearState(window->winId(),NET::SkipSwitcher);
}

bool XcbInterface::skipTaskBar(const WindowId &wid)
{
    const KWindowInfo winfo(wid.value<WId>(), NET::WMState);
    return winfo.valid() ?  winfo.hasState(NET::SkipTaskbar) : false;
}

bool XcbInterface::skipSwitcher(const WindowId &wid)
{
    const KWindowInfo winfo(wid.value<WId>(), NET::WMState);
    return winfo.valid() ?  winfo.hasState(NET::SkipSwitcher) : false;
}

bool XcbInterface::isShowingDesktop()
{
    return KWindowSystem::showingDesktop();
}

void XcbInterface::setOnAllDesktops(const WindowId &wid)
{
    WindowInfo wInfo = requestInfo(wid);

    if (!wInfo.isValid()) {
        return;
    }

    if (KWindowSystem::numberOfDesktops() <= 1) {
        return;
    }

    if (wInfo.isOnAllDesktops()) {
        KWindowSystem::setOnDesktop(wid.toUInt(), KWindowSystem::currentDesktop());
        KWindowSystem::forceActiveWindow(wid.toUInt());
    } else {
        KWindowSystem::setOnAllDesktops(wid.toUInt(), true);
    }
}

void XcbInterface::setPanelTakefocus(QWindow *window, bool flag)
{
    //do nothing
    return;
}

NET::WindowType XcbInterface::windowType(WindowId wid)
{
    KWindowInfo info(wid.value<WId>(),NET::WMWindowType|NET::WMState,NET::WM2TransientFor);
    return info.windowType(NET::AllTypesMask);
}

void XcbInterface::demandAttention(const WindowId &wid)
{
    KWindowSystem::demandAttention(wid.value<WId>());
}

bool XcbInterface::isValidWindow(WindowId wid)
{
    if (this->isValidFor(wid)) {
        return true;
    }
    const KWindowInfo winfo{wid.value<WId>(), NET::WMWindowType | NET::WMState};
    return isValidWindow(winfo);
}

bool XcbInterface::isValidWindow(const KWindowInfo &winfo)
{
    if (this->isValidFor(winfo.win())) {
        return true;
    }

    if (m_desktopId == winfo.win()) {
        return false;
    }

    bool hasSkipTaskbar = winfo.hasState(NET::SkipTaskbar);
    bool hasSkipPager = winfo.hasState(NET::SkipPager);

    bool isSkipped = hasSkipTaskbar && hasSkipPager;

    return !isSkipped;
}

void XcbInterface::windowChangedProxy(WId wid, NET::Properties prop1, NET::Properties2 prop2)
{
    if (prop1 == 0 && !(prop2 & (NET::WM2Activities | NET::WM2TransientFor))) {
        return;
    }
    if (!isValidWindow(wid)) {
        return;
    }
    emit windowChanged(wid);

    const KWindowInfo info(wid,prop1,prop2);
    WindowInfo kInfo = m_windows.value(wid);

    if(prop1.testFlag(NET::WMState) && kInfo.isValid())
    {
        if(info.hasState(NET::State::FullScreen) != kInfo.isFullscreen())
        {
            kInfo.setIsFullscreen(info.hasState(NET::State::FullScreen));
            emit fullscreenChanged(wid);
        }
        if(info.hasState(NET::State::KeepAbove) != kInfo.isKeepAbove())
        {
            kInfo.setIsKeepAbove(info.hasState(NET::State::KeepAbove));
            emit keepAboveChanged(wid);
        }
        if(info.hasState(NET::State::SkipSwitcher) != kInfo.hasSkipSwitcher())
        {
            kInfo.setHasSkipSwitcher(info.hasState(NET::State::SkipSwitcher));
            emit skipSwitcherChanged(wid);
        }
        if(info.hasState(NET::State::SkipTaskbar) != kInfo.hasSkipTaskbar())
        {
            kInfo.setHasSkipTaskbar(info.hasState(NET::State::SkipTaskbar));
            emit skipTaskbarChanged(wid);
        }
        if(info.hasState(NET::State::DemandsAttention) != kInfo.isDemandAttention())
        {
            kInfo.setIsDemandAttention(info.hasState(NET::State::DemandsAttention));
            emit demandsAttentionChanged(wid);
        }
        if(info.hasState(NET::State::Max) != kInfo.isMaximized())
        {
           kInfo.setIsMaxHoriz(info.hasState(NET::State::Max));
           kInfo.setIsMaxVert(info.hasState(NET::State::Max));
           emit maximizedChanged(wid);
        }
        if(info.hasState(NET::State::Hidden) != kInfo.isMinimized())
        {
            kInfo.setIsMinimized(info.hasState(NET::State::Hidden));
            emit minimizedChanged(wid);
        }
    }
    if(prop1.testFlag(NET::WMIcon))
        emit iconChanged(wid);
    if(prop1.testFlag(NET::ActiveWindow))
        emit activeChanged(wid);
    if(prop1.testFlag(NET::WMName) || prop1.testFlag(NET::WMVisibleName))
        emit titleChanged(wid);
    if(prop1.testFlag(NET::WMGeometry))
        emit geometryChanged(wid);


    if(prop1.testFlag(NET::WMDesktop) && info.onAllDesktops())
        emit onAllDesktopsChanged(wid);
}
