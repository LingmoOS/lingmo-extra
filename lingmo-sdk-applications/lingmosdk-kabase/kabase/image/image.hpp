/*
 * lingmo-sdk-base's Library
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
 * Authors: Shengjie Ji <jishengjie@lingmoos.cn>
 *
 */

#ifndef IMAGE_IMAGE_H_
#define IMAGE_IMAGE_H_

#include <QImage>
#include <QString>

namespace kdk
{
namespace kabase
{

class Image
{
public:
    Image();
    ~Image();

    QImage loadImage(QString path);

private:
};

} /* namespace kabase */
} /* namespace kdk */

#endif
