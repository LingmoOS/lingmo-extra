/*
 * Copyright (C) 2021 LingmoSoft Co., Ltd.
 *
 * Authors:
 *  Yang Min yangmin@lingmoos.cn
 *
 * This program is free software; you can redistribute it and/or modify
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kheaderview.h"

#include <QDebug>
#include <QtMath>
#include <QModelIndex>

// Constructor
KHeaderView::KHeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
    installEventFilter(this);
}

// Get header view's hint size
QSize KHeaderView::sizeHint() const
{
    return QSize(width(), 36 + m_spacing);
}

// Get section's hint size
int KHeaderView::sectionSizeHint(int logicalIndex) const
{
    return QHeaderView::sectionSizeHint(logicalIndex);
}
