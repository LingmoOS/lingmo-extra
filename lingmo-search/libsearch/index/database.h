/*
 *
 * Copyright (C) 2023, LingmoSoft Co., Ltd.
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
 */
#ifndef DATABASE_H
#define DATABASE_H
#include <xapian.h>
#include "common.h"
namespace LingmoSearch {

class Database
{
public:
    explicit Database(const DataBaseType &type);
    ~Database();
    bool valid() const;
    bool refresh();

    uint getIndexDocCount();

private:
    Xapian::Database *m_database = nullptr;
    DataBaseType m_type;
};
}
#endif // DATABASE_H
