#include <QCoreApplication>
#include <QDBusConnection>
#include "logrotate.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDBusConnection conn = QDBusConnection::systemBus();

    conn.registerService("com.lingmosdk.base");

    LogRotate logRotate;

    conn.registerObject("/com/lingmosdk/base/logrotate",&logRotate,QDBusConnection::ExportAllSlots);

    return a.exec();
}
