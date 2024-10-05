#include <QCoreApplication>
#include "lingmosdkrest.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDBusConnection con = QDBusConnection::sessionBus();
    KySdkRest res;
    if(!con.registerService("com.lingmo.lingmosdk.service"))
    {
        qDebug() << "com.lingmo.lingmosdk.service 服务注册失败";
        return -1;
    }

    if(!con.registerObject("/com/lingmo/lingmosdk/resolution", &res,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/resolution 对象注册失败";
        return -1;
    }

    return a.exec();
}
