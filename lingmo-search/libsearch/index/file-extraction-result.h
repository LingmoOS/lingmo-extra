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
#ifndef LINGMO_SEARCH_FILE_EXTRACTION_RESULT_H
#define LINGMO_SEARCH_FILE_EXTRACTION_RESULT_H

#include <extraction-result.h>

namespace LingmoSearch {
class FileExtractionResultPrivate;
class FileExtractionResult : public LingmoFileMetadata::ExtractionResult
{
public:
    explicit FileExtractionResult(const QString& url, const QString& mimetype = QString(), const Flags& flags = Flags{ExtractPlainText | ExtractMetaData});
    FileExtractionResult(const FileExtractionResult& rhs);
    ~FileExtractionResult() override;
    FileExtractionResult& operator=(const FileExtractionResult& rhs);

    void add(LingmoFileMetadata::Property::Property property, const QVariant& value) override;
    void addType(LingmoFileMetadata::Type::Type type) override;
    void append(const QString& text) override;

    LingmoFileMetadata::PropertyMultiMap properties() const;
    QString text() const;
    QVector<LingmoFileMetadata::Type::Type> types() const;
private:
    const std::unique_ptr<FileExtractionResultPrivate> d;
};
}
#endif //LINGMO_SEARCH_FILE_EXTRACTION_RESULT_H
