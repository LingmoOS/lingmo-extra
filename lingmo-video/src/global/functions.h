#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <time.h>
#include <unistd.h>
#include <math.h>

#include <QStringList>
#include <QIcon>
#include <QDate>

struct AVDictionary;

class QHeaderView;
class QMPlay2OSD;
class QMimeData;
class QPainter;
class QPixmap;
class QWindow;
class QIcon;
class QRect;

namespace Functions
{
    struct DemuxerInfo
    {
        QString name;
        QIcon icon;
        QStringList extensions;
    };
    using DemuxersInfos = QVector<DemuxerInfo>;
    using ChecksumList = QVector<quint64>;
    QString Url(QString, const QString &pth = QString());
    QString getUrlType(const QString &url);
    QString getVersion();
    QString timeToStr(const double t, const bool decimals = false);

    QString filePath(const QString &);
    QString fileExt(const QString &);
    QString processComand(const QString &);
    QString humanSize(const qint64 &);
    int executeCMD(QString, QString&);
    bool isVirtualSuppotGpu();
    bool isKirinCpu();
    bool isQingsongDevice();
    bool isGreatwallDevice();
}

#endif // FUNCTIONS_H
