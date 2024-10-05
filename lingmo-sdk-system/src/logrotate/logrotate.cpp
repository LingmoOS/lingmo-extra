#include "logrotate.h"
#include <QFile>
#include <QDir>
#include <QDebug>

#define DefaultRule "{\n\tdaily\n\trotate 5\n\tcompress\n\tdateext\n\tmissingok\n\tnotifempty\n\tcopytruncate\n\tsu root root\n}\n"

LogRotate::LogRotate(QObject *parent) : QObject(parent)
{
}

int LogRotate::setConfig(QString path)
{
    qDebug() << "begin";
    int ret = -1;
    QString logfile = path.mid(path.lastIndexOf('/') + 1);
    if(!logfile.endsWith(".log"))
        return ret;

    QString fileName = logfile.left(logfile.length() - 4);
    QDir dir("/etc/logrotate.d/kylog-rotate");
    if (!dir.exists())
        dir.mkdir("/etc/logrotate.d/kylog-rotate");
    QFile file(QString("/etc/logrotate.d/kylog-rotate/%1").arg(fileName));
    do
    {

        bool exist = file.exists();

        if(!file.open(QFile::ReadWrite | QFile::Text))
        {
            qDebug() << "打开文件失败";
            break;
        }

        QString conf;
        if(exist)
        {
            conf = file.readAll();
            if(!conf.contains(path))
                conf = QString("%1\n%2").arg(path).arg(conf).toStdString().c_str();
        }
        else
            conf = QString("%1\n%2").arg(path).arg(DefaultRule).toStdString().c_str();
        file.reset();
        file.write(conf.toLatin1());
        ret = 0;
    } while (0);

    file.close();
    qDebug() << "end";
    return ret;
}
