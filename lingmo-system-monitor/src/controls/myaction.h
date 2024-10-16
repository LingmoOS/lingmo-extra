/*
 * Copyright (C) 2020 LingmoSoft Co., Ltd.
 *
 * Authors:
 *  Kobe Lee    xiangli@ubuntulingmo.com/kobe24_lixiang@126.com
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

#ifndef _MYACTION_H_
#define _MYACTION_H_

#include <QAction>
#include <QString>
#include <QIcon>
#include <QKeySequence>

class MyAction : public QAction
{

public:

    MyAction (QObject * parent, const char * name, bool autoadd = true );
    MyAction (QObject * parent, bool autoadd = true );
    MyAction (const QString & text, QKeySequence accel,
               QObject * parent, const char * name = "",
               bool autoadd = true );
    MyAction (QKeySequence accel, QObject * parent,
               const char * name = "", bool autoadd = true );
    ~MyAction();
    void addShortcut(QKeySequence key);
    void change(const QIcon & icon, const QString & text );
    void change(const QString & text);

protected:
    void addActionToParent();
};

#endif

