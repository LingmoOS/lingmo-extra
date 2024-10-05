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
#include <unistd.h>

#include "abstractinterface.h"
#include <QGuiApplication>
#include <QRect>
#include <QScreen>

using namespace kdk;

AbstractInterface::AbstractInterface(QObject *parent)
    :QObject(parent)
{
}

AbstractInterface::~AbstractInterface()
{
}

bool AbstractInterface::inCurrentDesktopActivity(const WindowInfo &winfo)
{
    if(winfo.isValid() && winfo.isOnDesktop(currentDesktop()))
    return (winfo.isValid() && winfo.isOnDesktop(currentDesktop()) /*&& winfo.isOnActivity(currentActivity())*/);
}

bool AbstractInterface::isPlasmaDesktop(const QRect &wGeometry)
{
    if (wGeometry.isEmpty()) {
        return false;
    }

    for (const auto scr : qGuiApp->screens()) {
        if (wGeometry == scr->geometry()) {
            return true;
        }
    }

    return false;
}

QString AbstractInterface::currentDesktop()
{
    return m_currentDesktop;
}

QString AbstractInterface::currentActivity()
{
    return m_currentActivity;
}

void AbstractInterface::setPlasmaDesktop(WindowId wid)
{
    if (!m_windows.contains(wid)) {
        return;
    }

    if (!m_windows[wid].isPlasmaDesktop()) {
        m_windows[wid].setIsPlasmaDesktop(true);
        //updateAllHints();
    }
}

bool AbstractInterface::isValidFor(const WindowId &wid)
{
    if (!m_windows.contains(wid)) {
        return false;
    }

    return m_windows[wid].isValid() && !m_windows[wid].isPlasmaDesktop();
}

QList<WindowId> AbstractInterface::windows()
{
    return m_windows.keys();
}

QString AbstractInterface::getProcessName(const WindowId &wid)
{
    auto id = pid(wid);
    char path[128];
    memset(path,0,128);
    char name[128];
    memset(name,0,128);
    if(!name)
    {
        return NULL;
    }
    char link[512];
    memset(link,0,512);
    sprintf(path, "/proc/%d/exe", id);

    int count = readlink(path, link, sizeof(link));
    if(count != -1)
    {
        strcpy(name, basename(link));
    }
    return QString(name);
}
