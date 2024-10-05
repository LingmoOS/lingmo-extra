#include "functions.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QHeaderView>
#include <QMimeData>
#include <QPainter>
#include <QDir>
#include <QUrl>
#include <QRegExp>
#include <QWindow>
#include <QLibrary>
#include <QProcess>
#include <QStyleOption>

#include <lingmo-log4qt.h>

QString Functions::Url(QString url, const QString &path)
{
    const QString urlType = getUrlType(url);
    const bool hasBackslash = url.contains('\\');
    if (urlType.isEmpty())
    {
        if (!url.startsWith("/"))
        {
            QString addPth = path.isEmpty() ? QDir::currentPath() : path;
            if (!addPth.endsWith("/"))
                addPth += '/';
            url.prepend(addPth);
        }
        if (hasBackslash && !QFileInfo(url).exists())
            url.replace("\\", "/");
        url.prepend("file://");
    }
    return url;
}

/** ********************
* 获取url类型
* @parm : url
***********************/
QString Functions::getUrlType(const QString &url)
{
    int idx = url.indexOf(':');
    if (idx > -1 && url[0] != '/')
        return url.left(idx);
    return QString();
}

/** ********************
* 时间戳转字符串
* @parm : t 时间
* @parm : decimals 是否有小数
***********************/
QString Functions::timeToStr(const double t, const bool decimals)
{
    if (t <= 0.0)
        return QString("--:--:--");

    const int intT = t;
    const int h = intT / 3600;
    const int m = intT % 3600 / 60;
    const int s = intT % 60;

    QString timStr;
//    if (h > 0)  // 默认先显示小时
        timStr = QString("%1:").arg(h, 2, 10, QChar('0'));
    timStr += QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
    if (decimals)
        timStr += QString(".%1").arg(qRound((t - floor(t)) * 10.0), 1, 10);

    return timStr;
}

/** ********************
* 获取文件全路径
* @parm : file 文件绝对路径
***********************/
QString Functions::filePath(const QString &file)
{
    return file.left(file.lastIndexOf('/') + 1);
}

/**  ********************
* 获取文件扩展名
* @parm : file 文件绝对路径
***********************/
QString Functions::fileExt(const QString &file)
{
    const int idx = file.lastIndexOf('.');
    if (idx > -1)
        return file.mid(idx+1);
    return QString();
}

bool Functions::isKirinCpu()
{
    QProcess p;
    p.setProgram("lscpu");
    p.start();
    p.waitForReadyRead();
    QByteArray line = "123";
    bool is_kirin_cpu = false;
    while (line.length()) {
        line = p.readLine();
        if (QString(line).startsWith(QObject::tr("Model name")) && QString(line).indexOf("Kirin") >= 0) {
            is_kirin_cpu = true;
        }
    }
    p.waitForFinished();
    return is_kirin_cpu;
}

QString Functions::processComand(const QString &cmd)
{
    FILE *fp = popen(cmd.toStdString().c_str(), "r");
    char buffer[1024] = {0};
    QString result = QString::fromUtf8(fgets(buffer, sizeof(buffer), fp));
    pclose(fp);

    return result.replace("\n", "");
}

bool Functions::isQingsongDevice()
{
    return processComand("lsmod | grep qs | wc -l").toInt() > 0;
}

static inline void strstripspace_kv(char *str)
{
    if (strlen(str) == 0)
        return;
    char *startPos = str;
    while (*startPos != '\0' && isspace(*startPos))
        startPos++;
    if (*startPos == '\0') {
        str[0] = 0;
        return;
    }

    char *endPos = str + strlen(str) - 1;
    while (endPos != str && isspace(*endPos))
        endPos--;

    memmove(str, startPos, endPos - startPos + 1);
    *(str + (endPos - startPos) + 1) = 0;
}

static char* kdk_system_get_hostVirtType_kv()
{
    char *virtType = (char*)malloc(sizeof(char) * 65);
    assert(virtType);
#ifdef __linux__
    FILE *pipeLine = popen("systemd-detect-virt", "r");  // 建立流管道
    if (!pipeLine)
    {
        free(virtType);
        return NULL;
    }
    if (fgets(virtType, 64 * sizeof(char), pipeLine) == NULL)
    {
        free(virtType);
        pclose(pipeLine);
        return NULL;
    }
    pclose(pipeLine);
    strstripspace_kv(virtType);
    if (strcmp(virtType, "microsoft") == 0)
        strcpy(virtType, "hyper-v");
    else if (strcmp(virtType, "oracle") == 0)
        strcpy(virtType, "orcale virtualbox");
#endif
    return virtType;

}

bool Functions::isVirtualSuppotGpu()
{
    char* type = kdk_system_get_hostVirtType_kv();
    QString t = type;
    free(type);
    return t != "none";

    QString command = "lspci | grep -i vga";
    bool vgpu = false;

    QString result = processComand(command);
    if(result.contains("virtual"))
    {
        KyInfo() << "vga is virtual";
        vgpu = true;
    }

    command = "lspci | grep -i display";
    result = processComand(command);
    if(result.contains("Virtio GPU"))
    {
        KyInfo() << "display is virtio GPU";
        vgpu = true;
    }

    return vgpu;
}

QString Functions::humanSize(const qint64 &size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
    {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

int Functions::executeCMD(QString command, QString &result)
{
    QProcess p;
    QStringList args;
    args << "-c" << command;
    p.start("bash", args);
    p.waitForFinished();
    p.waitForReadyRead();
    result = p.readAll();
    return 0;
}

QString Functions::getVersion()
{
    QString version;
    executeCMD("dpkg -l lingmo-video | grep lingmo-video", version);
    QStringList fields = version.split(QRegularExpression("[ \t]+"));
    if (fields.size() >= 3)
        version = fields.at(2);
    else
        version = "none";

    return version;
}

bool Functions::isGreatwallDevice()
{
    QProcess p;
    p.start("cat /sys/class/dmi/id/modalias");
    p.waitForFinished();
    QString str(p.readAll());
    return str.indexOf("GreatWall", Qt::CaseInsensitive) > 0;
}
