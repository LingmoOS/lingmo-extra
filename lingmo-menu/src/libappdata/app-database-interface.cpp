/*
 * Copyright (C) 2024, LingmoSoft Co., Ltd.
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
 * Authors: hxf <hewenfei@lingmoos.cn>
 *
 */


#include "app-database-interface.h"
#include "user-config.h"
#include "settings.h"

#include <application-info.h>
#include <QDebug>

#define APP_ICON_PREFIX "image://theme/"

namespace LingmoMenu {

class AppDatabaseWorkerPrivate : public QObject
{
    Q_OBJECT
    friend class AppDatabaseInterface;

public:
    explicit AppDatabaseWorkerPrivate(AppDatabaseInterface *parent = nullptr);
    DataEntityVector getAllApps();
    bool getApp(const QString &appid, DataEntity &app);

    // 数据库操作函数
    void setAppProperty(const QString &appid, const LingmoSearch::ApplicationPropertyMap &propertyMap);
    void setAppProperty(const QString &appid, const LingmoSearch::ApplicationProperty::Property &property, const QVariant &value);

private Q_SLOTS:
    /**
     * 应用数据库的添加信号处理函数
     * @param infos 新增应用的id列表
     */
    void onAppDatabaseAdded(const QStringList &infos);
    void onAppDatabaseUpdate(const LingmoSearch::ApplicationInfoMap &infoMap);
    void onAppDatabaseUpdateAll(const QStringList &infos);
    void onAppDatabaseDeleted(const QStringList &infos);

private:
    static void addInfoToApp(const QMap<LingmoSearch::ApplicationProperty::Property, QVariant> &info, DataEntity &app);
    bool isFilterAccepted(const LingmoSearch::ApplicationPropertyMap &appInfo) const;

private:
    AppDatabaseInterface *q {nullptr};

    LingmoSearch::ApplicationInfo       *appDatabase {nullptr};
    LingmoSearch::ApplicationProperties  properties;
    // 设置我们需要的属性和值
    LingmoSearch::ApplicationPropertyMap filter;
};

AppDatabaseWorkerPrivate::AppDatabaseWorkerPrivate(AppDatabaseInterface *parent) : QObject(parent), q(parent)
{
    // 注册需要在信号和槽函数中使用的数据结构
    qRegisterMetaType<LingmoMenu::DataEntityVector>("DataEntityVector");

    // 初始化应用数据库链接
    appDatabase = new LingmoSearch::ApplicationInfo(this);

    // 首次启动时，为某些应用设置标志位
    if (UserConfig::instance()->isFirstStartUp()) {
        // 默认收藏应用
        for (const auto &appid : GlobalSetting::instance()->defaultFavoriteApps()) {
            appDatabase->setAppToFavorites(appid);
            appDatabase->setAppLaunchedState(appid, true);
        }
    }

    // 设置从数据库查询哪些属性
    properties << LingmoSearch::ApplicationProperty::Property::Top
                  << LingmoSearch::ApplicationProperty::Property::Lock
                  << LingmoSearch::ApplicationProperty::Property::Favorites
                  << LingmoSearch::ApplicationProperty::Property::LaunchTimes
                  << LingmoSearch::ApplicationProperty::Property::DesktopFilePath
                  << LingmoSearch::ApplicationProperty::Property::Icon
                  << LingmoSearch::ApplicationProperty::Property::LocalName
                  << LingmoSearch::ApplicationProperty::Property::Category
                  << LingmoSearch::ApplicationProperty::Property::FirstLetterAll
                  << LingmoSearch::ApplicationProperty::Property::DontDisplay
                  << LingmoSearch::ApplicationProperty::Property::AutoStart
                  << LingmoSearch::ApplicationProperty::Property::InsertTime
                  << LingmoSearch::ApplicationProperty::Property::Launched;
    
    // 需要从数据库过滤的属性，满足该条件的数据才会被查询出来
    filter.insert(LingmoSearch::ApplicationProperty::Property::DontDisplay, 0);
    filter.insert(LingmoSearch::ApplicationProperty::Property::AutoStart, 0);

    // 链接数据库信号
    connect(appDatabase, &LingmoSearch::ApplicationInfo::appDBItems2BAdd, this, &AppDatabaseWorkerPrivate::onAppDatabaseAdded);
    connect(appDatabase, &LingmoSearch::ApplicationInfo::appDBItems2BUpdate, this, &AppDatabaseWorkerPrivate::onAppDatabaseUpdate);
    connect(appDatabase, &LingmoSearch::ApplicationInfo::appDBItems2BUpdateAll, this, &AppDatabaseWorkerPrivate::onAppDatabaseUpdateAll);
    connect(appDatabase, &LingmoSearch::ApplicationInfo::appDBItems2BDelete, this, &AppDatabaseWorkerPrivate::onAppDatabaseDeleted);
    connect(appDatabase, &LingmoSearch::ApplicationInfo::DBOpenFailed, q, &AppDatabaseInterface::appDatabaseOpenFailed);
}

void AppDatabaseWorkerPrivate::addInfoToApp(const LingmoSearch::ApplicationPropertyMap &info, DataEntity &app)
{
    app.setTop(info.value(LingmoSearch::ApplicationProperty::Property::Top).toInt());
    app.setLock(info.value(LingmoSearch::ApplicationProperty::Property::Lock).toInt() == 1);
    app.setFavorite(info.value(LingmoSearch::ApplicationProperty::Property::Favorites).toInt());
    app.setLaunchTimes(info.value(LingmoSearch::ApplicationProperty::Property::LaunchTimes).toInt());
    app.setId(info.value(LingmoSearch::ApplicationProperty::Property::DesktopFilePath).toString());
    app.setIcon(info.value(LingmoSearch::ApplicationProperty::Property::Icon).toString());
    app.setName(info.value(LingmoSearch::ApplicationProperty::Property::LocalName).toString());
    app.setCategory(info.value(LingmoSearch::ApplicationProperty::Property::Category).toString());
    app.setFirstLetter(info.value(LingmoSearch::ApplicationProperty::Property::FirstLetterAll).toString());
    app.setInsertTime(info.value(LingmoSearch::ApplicationProperty::Property::InsertTime).toString());
    app.setLaunched(info.value(LingmoSearch::ApplicationProperty::Property::Launched).toInt());
}

bool AppDatabaseWorkerPrivate::isFilterAccepted(const LingmoSearch::ApplicationPropertyMap &appInfo) const
{
    QMapIterator<LingmoSearch::ApplicationProperty::Property, QVariant> iterator(filter);
    while (iterator.hasNext()) {
        iterator.next();
        if (appInfo.value(iterator.key()) != iterator.value()) {
            return false;
        }

        // TODO: 根据数据的类型进行比较
//        bool equals = false;
//        QVariant value = appInfo.value(iterator.key());
//        value.userType();
    }

    return true;
}

DataEntityVector AppDatabaseWorkerPrivate::getAllApps()
{
    LingmoSearch::ApplicationInfoMap appInfos = appDatabase->getInfo(properties, filter);
    if (appInfos.isEmpty()) {
        return {};
    }

    DataEntityVector apps;
    for (const auto &info : appInfos) {
        DataEntity app;
        AppDatabaseWorkerPrivate::addInfoToApp(info, app);
        apps.append(app);
    }

    return apps;
}

void AppDatabaseWorkerPrivate::onAppDatabaseAdded(const QStringList &infos)
{
    if (infos.isEmpty()) {
        return;
    }

    DataEntityVector apps;
    for (const QString &appid : infos) {
        const LingmoSearch::ApplicationPropertyMap appInfo = appDatabase->getInfo(appid, properties);
        if (!isFilterAccepted(appInfo)) {
            continue;
        }

        DataEntity app;
        addInfoToApp(appInfo, app);
        apps.append(app);
    }

    if (apps.isEmpty()) {
        return;
    }

    Q_EMIT q->appAdded(apps);
}

void AppDatabaseWorkerPrivate::onAppDatabaseUpdate(const LingmoSearch::ApplicationInfoMap &infoMap)
{
    if (infoMap.isEmpty()) {
        return;
    }

    QVector<QPair<DataEntity, QVector<int> > > updates;
    QMapIterator<QString, LingmoSearch::ApplicationPropertyMap> iterator(infoMap);
    while (iterator.hasNext()) {
        iterator.next();

        DataEntity app;
        QVector<int> roles;

        QMapIterator<LingmoSearch::ApplicationProperty::Property, QVariant> it(iterator.value());
        while (it.hasNext()) {
            it.next();

            switch (it.key()) {
                case LingmoSearch::ApplicationProperty::LocalName:
                    app.setName(it.value().toString());
                    roles.append(DataEntity::Name);
                    break;
                case LingmoSearch::ApplicationProperty::FirstLetterAll:
                    app.setFirstLetter(it.value().toString());
                    roles.append(DataEntity::FirstLetter);
                    break;
                case LingmoSearch::ApplicationProperty::Icon:
                    app.setIcon(it.value().toString());
                    roles.append(DataEntity::Icon);
                    break;
                case LingmoSearch::ApplicationProperty::InsertTime:
                    app.setInsertTime(it.value().toString());
                    roles.append(DataEntity::Icon);
                    break;
                case LingmoSearch::ApplicationProperty::Category:
                    app.setCategory(it.value().toString());
                    roles.append(DataEntity::Category);
                    break;
                case LingmoSearch::ApplicationProperty::LaunchTimes:
                    app.setLaunchTimes(it.value().toInt());
                    roles.append(DataEntity::LaunchTimes);
                    break;
                case LingmoSearch::ApplicationProperty::Favorites:
                    app.setFavorite(it.value().toInt());
                    roles.append(DataEntity::Favorite);
                    break;
                case LingmoSearch::ApplicationProperty::Launched:
                    app.setLaunched(it.value().toInt());
                    roles.append(DataEntity::IsLaunched);
                    break;
                case LingmoSearch::ApplicationProperty::Top:
                    app.setTop(it.value().toInt());
                    roles.append(DataEntity::Top);
                    break;
                case LingmoSearch::ApplicationProperty::Lock:
                    app.setLock(it.value().toBool());
                    roles.append(DataEntity::IsLocked);
                    break;
                default:
                    break;
            }
        }

        // 这个函数中，没有更新列不会发送信号
        if (roles.isEmpty()) {
            continue;
        }

        app.setId(iterator.key());
        updates.append({app, roles});
    }

    if (!updates.isEmpty()) {
        Q_EMIT q->appUpdated(updates);
    }
}

void AppDatabaseWorkerPrivate::onAppDatabaseDeleted(const QStringList &infos)
{
    if (infos.empty()) {
        return;
    }

    Q_EMIT q->appDeleted(infos);
}

void AppDatabaseWorkerPrivate::onAppDatabaseUpdateAll(const QStringList &infos)
{
    if (infos.isEmpty()) {
        return;
    }

    QVector<QPair<DataEntity, QVector<int> > > updates;
    DataEntityVector apps;

    for (const auto &appid : infos) {
        const LingmoSearch::ApplicationPropertyMap appInfo = appDatabase->getInfo(appid, properties);
        DataEntity app;
        addInfoToApp(appInfo, app);
        apps.append(app);

        updates.append({app, {}});
    }

    Q_EMIT q->appUpdated(updates);
}

void AppDatabaseWorkerPrivate::setAppProperty(const QString &appid, const LingmoSearch::ApplicationPropertyMap &propertyMap)
{
    QMapIterator<LingmoSearch::ApplicationProperty::Property, QVariant> iterator(propertyMap);
    while (iterator.hasNext()) {
        iterator.next();
        setAppProperty(appid, iterator.key(), iterator.value());
    }
}

void AppDatabaseWorkerPrivate::setAppProperty(const QString &appid, const LingmoSearch::ApplicationProperty::Property &property,
                                              const QVariant &value)
{
    switch (property) {
        case LingmoSearch::ApplicationProperty::Favorites:
            appDatabase->setFavoritesOfApp(appid, value.toInt());
            break;
        case LingmoSearch::ApplicationProperty::Launched:
            appDatabase->setAppLaunchedState(appid, value.toBool());
            break;
        case LingmoSearch::ApplicationProperty::Top:
            appDatabase->setTopOfApp(appid, value.toInt());
            break;
        default:
            break;
    }
}

bool AppDatabaseWorkerPrivate::getApp(const QString &appid, DataEntity &app)
{
    if (appid.isEmpty()) {
        return false;
    }

    const LingmoSearch::ApplicationPropertyMap appInfo = appDatabase->getInfo(appid, properties);
    addInfoToApp(appInfo, app);
    return true;
}

// ====== AppDatabaseInterface ====== //
// TODO: 多线程
AppDatabaseInterface::AppDatabaseInterface(QObject *parent) : QObject(parent), d(new AppDatabaseWorkerPrivate(this))
{

}

DataEntityVector AppDatabaseInterface::apps() const
{
    return d->getAllApps();
}

void AppDatabaseInterface::fixAppToTop(const QString &appid, int index) const
{
    if (index < 0) {
        index = 0;
    }

    d->setAppProperty(appid, LingmoSearch::ApplicationProperty::Top, index);
}

void AppDatabaseInterface::fixAppToFavorite(const QString &appid, int index) const
{
    if (index < 0) {
        index = 0;
    }

    d->setAppProperty(appid, LingmoSearch::ApplicationProperty::Favorites, index);
}

void AppDatabaseInterface::updateApLaunchedState(const QString &appid, bool state) const
{
    d->setAppProperty(appid, LingmoSearch::ApplicationProperty::Launched, state);
}

bool AppDatabaseInterface::getApp(const QString &appid, DataEntity &app) const
{
    return d->getApp(appid, app);
}

} // LingmoMenu

#include "app-database-interface.moc"
