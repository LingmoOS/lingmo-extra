/*
 *
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
 * Authors: Shengjie Ji <jishengjie@lingmoos.cn>
 *
 */

#ifndef Fun_H_
#define Fun_H_

#include <QObject>

class Fun : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.lingmo.ipmsg.interface");

public:
    Fun() = default;
    ~Fun() = default;

public slots:
    int ride(int a, int b);
};

#endif
