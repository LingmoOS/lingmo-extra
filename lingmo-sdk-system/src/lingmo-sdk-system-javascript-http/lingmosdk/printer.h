#ifndef PRINTER_H
#define PRINTER_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include "lingmosdkdbus.h"

class Printer : public QWidget
{
    Q_OBJECT
public:
    explicit Printer(QWidget *parent = nullptr);
    Printer(QString urlPath);
    int dbusConnect;
    Kysdkdbus kdbus;
    QString ServerName ;
    QString ServerInterface;
    QString ServerPath;
    QJsonObject deal_printer();
    QJsonObject deal_printer_path();
    QJsonObject deal_printer_path_para();
private:
    QStringList path_list;
    QString path;

public slots:
    QStringList getPrinterList();
    QStringList getPrinterAvailableList();
    int setPrinterOptions(int num,QString type,QString cups,QString sides);
    int getPrinterPrintLocalFile(QString name,QString savepath);
    int getPrinterCancelAllJobs(QString name);
    int getPrinterStatus(QString name);
    QString getPrinterFilename(QString name);
    int getPrinterJobStatus(QString name,int jobid);
    int getPrinterDownloadRemoteFile(QString name,QString savepath);

signals:

};

#endif // PRINTER_H
