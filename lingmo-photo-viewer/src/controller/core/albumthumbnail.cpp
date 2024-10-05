#include "albumthumbnail.h"
#include <QBitmap>
AlbumThumbnail::AlbumThumbnail(const QString &path, const QString &realFormat)
{
    m_path = path;
    m_realFormat = realFormat;
}

void AlbumThumbnail::run()
{
    ImageAndInfo package;
    MatAndFileinfo maf = File::loadImage(m_path, m_realFormat, IMREAD_REDUCED_COLOR_8);
    if (maf.mat.data) {
        QPixmap pix = Processing::converFormat(maf.mat);
        pix = Processing::resizePix(
            pix, QSize(Variable::ALBUM_IMAGE_SIZE.width() - 2, Variable::ALBUM_IMAGE_SIZE.height() - 2));
        package.image = changImageSize(pix);
    }
    package.info = maf.info;
    QVariant var;
    var.setValue<ImageAndInfo>(package);
    Q_EMIT albumFinish(var);
}

QPixmap AlbumThumbnail::changImageSize(const QPixmap &pix)
{
    if (pix.size() == QSize(Variable::ALBUM_IMAGE_SIZE.width() - 2, Variable::ALBUM_IMAGE_SIZE.height() - 2)) {
        return pix;
    }

    int w = 0;
    int h = 0;

    w = Variable::ALBUM_IMAGE_SIZE.width() - pix.width();
    h = Variable::ALBUM_IMAGE_SIZE.height() - pix.height();

    QPixmap newPix(QSize(Variable::ALBUM_IMAGE_SIZE.width() - 2, Variable::ALBUM_IMAGE_SIZE.height() - 2));
    newPix.fill(Qt::transparent);
    //    QRectF rectangle(0.0, 0.0, Variable::ALBUM_IMAGE_SIZE.width() - 4, Variable::ALBUM_IMAGE_SIZE.height() - 4);
    QPainter painter(&newPix);
    painter.drawPixmap(w / 2, h / 2, pix);

    return newPix;
}
