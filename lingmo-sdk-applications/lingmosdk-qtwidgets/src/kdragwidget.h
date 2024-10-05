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

#ifndef KDRAGWIDGET_H
#define KDRAGWIDGET_H

#include "gui_g.h"
#include "kpushbutton.h"
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>

namespace kdk {

/**
 * @defgroup DragModule
 */
class KDragWidgetPrivate;

/**
 * @brief 拖动文件或文件夹，可识别并获取文件或文件夹路径
 * @since 2.3
 */
class GUI_EXPORT KDragWidget :public QWidget
{
    Q_OBJECT

public:
    KDragWidget(QWidget*parent=nullptr);

    /**
     * @brief iconButton
     * @return 返回iconbutton
     */
    KPushButton *iconButton();

    /**
     * @brief textLabel
     * @return 返回textlabel
     */
    QLabel *textLabel();

    /**
     * @brief fileDialog
     * @return 返回QFiledialog
     */
    QFileDialog *fileDialog();

    /**
     * @brief 将文件对话框中使用的过滤器设置为给定的过滤器。
     * @param filter
     */
    void setNameFilter(const QString &filter);

    /**
     * @brief 设置文件对话框中使用的过滤器。
     * @param filters
     */
    void setNameFilters(const QStringList &filters);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent* event);

Q_SIGNALS:
    void getPath(QString);

private:
    Q_DECLARE_PRIVATE(KDragWidget)
    KDragWidgetPrivate* const d_ptr;
};

}
#endif // KDRAGWIDGET_H
