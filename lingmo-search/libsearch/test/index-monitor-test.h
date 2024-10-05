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
 * Authors: iaom <zhangpengfei@lingmoos.cn>
 *
 */

#ifndef LINGMO_SEARCH_INDEX_MONITOR_TEST_H
#define LINGMO_SEARCH_INDEX_MONITOR_TEST_H

#include <QObject>
#include "index-monitor.h"

class IndexMonitorTest : public QObject
{
    Q_OBJECT
public:
    explicit IndexMonitorTest(QObject *parent = nullptr);
    ~IndexMonitorTest();

private:
    LingmoSearch::IndexMonitor *m_monitor = nullptr;

};


#endif //LINGMO_SEARCH_INDEX_MONITOR_TEST_H
