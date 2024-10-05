/*
 * Copyright (C) 2020, LingmoSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangzihao <zhangzihao@lingmoos.cn>
 *
 */
#include "lingmo-search-qdbus.h"
#include <QDebug>
using namespace LingmoSearch;
LingmoSearchQDBus::LingmoSearchQDBus() {
    this->tmpSystemQDBusInterface = new QDBusInterface("com.lingmo.search.qt.systemdbus",
            "/",
            "com.lingmo.search.interface",
            QDBusConnection::systemBus());
    if(!tmpSystemQDBusInterface->isValid()) {
        qCritical() << "Create Client Interface Failed When execute chage: " << QDBusConnection::systemBus().lastError();
        return;
    }
}
LingmoSearchQDBus::~LingmoSearchQDBus() {
    if (this->tmpSystemQDBusInterface)
        delete this->tmpSystemQDBusInterface;
    this->tmpSystemQDBusInterface = nullptr;
}

void LingmoSearchQDBus::setInotifyMaxUserWatches() {
    // /proc/sys/fs/inotify/max_user_watches

    this->tmpSystemQDBusInterface->call("setInotifyMaxUserWatchesStep2");
}

void LingmoSearchQDBus::addInotifyUserInstances(int addNum)
{
    QDBusReply<int> reply = tmpSystemQDBusInterface->call("AddInotifyMaxUserInstance", addNum);
    if(reply.isValid()) {
        qDebug() << "Set inotify_max_user_instances to" << reply.value();
    } else {
        qWarning() << "Call AddInotifyMaxUserInstance failed!";
    }
}
