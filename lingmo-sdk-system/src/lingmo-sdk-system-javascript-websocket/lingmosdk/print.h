#ifndef PRINT_H
#define PRINT_H

#include <QWidget>
#include "lingmosdkdbus.h"

class Print : public QObject
{
    Q_OBJECT
public:
    explicit Print(QObject *parent = nullptr);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;


public slots:
    QStringList getPrinterList(void);
    QStringList getPrinterAvailableList(void);
    void setPrinterOptions(int,QString,QString,QString);
    int getPrinterPrintLocalFile(QString,QString);
    int getPrinterCancelAllJobs(QString);
    int getPrinterStatus(QString);
    QString getPrinterFilename(QString);
    int getPrinterJobStatus(QString,int);
    int getPrinterDownloadRemoteFile(QString,QString);

signals:
    void sendText(const QString &text);
};

#endif // PRINT_H