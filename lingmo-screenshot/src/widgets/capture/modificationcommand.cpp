/* Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
*
* This file is part of Lingmo-Screenshot.
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

#include "modificationcommand.h"
#include <QPainter>

ModificationCommand::ModificationCommand(QPixmap *p, CaptureTool *t) :
    m_pixmap(p), m_tool(t)
{
    setText(t->name());
}

void ModificationCommand::undo() {
    m_tool->undo(*m_pixmap);
}

void ModificationCommand::redo() {
    QPainter p(m_pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    m_tool->process(p, *m_pixmap, true);
}
