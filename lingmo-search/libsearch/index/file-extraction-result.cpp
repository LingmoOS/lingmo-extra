/*
 * Copyright (C) 2022, LingmoSoft Co., Ltd.
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
 *
 */

#include "file-extraction-result.h"
namespace LingmoSearch {
class FileExtractionResultPrivate
{
public:
    LingmoFileMetadata::PropertyMultiMap m_properties;
    QString m_text;
    QVector<LingmoFileMetadata::Type::Type> m_types;
};

LingmoSearch::FileExtractionResult::FileExtractionResult(const QString &url, const QString &mimetype,
                                                       const LingmoFileMetadata::ExtractionResult::Flags &flags)
        : ExtractionResult(url, mimetype, flags)
        , d(new FileExtractionResultPrivate)
{
}

FileExtractionResult::~FileExtractionResult() = default;


FileExtractionResult::FileExtractionResult(const FileExtractionResult &rhs): ExtractionResult(*this)
        , d(new FileExtractionResultPrivate(*rhs.d))
{
}

FileExtractionResult &FileExtractionResult::operator=(const FileExtractionResult &rhs)
{
    *d = *rhs.d;
    return *this;
}

void FileExtractionResult::add(LingmoFileMetadata::Property::Property property, const QVariant &value)
{
    d->m_properties.insert(property, value);
}

void FileExtractionResult::addType(LingmoFileMetadata::Type::Type type)
{
    d->m_types << type;
}

void FileExtractionResult::append(const QString &text)
{
    QString tmp = text;
    d->m_text.append(tmp.replace("\n", "").replace("\r", " "));
}

LingmoFileMetadata::PropertyMultiMap FileExtractionResult::properties() const
{
    return d->m_properties;
}

QString FileExtractionResult::text() const
{
    return d->m_text;
}

QVector<LingmoFileMetadata::Type::Type> FileExtractionResult::types() const
{
    return d->m_types;
}
}