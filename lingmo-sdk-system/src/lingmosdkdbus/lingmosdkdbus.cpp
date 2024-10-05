#include "lingmosdkdbus.h"
#include <QDBusReply>

KySdkDisk::KySdkDisk(){}

KySdkDisk::~KySdkDisk(){}

QStringList KySdkDisk::getDiskList() const
{
    QStringList res = {};
    char** disk = kdk_get_disklist();
    for (int i = 0; disk[i]; i++)
    {
        res << QString::fromLocal8Bit(disk[i]);
    }

    return res;
}

unsigned int KySdkDisk::getDiskSectorSize(const QString diskname) const
{
    std::string dn = diskname.toStdString();
    kdk_diskinfo* p = kdk_get_diskinfo(dn.c_str());
    if(p != NULL)
    {
        return p->sector_size;
    }else{
        return NULL;
    }
}

double KySdkDisk::getDiskTotalSizeMiB(const QString diskname) const
{
    std::string dn = diskname.toStdString();
    kdk_diskinfo* p = kdk_get_diskinfo(dn.c_str());
    if(p != NULL)
    {
        return static_cast<double>(p->total_size_MiB);
    }else{
        return NULL;
    }
}

QString KySdkDisk::getDiskModel(const QString diskname) const
{
    std::string dn = diskname.toStdString();
    kdk_diskinfo* p = kdk_get_diskinfo(dn.c_str());
    if(p != NULL)
    {
        return p->model;
    }else{
        return NULL;
    }
}

QString KySdkDisk::getDiskSerial(const QString diskname) const
{
    std::string dn = diskname.toStdString();
    kdk_diskinfo* p = kdk_get_diskinfo(dn.c_str());
    if(p != NULL)
    {
        return p->serial;
    }else{
        return NULL;
    }
}

unsigned int KySdkDisk::getDiskPartitionNums(const QString diskname) const
{
    std::string dn = diskname.toStdString();
    kdk_diskinfo* p = kdk_get_diskinfo(dn.c_str());
    if(p != NULL)
    {
        return p->partition_nums;
    }else{
        return NULL;
    }
}

QString KySdkDisk::getDiskType(const QString diskname) const
{
    std::string dn = diskname.toStdString();
    kdk_diskinfo* p = kdk_get_diskinfo(dn.c_str());
    if(p != NULL)
    {
        if(p->disk_type == DISK_TYPE_HDD) {
            return "DISK_TYPE_HDD";
        }
        else if(p->disk_type == DISK_TYPE_SSD) {
            return "DISK_TYPE_SSD";
        }
        else if (p->disk_type == DISK_TYPE_NONE)
        {
            return "DISK_TYPE_NONE";
        }
        else return "DISK_TYPE_OTHER";
    }else{
        return NULL;
    }
}

QString KySdkDisk::getDiskVersion(const QString diskname) const
{
    std::string dn = diskname.toStdString();
    kdk_diskinfo* p = kdk_get_diskinfo(dn.c_str());
    if(p != NULL)
    {
        return QString::fromLocal8Bit((p->fwrev)).remove("\n");
    }else{
        return NULL;
    }
}

unsigned long long KySdkDisk::getDiskSectorNum(const QString diskname) const
{
    std::string dn = diskname.toStdString();
    kdk_diskinfo* p = kdk_get_diskinfo(dn.c_str());
    if(p != NULL)
    {
        return p->sectors_num;
    }else{
        return NULL;
    }
}

KySdkCpuInfo::KySdkCpuInfo(){}

KySdkCpuInfo::~KySdkCpuInfo(){}

QString KySdkCpuInfo::getCpuArch() const
{
    return QString::fromLocal8Bit(kdk_cpu_get_arch());
}

QString KySdkCpuInfo::getCpuVendor() const
{
    return QString::fromLocal8Bit(kdk_cpu_get_vendor());
}

QString KySdkCpuInfo::getCpuModel() const
{
    return QString::fromLocal8Bit(kdk_cpu_get_model());
}

QString KySdkCpuInfo::getCpuFreqMHz() const
{
    return QString::fromLocal8Bit(kdk_cpu_get_freq_MHz());
}

unsigned int KySdkCpuInfo::getCpuCorenums() const
{
    return kdk_cpu_get_corenums();
}

QString KySdkCpuInfo::getCpuVirt() const
{
    return QString::fromLocal8Bit(kdk_cpu_get_virt());
}

unsigned int KySdkCpuInfo::getCpuProcess() const
{
    return kdk_cpu_get_process();
}

KySdkNetCard::KySdkNetCard(){}

KySdkNetCard::~KySdkNetCard(){}

QStringList KySdkNetCard::getNetCardName() const
{
    QStringList res = {};
    char** net = kdk_nc_get_cardlist();
    for (int i = 0; net[i]; i++)
    {
        res << QString::fromLocal8Bit(net[i]);
    }
    return res;
}

int KySdkNetCard::getNetCardUp(const QString netCardName) const
{
    std::string stdStr = netCardName.toStdString();
    return kdk_nc_is_up(stdStr.c_str());
}

QStringList KySdkNetCard::getNetCardUpcards() const
{
    QStringList res = {};
    char** upcards = kdk_nc_get_upcards();
    for (int i = 0; upcards[i]; i++)
    {
        res << QString::fromLocal8Bit(upcards[i]);
    }
    return res;
}

QString KySdkNetCard::getNetCardPhymac(const QString netCardName) const
{
    std::string stdStr = netCardName.toStdString();
    return QString::fromLocal8Bit(kdk_nc_get_phymac(stdStr.c_str()));
}

QString KySdkNetCard::getNetCardPrivateIPv4(const QString netCardName) const
{
    std::string stdStr = netCardName.toStdString();
    return QString::fromLocal8Bit(kdk_nc_get_private_ipv4(stdStr.c_str()));
}

QStringList KySdkNetCard::getNetCardIPv4(const QString netCardName) const
{
    QStringList res;
    char ** list = kdk_nc_get_ipv4(netCardName.toStdString().c_str());
    int i = 0;
    while(list && list[i])
    {
        res << list[i++];
    }
    return res;
}

QString KySdkNetCard::getNetCardPrivateIPv6(const QString netCardName) const
{
    std::string stdStr = netCardName.toStdString();
    return QString::fromLocal8Bit(kdk_nc_get_private_ipv6(stdStr.c_str()));
}

int KySdkNetCard::getNetCardType(const QString netCardName) const
{
    std::string stdStr = netCardName.toStdString();
    return kdk_nc_is_wireless(stdStr.c_str());
}

QStringList KySdkNetCard::getNetCardProduct(const QString netCardName) const
{
    std::string ncn = netCardName.toStdString();
    QStringList res = {};
    char vend[256] = "", prod[256] = "";
    if(kdk_nc_get_vendor_and_product(ncn.c_str(), vend, prod) == 0) {
        res << QString::fromLocal8Bit(vend).remove("\n");
        res << QString::fromLocal8Bit(prod).remove("\n");
    }
//    qFatal("kdk_nc_get_vendor_and_product 调用失败");
    return res;
}

QStringList KySdkNetCard::getNetCardIPv6(const QString netCardName) const
{
    QStringList res;
    char **list = kdk_nc_get_ipv6(netCardName.toStdString().c_str());
    int i = 0;
    while (list && list[i])
    {
        res << list[i++];
    }
    return res;
}

KySdkBios::KySdkBios(){}

KySdkBios::~KySdkBios(){}

QString KySdkBios::getBiosVendor() const
{
    return QString::fromLocal8Bit(kdk_bios_get_vendor()).remove("\n");
}

QString KySdkBios::getBiosVersion() const
{
    return QString::fromLocal8Bit(kdk_bios_get_version()).remove("\n");
}

KySdkMainBoard::KySdkMainBoard(){}

KySdkMainBoard::~KySdkMainBoard(){}

QString KySdkMainBoard::getMainboardName() const
{
   return QString::fromLocal8Bit(kdk_board_get_name()).remove("\n");
}

QString KySdkMainBoard::getMainboardDate() const
{
   return QString::fromLocal8Bit(kdk_board_get_date()).remove("\n");
}

QString KySdkMainBoard::getMainboardSerial() const
{
   return QString::fromLocal8Bit(kdk_board_get_serial()).remove("\n");
}

QString KySdkMainBoard::getMainboardVendor() const
{
   return QString::fromLocal8Bit(kdk_board_get_vendor()).remove("\n");
}

KySdkPeripheralsEnum::KySdkPeripheralsEnum(){}

QStringList KySdkPeripheralsEnum::getAllUsbInfo() const
{
    QStringList res = {};
    pDriverInfoList usb = kdk_usb_get_list();
    if(usb)
    {
        pDriverInfoList usb_list = usb;
        while(usb_list)
        {
            res << "name:" + QString::fromLocal8Bit(usb_list->data->name) + "," +
                   "type:" + QString::fromLocal8Bit(usb_list->data->type) + "," +
                   "pid:" + QString::fromLocal8Bit(usb_list->data->pid) + "," +
                   "vid:" + QString::fromLocal8Bit(usb_list->data->vid) + "," +
                   "serialNo:" + QString::fromLocal8Bit(usb_list->data->serialNo) + "," +
                   "devNode:" + QString::fromLocal8Bit(usb_list->data->devNode);
            usb_list = usb_list->next;
        }
    }
    return res;
}

KySdkPackageInfo::KySdkPackageInfo(){}

QStringList KySdkPackageInfo::getPackageList() const
{
    QStringList res = {};
    kdk_package_list *pack = kdk_package_get_packagelist();
    qDebug() << pack->nums;
    for (size_t i = 0; i < pack->nums; i ++)
    {
        res << "包名：" + QString::fromLocal8Bit(pack->list[i]->name) + "," +
               "版本号：" + QString::fromLocal8Bit(pack->list[i]->version) + "," +
               "包类型：" + QString::fromLocal8Bit(pack->list[i]->section) + "," +
               "包状态：" + QString::fromLocal8Bit(pack->list[i]->status) + "," +
               "包大小：" + QString::number(pack->list[i]->size_kb);
    }
    return res;
}

QString KySdkPackageInfo::getPackageVersion(const QString packageName) const
{
    std::string package = packageName.toStdString();
    return QString::fromLocal8Bit(kdk_package_get_version(package.c_str()));
}

int KySdkPackageInfo::getPackageInstalled(const QString packageName, const QString version) const
{
    std::string package = packageName.toStdString();
    std::string ver = version.toStdString();
    return kdk_package_is_installed(package.c_str(), ver.c_str());
}

KySdkResource::KySdkResource(){}

double KySdkResource::getMemTotalKiB() const
{
    return static_cast<double>(kdk_rti_get_mem_res_total_KiB());
}

double KySdkResource::getMemUsagePercent() const
{
    return static_cast<double>(kdk_rti_get_mem_res_usage_percent());
}

double KySdkResource::getMemUsageKiB() const
{
    return static_cast<double>(kdk_rti_get_mem_res_usage_KiB());
}

double KySdkResource::getMemAvailableKiB() const
{
    return static_cast<double>(kdk_rti_get_mem_res_available_KiB());
}

double KySdkResource::getMemFreeKiB() const
{
    return static_cast<double>(kdk_rti_get_mem_res_free_KiB());
}

double KySdkResource::getMemVirtAllocKiB() const
{
    return kdk_rti_get_mem_virt_alloc_KiB();
}

double KySdkResource::getMemSwapTotalKiB() const
{
    return static_cast<double>(kdk_rti_get_mem_swap_total_KiB());
}

double KySdkResource::getMemSwapUsagePercent() const
{
    return static_cast<double>(kdk_rti_get_mem_swap_usage_percent());
}

double KySdkResource::getMemSwapUsageKiB() const
{
    return static_cast<double>(kdk_rti_get_mem_swap_usage_KiB());
}

double KySdkResource::getMemSwapFreeKiB() const
{
    return static_cast<double>(kdk_rti_get_mem_swap_free_KiB());
}

double KySdkResource::getCpuCurrentUsage() const
{
    return static_cast<double>(kdk_rti_get_cpu_current_usage());
}

QString KySdkResource::getUpTime() const
{
    unsigned int day, hour, min, sec;
    if(kdk_rti_get_uptime(&day, &hour, &min, &sec) == 0)
    {
         return QString::number(day) + "天" + QString::number(hour) + "小时" + QString::number(min) + "分" + QString::number(sec) + "秒";
    }

}

KySdkProcess::KySdkProcess(){}

double KySdkProcess::getProcInfoCpuUsage(int pid) const
{
    return static_cast<double>(kdk_get_process_cpu_usage_percent(pid));
}

double KySdkProcess::getProcInfoMemUsage(int pid) const
{
    return static_cast<double>(kdk_get_process_mem_usage_percent(pid));
}

QString KySdkProcess::getProcInfoStatus(int pid) const
{
    return QString::fromLocal8Bit(kdk_get_process_status(pid));
}

int KySdkProcess::getProcInfoPort(int pid) const
{
    return kdk_get_process_port(pid);
}

QString KySdkProcess::getProcInfoStartTime(int pid) const
{
    return QString::fromLocal8Bit(kdk_get_process_start_time(pid)).remove("\n");
}

QString KySdkProcess::getProcInfoRunningTime(int pid) const
{
    return QString::fromLocal8Bit(kdk_get_process_running_time(pid));
}

QString KySdkProcess::getProcInfoCpuTime(int pid) const
{
    return QString::fromLocal8Bit(kdk_get_process_cpu_time(pid));
}

QString KySdkProcess::getProcInfoCmd(int pid) const
{
    return QString::fromLocal8Bit(kdk_get_process_command(pid));
}

QString KySdkProcess::getProcInfoUser(int pid) const
{
    return QString::fromLocal8Bit(kdk_get_process_user(pid));
}

QStringList KySdkProcess::getProcInfo(const QString procName) const
{
    std::string pN = procName.toStdString();
    QStringList res = {};
    char **proc = kdk_procname_get_process_infomation(const_cast<char*>(pN.c_str()));
    size_t count = 0;
    while (proc[count])
    {
        res << QString::fromLocal8Bit(proc[count]).remove("\n");
        count++;
    }
    kdk_proc_freeall(proc);
    return res;
}

QStringList KySdkProcess::getAllProcInfo() const
{
    QStringList res = {};
    char** proc_info = kdk_get_process_all_information();
    size_t count = 0;
    while (proc_info[count])
    {
        res << QString::fromLocal8Bit(proc_info[count]).remove("\n");
        count++;
    }
    kdk_proc_freeall(proc_info);
    return res;
}

KySdkResolution::KySdkResolution(){}

QString KySdkResolution::getSystemArchitecture() const
{
    return QString::fromLocal8Bit(kdk_system_get_architecture());
}

QString KySdkResolution::getSystemName() const
{
    return QString::fromLocal8Bit(kdk_system_get_systemName());
}

QString KySdkResolution::getSystemVersion(bool verbose) const
{
    return QString::fromLocal8Bit(kdk_system_get_version(verbose));
}

int KySdkResolution::getSystemActivationStatus() const
{
    int p,d;
    return kdk_system_get_activationStatus(&p, &d);
}

QString KySdkResolution::getSystemSerialNumber() const
{
    return QString::fromLocal8Bit(kdk_system_get_serialNumber()).remove("\n");;
}

QString KySdkResolution::getSystemKernelVersion() const
{
    return QString::fromLocal8Bit(kdk_system_get_kernelVersion()).remove("\n");
}

QString KySdkResolution::getSystemEffectUser() const
{
    return QString::fromLocal8Bit(kdk_system_get_eUser());
}

QString KySdkResolution::getSystemProjectName() const
{
    return QString::fromLocal8Bit(kdk_system_get_projectName());
}

QString KySdkResolution::getSystemProjectSubName() const
{
    return QString::fromLocal8Bit(kdk_system_get_projectSubName());
}

unsigned int KySdkResolution::getSystemProductFeatures() const
{
    return kdk_system_get_productFeatures();
}

QString KySdkResolution::getSystemHostVirtType() const
{
    return QString::fromLocal8Bit(kdk_system_get_hostVirtType());
}

QString KySdkResolution::getSystemHostCloudPlatform() const
{
    return QString::fromLocal8Bit(kdk_system_get_hostCloudPlatform());
}

QString KySdkResolution::getSystemOSVersion() const
{
    version_t ver = kdk_system_get_version_detaile();
    return QString::fromLocal8Bit(ver.os_version);
}

QString KySdkResolution::getSystemUpdateVersion() const
{
    version_t ver = kdk_system_get_version_detaile();
    return QString::fromLocal8Bit(ver.update_version);
}

bool KySdkResolution::getSystemIsZYJ() const
{
    return kdk_system_is_zyj();
}

KySdkNet::KySdkNet(){}

int KySdkNet::getPortState(int port) const
{
    return kdk_net_get_port_stat(port);
}

QStringList KySdkNet::getMultiplePortStat(int start, int end) const
{
    QStringList res = {};
    if(end < start)
        return res;
    int result[end - start + 1];
    if(0 == kdk_net_get_multiple_port_stat(start, end, result))
    {
        size_t count = 0;
        int num = end - start + 1;
        for(;count < num; count++)
        {
            res << QString::number(result[count]);
        }
    }
    return res;
}

QStringList KySdkNet::getGatewayInfo() const
{
    prouteMapList p = kdk_net_get_route();
    QStringList res = {};
    if(p == NULL)
    {
         return res;
    }
    while(p) {
        res << QString(QLatin1String(p->name)) << QString(QLatin1String(p->addr));
        p = p->next;
    }
    return res;
}

QStringList KySdkNet::getFirewallState() const
{
    QStringList res = {};
    pChain chain = kdk_net_get_iptable_rules();
    pChain tmpchain = chain;
    while(tmpchain)
    {
        pChainData tmpData = tmpchain->data;
        res << "total:" + QString::fromLocal8Bit(tmpchain->total) + "," +
               "policy:" + QString::fromLocal8Bit(tmpchain->policy);
        if(tmpData)
        {
            res << "target:" + QString::fromLocal8Bit(tmpData->target) + "," +
                   "prot:" + QString::fromLocal8Bit(tmpData->prot) + "," +
                   "opt:" + QString::fromLocal8Bit(tmpData->opt) + "," +
                   "source:" + QString::fromLocal8Bit(tmpData->source)+ "," +
                   "destination:" + QString::fromLocal8Bit(tmpData->destination) + "," +
                   "option:" + QString::fromLocal8Bit(tmpData->option);
        }
        tmpchain = tmpchain->next;
    }
    return res;     
}

KySdkRunInfo::KySdkRunInfo(){}

double KySdkRunInfo::getNetSpeed(const QString nc) const
{
    std::string dp = nc.toStdString();
    return static_cast<double>(kdk_real_get_net_speed(dp.c_str()));
}

int KySdkRunInfo::getDiskRate(const QString diskpath) const
{
    std::string dp = diskpath.toStdString();
    return kdk_real_get_disk_rate(dp.c_str());
}

double KySdkRunInfo::getCpuTemperature() const
{
    return static_cast<double>(kdk_real_get_cpu_temperature());
}
double KySdkRunInfo::getDiskTemperature(const QString diskpath) const
{
    std::string dp = diskpath.toStdString();
    return static_cast<double>(kdk_real_get_disk_temperature(dp.c_str()));
}

KySdkGps::KySdkGps(){}
QString KySdkGps::getGPSInfo() const
{
    return QString::fromLocal8Bit(kdk_loaction_get());
}

KySdkPrint::KySdkPrint(){}

QStringList KySdkPrint::getPrinterList() const
{
    QStringList res = {};
    char **printers = kdk_printer_get_list();
    size_t index = 0;
    while (printers[index])
    {
        res << QString::fromLocal8Bit(printers[index]);
        index ++;
    }
    return res;
}

QStringList KySdkPrint::getPrinterAvailableList() const
{
    QStringList res = {};
    char **printers = kdk_printer_get_available_list();
    size_t index = 0;
    while (printers[index])
    {
        res << QString::fromLocal8Bit(printers[index]);
        index ++;
    }
    return res;
}

void KySdkPrint::setPrinterOptions(int number_up, const QString media, const QString number_up_layout, const QString sides) const
{
    std::string med = media.toStdString();
    std::string nul = number_up_layout.toStdString();
    std::string sid = sides.toStdString();
    kdk_printer_set_options(number_up, med.c_str(), nul.c_str(), sid.c_str());
}

int KySdkPrint::getPrinterPrintLocalFile (const QString printername, const QString filepath) const
{
    std::string printers = printername.toStdString();
    std::string fp = filepath.toStdString();
    return kdk_printer_print_local_file(printers.c_str(), fp.c_str());
}

int KySdkPrint::getPrinterCancelAllJobs(const QString printername) const
{
    std::string printers = printername.toStdString();
    return kdk_printer_cancel_all_jobs(printers.c_str());
}

int KySdkPrint::getPrinterStatus(const QString printername) const
{
    std::string printers = printername.toStdString();
    return kdk_printer_get_status(printers.c_str());
}

QString KySdkPrint::getPrinterFilename(const QString url) const
{
    std::string urlpath = url.toStdString();
    
    return QString::fromLocal8Bit(kdk_printer_get_filename(urlpath.c_str()));
}
int KySdkPrint::getPrinterJobStatus(const QString printername, int jobid) const
{
    std::string printers = printername.toStdString();
    return kdk_printer_get_job_status(printers.c_str(), jobid);
}

int KySdkPrint::getPrinterDownloadRemoteFile(const QString url, const QString filepath) const
{
    std::string urlpath = url.toStdString();
    std::string fp = filepath.toStdString();
    return kdk_printer_print_download_remote_file(urlpath.c_str(), fp.c_str());
}

KySdkRest::KySdkRest(){}

KySdkRest::~KySdkRest(){}

QStringList KySdkRest::getSysLegalResolution() const
{
    QStringList res = {};
    char **power = kdk_system_get_resolving_power();
    size_t count = 0;
    while(power[count])
    {
        res << QString::fromLocal8Bit(power[count]);
        count ++;
    }
    kdk_resolving_freeall(power);
    return res;
}
