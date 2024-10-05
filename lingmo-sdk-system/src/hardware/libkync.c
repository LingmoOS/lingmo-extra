#define _GNU_SOURCE     # required for NI_NUMERICHOST
#include "libkync.h"
#include <cstring-extension.h>
#include <errno.h>
#include <kerr.h>
#include <libkylog.h>
#include <stdio.h>
#include <string.h>
#include <lingmosdk/lingmosdk-base/sdkmarcos.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <dirent.h>
#include <glib.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

enum cardspec{
    NCSPEC_ALL,
    NCSPEC_UP,
    NCSPEC_DOWN
};

enum cardstat{
    NCSTAT_LINK_UP,
    NCSTAT_LINK_DOWN,
    NCSTAT_ERROR
};

enum cardcfg{
    NCCFG_MAC,
    NCCFG_IPv4,
    NCCFG_IPv6,
    NCCFG_IPv4_MASK,
    NCCFG_IPv6_MASK
};

#define NC_IPv4_SIZE    16
#define NC_MAC_SIZE     18
#define SIOCGIWNAME     0x8B01
#define PCIID_PATH "/usr/share/misc/pci.ids"

static GKeyFile *
configure_open_file(const gchar *filename)
{
    GError *err = NULL;
    GKeyFile *keyfile;
    keyfile = g_key_file_new();
    g_key_file_set_list_separator(keyfile, ',');
    if (!g_key_file_load_from_file(keyfile, filename, G_KEY_FILE_NONE, &err))
    {
        g_error_free(err);
        g_key_file_free(keyfile);
        return NULL;
    }
    return keyfile;
}

static void
configure_read_string(GKeyFile *keyfile, GString *ret, gchar *sec, gchar *key, gchar *default_val)
{
    GError *err = NULL;
    gchar *val = g_key_file_get_string(keyfile,
                                       sec,
                                       key,
                                       &err);
    if (err)
    {
        g_string_assign(ret, default_val);
        g_error_free(err);
        return;
    }
    g_string_assign(ret, val);
    g_free(val);
}

static char **
get_ip_list(enum cardcfg type,const char *nc)
{
    int count = 0;
    char curAddr[64] = "\0";
    char **result = NULL;
    char **res = NULL;
    DIR *dir = NULL;
    struct dirent *file;
    char *path = "/etc/NetworkManager/system-connections/";
    char *value = NULL;
    if (type == NCCFG_IPv4)
    {
        value = "ipv4";
        if (kdk_nc_get_private_ipv4(nc))
            strcpy(curAddr, kdk_nc_get_private_ipv4(nc));
    }
    else if(type == NCCFG_IPv6)
    {
        value = "ipv6";
        if (kdk_nc_get_private_ipv6(nc))
            strcpy(curAddr, kdk_nc_get_private_ipv6(nc));
    }
    else
        value = NULL;

    if ((dir = opendir(path)) == NULL)
    {
        klog_err("opendir %s failed!", dir);
        return NULL;
    }

    //网卡当前地址加入返回列表
    if (curAddr && strcmp(curAddr,""))
    {
        result = (char **)calloc(++count + 1, sizeof(char *));
        if(!result)
            goto err_out;
        result[count - 1] = (char *)calloc(strlen(curAddr) + 1, sizeof(char));
        if(!result[count - 1])
            goto err_out;
        strncpy(result[count - 1], curAddr, strlen(curAddr));
    }

    while (file = readdir(dir))
    {
        // 判断是否为文件
        if (file->d_type != 8)
            continue;
        // 为文件加上相对路径
        char fileName[512] = "\0";
        strncpy(fileName, path, strlen(path));
        strcat(fileName, file->d_name);

        GKeyFile *keyFile = configure_open_file(fileName);
        if (NULL == keyFile)
        {
            klog_err("load config file %s failed\n", fileName);
            continue;
        }

        GString *val = g_string_new("");
        configure_read_string(keyFile, val, "connection", "interface-name", "null");

        if (0 == strcmp(val->str, nc))
        {
            configure_read_string(keyFile, val, value, "address1", "null");
            if (0 != strcmp(val->str, "null"))
            {
                int i = -1; 
                while(val->str[++i] != 0x2f); // 0x2f是'/'的ascii码值
                val->str[i] = '\0';
                if (0 != strcmp(curAddr, val->str))
                {
                    res = (char **)realloc(result, (++count + 1) * sizeof(char *));
                    if(!res)
                        goto err_out;
                    result = res;
                    result[count - 1] = (char *)calloc(i + 1, sizeof(char));
                    if(!result[count - 1])
                        goto err_out;
                    strncpy(result[count - 1], val->str, i);
                    result[count] = NULL;
                }
            }
        }
        g_string_free(val,TRUE);
        g_key_file_free(keyFile);
    }
    closedir(dir);
    return result;
err_out:
    while (result && count)
    {
        free(result[count - 1]);
        result[count - 1] = NULL;
    }
    free(result);
    closedir(dir);
    return NULL;
}

static enum cardstat _get_card_stat(const char *nc)
{
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (!sfd)
    {
        klog_err("网卡[%s]状态获取失败：%s\n", nc, strerror(errno));
        return NCSTAT_ERROR;
    }

    struct ifreq stIf;
    strcpy(stIf.ifr_ifrn.ifrn_name, nc);

    if (ioctl(sfd, SIOCGIFFLAGS, &stIf) < 0)
    {
        klog_err("网卡[%s]状态获取失败：%s\n", nc, strerror(errno));
        close (sfd);
        return NCSTAT_ERROR;
    }

    close(sfd);
    if (stIf.ifr_ifru.ifru_flags & IFF_RUNNING)
        return NCSTAT_LINK_UP;

    return NCSTAT_LINK_DOWN;
}

static char **_get_cardlist(enum cardspec spec)
{
    char **cards = NULL;
    size_t cardnum = 0;
#ifdef __linux__
    FILE* fp = fopen("/proc/net/dev", "r");
    ASSERT_NOT_NULL(fp, NULL);
    char buffer[1024];
    while (fgets(buffer, 1024, fp))
    {
        if (strncmp(strskipblank(buffer), "Inter", 5) == 0 || strncmp(strskipblank(buffer), "face", 4) == 0)
            continue;
        int pos = strfirstof(buffer, ':');
        if (pos > 0)
        {
            buffer[pos] = 0;
            strstripblank(buffer);
            if (spec == NCSPEC_UP)
            {
                if (_get_card_stat(buffer) != NCSTAT_LINK_UP)
                    continue;
            }
            else if (spec == NCSPEC_DOWN)
            {
                if (_get_card_stat(buffer) != NCSTAT_LINK_DOWN)
                    continue;
            }
            cardnum ++;
            char **tmp = realloc(cards, sizeof(char*) * cardnum);
            if (!tmp)
            {
                klog_err("内存申请失败：%s\n", strerror(errno));
                goto err_out;
            }
            cards = tmp;
            int index = cardnum - 1;
            cards[index] = malloc(sizeof(char) * (strlen(buffer) + 1));
            if (!cards[index])
            {
                klog_err("内存申请失败：%s\n", strerror(errno));
                goto err_out;
            }
            strcpy(cards[index], buffer);
        }
    }
    char **tmp = realloc(cards, sizeof(char *) * (cardnum + 1));
    if (!tmp)
    {
        klog_err("内存申请失败：%s\n", strerror(errno));
        goto err_out;
    }
    cards = tmp;
    cards[cardnum] = NULL;
    goto out;
#endif  // __linux__
err_out:
#ifdef __linux__
    fclose(fp);
#endif
    while (cardnum)
    {
        free(cards[cardnum - 1]);
        cardnum--;
    }
    SAFE_FREE(cards);
    return NULL;
out:
    fclose(fp);
    return cards;
}

char **_get_nc_cfg(const char *nc, enum cardcfg cfg)
{
    char **res = NULL;
    struct ifreq stIf;
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd < 0)
    {
        klog_err("网卡[%s]信息获取失败：%s\n", nc, strerror(errno));
        return NULL;
    }
    strncpy(stIf.ifr_ifrn.ifrn_name, nc, sizeof(stIf.ifr_ifrn.ifrn_name) - 1);

    switch (cfg)
    {
    case NCCFG_MAC:{
        if (ioctl(sfd, SIOCGIFHWADDR, &stIf) < 0)
        {
            klog_err("网卡[%s]MAC获取失败：%s\n", nc, strerror(errno));
            close(sfd);
            return NULL;
        }
        res = malloc(sizeof(char*));
        if (!res)
        {
            klog_err("内存申请失败:%s\n", strerror(errno));
            close(sfd);
            return NULL;
        }
        res[0] = malloc(sizeof(char) * NC_MAC_SIZE);
        if (!res[0])
        {
            free(res);
            klog_err("内存申请失败:%s\n", strerror(errno));
            close(sfd);
            return NULL;
        }
        snprintf(res[0], NC_MAC_SIZE, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)stIf.ifr_ifru.ifru_hwaddr.sa_data[0],
            (unsigned char)stIf.ifr_ifru.ifru_hwaddr.sa_data[1],
            (unsigned char)stIf.ifr_ifru.ifru_hwaddr.sa_data[2],
            (unsigned char)stIf.ifr_ifru.ifru_hwaddr.sa_data[3],
            (unsigned char)stIf.ifr_ifru.ifru_hwaddr.sa_data[4],
            (unsigned char)stIf.ifr_ifru.ifru_hwaddr.sa_data[5]);
    }break;
    case NCCFG_IPv4:{
        if (ioctl(sfd, SIOCGIFADDR, &stIf) < 0)
        {
            klog_err("网卡[%s]IPv4获取失败：%s\n", nc, strerror(errno));
            close(sfd);
            return NULL;
        }
        res = malloc(sizeof(char*));
        if (!res)
        {
            klog_err("内存申请失败:%s\n", strerror(errno));
            close(sfd);
            return NULL;
        }
        res[0] = malloc(sizeof(char) * NC_IPv4_SIZE);
        if (!res[0])
        {
            free(res);
            klog_err("内存申请失败:%s\n", strerror(errno));
            close(sfd);
            return NULL;
        }
        struct sockaddr_in sin;
        memcpy(&sin, &stIf.ifr_ifru.ifru_addr, sizeof(sin));
        snprintf(res[0], NC_IPv4_SIZE, "%s", inet_ntoa(sin.sin_addr));
    }break;
    case NCCFG_IPv6:{
        struct ifaddrs *ifap, *ifa;
        struct sockaddr_in6 *sa;
        char addr[INET6_ADDRSTRLEN] = {0};
        getifaddrs(&ifap);
        for (ifa = ifap; ifa; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr->sa_family == AF_INET6 && !strcmp(ifa->ifa_name, nc))
            {
                sa = (struct sockaddr_in6 *)ifa->ifa_addr;
                getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6), addr,
                            sizeof(addr), NULL, 0, NI_NUMERICHOST);
            }
        }

        res = malloc(sizeof(char*));
        if (!res)
        {
            klog_err("内存申请失败:%s\n", strerror(errno));
            close(sfd);
            freeifaddrs(ifap);
            return NULL;
        }
        res[0] = malloc(sizeof(char) * INET6_ADDRSTRLEN);
        if (!res[0])
        {
            free(res);
            klog_err("内存申请失败:%s\n", strerror(errno));
            close(sfd);
            freeifaddrs(ifap);
            return NULL;
        }

        int i = 0;
        while (addr[i] != '%' && addr[i] != '\0')
            i++;
        addr[i] = '\0';
        
        sprintf(res[0], "%s",addr);

        freeifaddrs(ifap);
    }break;
    default:
        break;
    }
    close(sfd);
    return res;
}

char** kdk_nc_get_cardlist()
{
    return _get_cardlist(NCSPEC_ALL);
}

int kdk_nc_is_up(const char *nc)
{
    if (!nc)
        return -1;
    enum cardstat stat = _get_card_stat(nc);
    if (stat == NCSTAT_LINK_UP)
        return 1;
    else if (stat == NCSTAT_LINK_DOWN)
        return 0;
    return -1;
}

char** kdk_nc_get_upcards()
{
    return _get_cardlist(NCSPEC_UP);
}

char* kdk_nc_get_phymac(const char *nc)
{
    if (!nc)
        return NULL;
    
    char **maclist = _get_nc_cfg(nc, NCCFG_MAC);
    if (!maclist)
        return NULL;
    char *mac = maclist[0];
    free(maclist);
    return mac;
}

char* kdk_nc_get_private_ipv4(const char *nc)
{
    if (!nc)
        return NULL;
    
    char **ipv4list = _get_nc_cfg(nc, NCCFG_IPv4);
    if (!ipv4list)
        return NULL;
    char *ipv4 = ipv4list[0];
    free(ipv4list);
    return ipv4;
}

char **kdk_nc_get_ipv4(const char *nc)
{
    if(!nc)
        return NULL;
    return get_ip_list(NCCFG_IPv4, nc);    
}

char* kdk_nc_get_private_ipv6(const char *nc)
{
    if (!nc)
        return NULL;

    char **ipv6list = _get_nc_cfg(nc, NCCFG_IPv6);
    if (!ipv6list)
        return NULL;
    char *ipv6 = ipv6list[0];
    free(ipv6list);
    return ipv6;
}

char **kdk_nc_get_ipv6(const char *nc)
{
    if (!nc)
        return NULL;
    return get_ip_list(NCCFG_IPv6, nc);
}

int kdk_nc_is_wireless(const char *nc)
{
    if (!nc)
        return -1;
    int i = 0, ret = -1;
    char **list = kdk_nc_get_cardlist();
    if(!list)
        goto out;
    while(list[i])
    {
        if(0 == strcmp(nc, list[i++]))
        {
            char buffer[2 * IFNAMSIZ + 1];
            int fd = socket(PF_INET, SOCK_DGRAM, 0);
            if (fd < 0)
                goto out;
            memset(buffer, 0, sizeof(buffer));
            strncpy(buffer, nc, sizeof(buffer));
            ret = ioctl(fd, SIOCGIWNAME, &buffer) == 0 ? 1 : 0;
            close(fd);
            break;
        }
    }
out:
    kdk_nc_freeall(list);
    return ret;
}

//根据mac地址获取从hwinfo --usb中获取指定网卡的vendor和device
static int __hwinfo_usb(const char * mac,char * vendor, char *product)
{
    FILE * fp = popen("hwinfo --usb","r");
    if(!fp)
        return -1;
    char line[1024] = "\0";
    char ven[256] = "\0";
    char pro[256] = "\0";
    int flags = 0;
    while(fgets(line,1024,fp))
    {
        int index = 0;
        if (flags && (0 < strlen(ven)) && (0 < strlen(pro)))
        {
            break;
        }

        if (strstr(line, "HW Address") && strstr(line, mac))
        {
            flags = 1;
        }
        
        if(strstr(line, "Vendor:"))
        {
            // 返回数据为Vendor: usb 0xxxxx "Linux Foundation"，取最后双引号中的内容 
            strcpy(ven, &line[22]);
        }

        if (strstr(line, "Device:"))
        {
            // 返回数据为Device: usb 0xxxxx "2.0 root hub"，取最后双引号中的内容
            strcpy(pro, &line[22]);
        }

        if(0 < sscanf(line,"%d:%*s",&index))
        {
            memset(ven,0,256);
            memset(pro,0,256);
        }
    }
    if(flags)
    {
        strncpy(vendor, ven, strlen(ven) - 2);
        strncpy(product, pro, strlen(pro) - 2);
    }
    fclose(fp);
    return 0;
}

int kdk_nc_get_vendor_and_product(const char *nc, char *vendor, char *product)
{
    if (!nc)
        return -1;
    char filename[64] = "0";
    sprintf(filename, "/sys/class/net/%s/device", nc);
    if (0 != access(filename, F_OK))
        return -1; //非物理网卡，可能是逻辑网卡如 lo: 本地环回
    FILE *fp = NULL;
    
    
    //usb网卡通过hwinfo获取
    char ncType[64] = "\0";
    sprintf(ncType, "%s/%s", filename, "uevent");
    if(NULL != (fp = fopen(ncType,"r")))
    {
        char text[1024] = "\0";
        if (0 > fread(text, sizeof(char), 1024, fp))
        {
            klog_err("open %s failed", ncType);
            return -1;
        }
        if (strstr(text, "usb_interface"))
        {
            char *mac = kdk_nc_get_phymac(nc);
            int ret = __hwinfo_usb(mac,vendor,product);
            free(mac);
            return ret;
        }
    }
    char vendorId[64] = "0";
    char productId[64] = "0";
    strcpy(vendorId, filename);
    strcpy(productId, filename);
    //读厂家id
    if (NULL != (fp = fopen(strcat(vendorId, "/vendor"), "r")))
    {
        fgets(vendorId, sizeof(vendorId), fp);
        fclose(fp);
    }
    //读设备id
    if (NULL != (fp = fopen(strcat(productId, "/device"), "r")))
    {
        fgets(productId, sizeof(vendorId), fp);
        fclose(fp);
    }

    long u[4] = {0,1,2,3}; // 0,1存储网卡的厂家和设备ID，2，3存储pci.ids文件读到的ID
    // memset(u, 0, sizeof(u));
    int count = sscanf(vendorId, "%lx", &u[0]);
    if(count < 1)
        return -1;
    sscanf(productId, "%lx", &u[1]);
    if (count < 1)
        return -1;

    char line[512] = "0";
    if (NULL != (fp = fopen(PCIID_PATH, "r")))
    {
        long unsigned int level = 0;
        while (fgets(line, sizeof(line), fp))
        {
            // 忽略空行和注释
            if (strlen(line) == 0 || line[0] == '#')
                continue;

            //行前无制表符代表厂家id，一个制表符代表设备id
            level = 0;
            while (level < strlen(line) && line[level] == '\t')
                level++;
            //获取厂家名称
            if (0 == level)
            {
                sscanf(line, "%lx", &u[2]);
                if (u[0] == u[2])
                    strcpy(vendor, line + level + 6);
            }
            //在对应的厂家id下查找设备id
            if (1 == level && u[0] == u[2])
            {
                sscanf(line, "%lx", &u[3]);
                if (u[1] == u[3])
                {
                    strcpy(product, line + level + 6);
                    break;
                }
            }
        }
        fclose(fp);
    }
    return 0;
}

char *kdk_nc_get_driver(const char *nc)
{
    int ret;
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct ethtool_drvinfo *driverInfo = (struct ethtool_drvinfo *)calloc(1, sizeof(struct ethtool_drvinfo));
    driverInfo->cmd = ETHTOOL_GDRVINFO;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, nc);
    ifr.ifr_data = (char *)driverInfo;
    ret = ioctl(fd, SIOCETHTOOL, &ifr);
    if (0 != ret)
        return NULL;
    char *driver = (char*)malloc(32);
    strcpy(driver,driverInfo->driver);
    free(driverInfo);
    return driver;
}

inline void kdk_nc_freeall(char **list)
{
    if (! list)
        return;
    size_t index = 0;
    while (list[index])
    {
        free(list[index]);
        index ++;
    }
    free(list);
}
