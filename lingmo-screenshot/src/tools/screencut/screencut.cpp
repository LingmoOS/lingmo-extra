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
#include "screencut.h"

ScreenCut::ScreenCut(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenCut::name() const
{
    return tr("Options");
}
QString ScreenCut::nameID()
{
     return QLatin1String("");
}
QString ScreenCut::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenCut::isValid() const
{
    return true;
}
