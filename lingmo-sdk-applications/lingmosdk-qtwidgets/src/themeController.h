/*
 * lingmo-sdk-qtwidgets's Library
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

#ifndef THEMECONTROLLER_H
#define THEMECONTROLLER_H

#include <QGSettings/QGSettings>
#include <QPushButton>

static const QByteArray FITTHEMEWINDOW =  "org.lingmo.style";

enum ThemeFlag
{
    LightTheme,
    DarkTheme
};

enum IconFlag
{
    DefaultStyle,
    ClassicStyle
};

enum WidgetThemeFlag
{
    DefaultTheme, //寻光
    ClassicTheme, //启典
    FashionTheme  //和印
};

class ThemeController
{

public:
    static QPixmap drawSymbolicColoredPixmap(const QPixmap &source);
    static QPixmap drawColoredPixmap(const QPixmap &source,const QColor &sampleColor);
    static QColor getCurrentIconColor();
    static QColor mixColor(const QColor &c1, const QColor &c2, qreal bias = 0.5);
    static WidgetThemeFlag widgetTheme();
    static ThemeFlag themeMode();
    static IconFlag iconTheme();
    static int systemFontSize();
    static QPixmap drawFashionBackground(const QRect&rect,int sub_width,int sub_height,int radius,int flag); // flag ? rightBottom : leftBottom;

    ThemeController();
    ~ThemeController();

    virtual void changeTheme(){}
    virtual void changeIconStyle(){}
    void initThemeStyle();

    QGSettings *m_gsetting;
    //unused
    ThemeFlag m_themeFlag;
    IconFlag m_iconFlag;
};

#endif // THEMECONTROLLER_H
