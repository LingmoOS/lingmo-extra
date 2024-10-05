#include <QCoreApplication>
#include "lingmosdkdbus.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDBusConnection connection = QDBusConnection::systemBus();
    if(!connection.registerService("com.lingmo.lingmosdk.service"))
    {
        qDebug() << "com.lingmo.lingmosdk.service 服务注册失败";
        return -1;
    }

    KySdkDisk disk;
    if(!connection.registerObject("/com/lingmo/lingmosdk/disk", &disk,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/disk 对象注册失败";
        return -1;
    }

    KySdkCpuInfo cpuinfo;
    if(!connection.registerObject("/com/lingmo/lingmosdk/cpuinfo", &cpuinfo,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/cpuinfo 对象注册失败";
        return -1;
    }

    KySdkNetCard netcard;
    if(!connection.registerObject("/com/lingmo/lingmosdk/netcard", &netcard,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/netcard 对象注册失败";
        return -1;
    }

    KySdkBios bios;
    if(!connection.registerObject("/com/lingmo/lingmosdk/bios", &bios,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/bios 对象注册失败";
        return -1;
    }

    KySdkMainBoard mainboard;
    if(!connection.registerObject("/com/lingmo/lingmosdk/mainboard", &mainboard,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/mainboard 对象注册失败";
        return -1;
    }

    KySdkPeripheralsEnum peripheralsenum;
    if(!connection.registerObject("/com/lingmo/lingmosdk/peripheralsenum", &peripheralsenum,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/peripheralsenum 对象注册失败";
        return -1;
    }

    KySdkPackageInfo packageinfo;
    if(!connection.registerObject("/com/lingmo/lingmosdk/packageinfo", &packageinfo,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/packageinfo 对象注册失败";
        return -1;
    }

    KySdkResource resource;
    if(!connection.registerObject("/com/lingmo/lingmosdk/resource", &resource,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/resource 对象注册失败";
        return -1;
    }

    KySdkProcess process;
    if(!connection.registerObject("/com/lingmo/lingmosdk/process", &process,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/process 对象注册失败";
        return -1;
    }
    
    KySdkResolution resolution;
    if(!connection.registerObject("/com/lingmo/lingmosdk/sysinfo", &resolution,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/sysinfo 对象注册失败";
        return -1;
    }

    KySdkNet net;
    if(!connection.registerObject("/com/lingmo/lingmosdk/net", &net,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/net 对象注册失败";
        return -1;
    }

    KySdkRunInfo runinfo;
    if(!connection.registerObject("/com/lingmo/lingmosdk/runinfo", &runinfo,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/runinfo 对象注册失败";
        return -1;
    }

    KySdkGps gps;
    if(!connection.registerObject("/com/lingmo/lingmosdk/gps", &gps,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/gps 对象注册失败";
        return -1;
    }

    KySdkPrint print;
    if(!connection.registerObject("/com/lingmo/lingmosdk/print", &print,
                                  QDBusConnection::ExportAllContents))
    {
        qDebug() << "/com/lingmo/lingmosdk/print 对象注册失败";
        return -1;
    }

    // QDBusConnection con = QDBusConnection::sessionBus();
    // KySdkRest res;
    // if(!con.registerObject("/com/lingmo/lingmosdk/resolution", &res,
    //                               QDBusConnection::ExportAllContents))
    // {
    //     qDebug() << "/com/lingmo/lingmosdk/resolution 对象注册失败";
    //     return -1;
    // }

    return a.exec();
}
