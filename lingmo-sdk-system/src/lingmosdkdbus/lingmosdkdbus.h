#ifndef KYSDKDBUS_H
#define KYSDKDBUS_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QtDebug>
#include <string.h>

#include <memory>

#include "../systeminfo/libkysysinfo.h"     // 分辨率
#include "../hardware/libkync.h"         // 网卡
#include "../hardware/libkybios.h"         // BIOS
#include "../disk/libkydiskinfo.h"      // 磁盘
#include "../proc/libkyrtinfo.h"        // 进程
#include "../proc/libkyprocess.h"      // 进程
#include "../hardware/libkyboard.h"        // 主板
#include "../net/libkynetinfo.h"       // 网络
#include "../realtime/libkyrealtimeinfo.h"  // 运行时信息
#include "../hardware/libkyusb.h"           // USB外设
#include "../hardware/libkycpu.h"           //CPU
#include "../hardware/libkyprinter.h"       //打印
#include "../location/libkylocation.h"      //地理位置
#include "../packages/libkypackages.h"        //包

// 返回磁盘类型（机械/固态），额定转速，固件版本信息?，扇区数量
class KySdkDisk : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.disk")
public:
    explicit KySdkDisk();
    ~KySdkDisk();
public slots:
    QStringList getDiskList() const;
    unsigned int getDiskSectorSize(const QString diskname) const;
    double getDiskTotalSizeMiB(const QString diskname) const;
    QString getDiskModel(const QString diskname) const;
    QString getDiskSerial(const QString diskname) const;
    unsigned int getDiskPartitionNums(const QString diskname) const;
    QString getDiskType(const QString diskname) const;
    QString getDiskVersion(const QString diskname) const;
    unsigned long long getDiskSectorNum(const QString diskname) const;
};

// 获取CPU架构、制造厂商、型号、额定主频、核心数量、虚拟化的支持、线程数
class KySdkCpuInfo : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.cpuinfo")
public:
    explicit KySdkCpuInfo();
    ~KySdkCpuInfo();
public slots:
    QString getCpuArch() const;
    QString getCpuVendor() const;
    QString getCpuModel() const;
    QString getCpuFreqMHz() const;
    unsigned int getCpuCorenums() const;
    QString getCpuVirt() const;
    unsigned int getCpuProcess() const;
};

// 返回网卡驱动、型号、设备类型、制造商
class KySdkNetCard : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.netcard")
public:
    explicit KySdkNetCard();
    ~KySdkNetCard();
public slots:
    QStringList getNetCardName() const;
    int getNetCardUp(const QString netCardName) const;
    QStringList getNetCardUpcards() const;
    QString getNetCardPhymac(const QString netCardName) const;
    QString getNetCardPrivateIPv4(const QString netCardName) const;
    QStringList getNetCardIPv4(const QString netCardName) const;
    QString getNetCardPrivateIPv6(const QString netCardName) const;
    int getNetCardType(const QString netCardName) const;
    QStringList getNetCardProduct(const QString netCardName) const;
    QStringList getNetCardIPv6(const QString netCardName) const;
};

// 返回BIOS厂商及版本号
class KySdkBios : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.bios")
public:
    explicit KySdkBios();
    ~KySdkBios();
public slots:
    QString getBiosVendor() const;
    QString getBiosVersion() const;
};

// 返回主板型号、发布日期、序列号、厂商
class KySdkMainBoard : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.mainboard")
public:
    explicit KySdkMainBoard();
    ~KySdkMainBoard();
public slots:
    QString getMainboardName() const;
    QString getMainboardDate() const;
    QString getMainboardSerial() const;
    QString getMainboardVendor() const;
};

// 获取所有usb设备的：名称、类型、PID、VID、序列号、状态
class KySdkPeripheralsEnum : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.peripheralsenum")
public:
    explicit KySdkPeripheralsEnum();
public slots:
    QStringList getAllUsbInfo() const;
};

// 获取所有包列表、系统中指定包的版本号、检测指定包名的软件包是否正确安装
class KySdkPackageInfo : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.packageinfo")
public:
    explicit KySdkPackageInfo();
public slots:
    QStringList getPackageList() const;
    QString getPackageVersion(const QString packageName) const;
    int getPackageInstalled(const QString packageName, const QString version) const;
};

// 系统中物理内存总大小、物理内存使用率、物理内存使用大小、实际可用的物理内存大小
// 实际空闲的物理内存大小、所有应用申请的虚拟内存总量、Swap分区总大小、Swap分区使用率
// Swap分区使用量、Swap分区空闲大小、CPU瞬时使用率、开机天数
class KySdkResource : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.resource")
public:
    explicit KySdkResource();
public slots:
    double getMemTotalKiB() const;
    double getMemUsagePercent() const;
    double getMemUsageKiB() const;
    double getMemAvailableKiB() const;
    double getMemFreeKiB() const;
    double getMemVirtAllocKiB() const;
    double getMemSwapTotalKiB() const;
    double getMemSwapUsagePercent() const;
    double getMemSwapUsageKiB() const;
    double getMemSwapFreeKiB() const;
    double getCpuCurrentUsage() const;
    QString getUpTime() const;
};

// 返回进程相关信息
    // CPU使用率、内存占用率、进程状态、使用的端口号
    // 启动时间、CPU时间、cmd、进程属主
class KySdkProcess : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.process")
public:
    explicit KySdkProcess();
public slots:
    double getProcInfoCpuUsage(int pid) const;
    double getProcInfoMemUsage(int pid) const;
    QString getProcInfoStatus(int pid) const;
    int getProcInfoPort(int pid) const;
    QString getProcInfoStartTime(int pid) const;
    QString getProcInfoRunningTime(int pid) const;        
    QString getProcInfoCpuTime(int pid) const;       
    QString getProcInfoCmd(int pid) const;
    QString getProcInfoUser(int pid) const;
    QStringList getProcInfo(const QString procName) const;
    QStringList getAllProcInfo() const;
};

// 获取屏幕以及屏幕支持的分辨率，获取屏幕设置的分辨率
class KySdkResolution : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.sysinfo")
public:
    explicit KySdkResolution();
public slots:
    QString getSystemArchitecture() const;
    QString getSystemName() const;
    QString getSystemVersion(bool verbose) const;
    int getSystemActivationStatus() const;
    QString getSystemSerialNumber() const;
    QString getSystemKernelVersion() const;
    QString getSystemEffectUser() const;
    QString getSystemProjectName() const;
    QString getSystemProjectSubName() const;
    unsigned int getSystemProductFeatures() const;
    QString getSystemHostVirtType() const;
    QString getSystemHostCloudPlatform() const;
    QString getSystemOSVersion() const;
    QString getSystemUpdateVersion() const;
    bool getSystemIsZYJ() const;
};

// 获取端口状态，网关信息，防火墙状态
class KySdkNet : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.net")
public:
    explicit KySdkNet();
public slots:
    int getPortState(int port) const;
    QStringList getMultiplePortStat(int start, int end) const;
    QStringList getGatewayInfo() const;
    QStringList getFirewallState() const;
};

// 返回I/O读写速率，瞬时网速，主板温度，CPU温度，硬盘温度
class KySdkRunInfo : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.runinfo")
public:
    explicit KySdkRunInfo();
public slots:
    double getNetSpeed(const QString nc) const;
    int getDiskRate(const QString diskpath) const;
    double getCpuTemperature() const;
    double getDiskTemperature(const QString diskpath) const;
};

// 返回国家城市经纬度
class KySdkGps : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.gps")
public:
    explicit KySdkGps();
public slots:
    QString getGPSInfo() const;
private:
};

class KySdkPrint : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.print")
public:
    explicit KySdkPrint();
public slots:
    QStringList getPrinterList() const;
    QStringList getPrinterAvailableList() const;
    void setPrinterOptions(int number_up, const QString media, const QString number_up_layout, const QString sides) const;
    int getPrinterPrintLocalFile (const QString printername, const QString filepath) const;
    int getPrinterCancelAllJobs(const QString printername) const;
    int getPrinterStatus(const QString printername) const;
    QString getPrinterFilename(const QString url) const;
    int getPrinterJobStatus(const QString printername, int jobid) const;
    int getPrinterDownloadRemoteFile(const QString url, const QString filepath) const;
};

class KySdkRest : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.lingmo.lingmosdk.resolution")
public:
    explicit KySdkRest();
    ~KySdkRest();
public slots:
    QStringList getSysLegalResolution() const;
};

#endif // KYSDKDBUS_H
