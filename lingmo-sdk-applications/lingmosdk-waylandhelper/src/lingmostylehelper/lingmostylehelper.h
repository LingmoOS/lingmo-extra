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

#ifndef LINGMOSTYLEHELPER_H
#define LINGMOSTYLEHELPER_H
#include <QWindow>

namespace kdk {

/**
 * @defgroup LingmoStyleModule
 */

/**
 * @brief 解决wayland环境下的双标题栏问题
 */
class LingmoStyleHelper:public QObject
{
public:
    static LingmoStyleHelper *self();

    /**
     * @brief 移除窗管标题栏
     * @param widget
     * @since 1.2
     */
    void removeHeader(QWidget* widget);

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    LingmoStyleHelper();
    QWidget* m_widget;
};

}
#endif // LINGMOSTYLEHELPER_H
