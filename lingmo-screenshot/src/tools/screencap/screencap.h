/*
 *
 * Copyright: 2020 LingmoSoft Co., Ltd.
 * Authors:
 *   huanhuan zhang <zhanghuanhuan@lingmoos.cn>
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
 */
#ifndef SCREENCAP_H
#define SCREENCAP_H
#include "src/tools/CaptureToolCap.h"
class ScreenCap : public CaptureToolCap
{
    Q_OBJECT
public:
    explicit ScreenCap(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENCAP_H
