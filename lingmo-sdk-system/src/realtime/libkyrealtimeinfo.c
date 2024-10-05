#include "libkyrealtimeinfo.h"
#include "fcntl.h"
#include "stdio.h"
#include "net/if.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "ifaddrs.h"
#include "sys/types.h"

struct if_data
{
    /*  generic interface information */
    unsigned int ifi_opackets; /*  packets sent on interface */
    unsigned int ifi_ipackets; /*  packets received on interface */
    unsigned int ifi_ibytes;   /*  total number of octets received */
    unsigned int ifi_obytes;   /*  total number of octets sent */
};

struct if_info
{
    char ifi_name[64];
    unsigned long ifi_ibytes;
    unsigned long ifi_obytes;
};
struct if_speed
{
    char ifs_name[64];
    unsigned long ifs_ispeed;
    unsigned long ifs_ospeed;
    unsigned long ifs_us;
};

static int get_if_dbytes(struct if_info *ndev)
{
    if (!ndev)
        return -1;

    struct ifaddrs *ifa_list = NULL;

    struct ifaddrs *ifa = NULL;

    struct if_data *ifd = NULL;

    int ret = 0;

    ret = getifaddrs(&ifa_list);

    if (ret < 0)
    {

        perror("Get Interface Address Fail:");

        goto end;
    }

    for (ifa = ifa_list; ifa; ifa = ifa->ifa_next)
    {

        if (!(ifa->ifa_flags & IFF_UP) && !(ifa->ifa_flags & IFF_RUNNING))

            continue;

        if (ifa->ifa_data == 0)

            continue;

        ret = strcmp(ifa->ifa_name, ndev->ifi_name);

        if (ret == 0)
        {

            ifd = (struct if_data *)ifa->ifa_data;

            ndev->ifi_ibytes = ifd->ifi_ibytes;

            ndev->ifi_obytes = ifd->ifi_obytes;

            break;
        }
    }

    freeifaddrs(ifa_list);

end:
    return (ret ? -1 : 0);
}

static int get_if_speed(struct if_speed *ndev)
{
    if (!ndev)
        return -1;

    struct if_info *p1 = NULL, *p2 = NULL;

    p1 = (struct if_info *)malloc(sizeof(struct if_info));

    p2 = (struct if_info *)malloc(sizeof(struct if_info));

    bzero(p1, sizeof(struct if_info));

    bzero(p2, sizeof(struct if_info));

    strncpy(p1->ifi_name, ndev->ifs_name, strlen(ndev->ifs_name));

    strncpy(p2->ifi_name, ndev->ifs_name, strlen(ndev->ifs_name));

    int ret = 0;

    ret = get_if_dbytes(p1);

    if (ret < 0)
        goto end;

    usleep(ndev->ifs_us);

    ret = get_if_dbytes(p2);

    if (ret < 0)
        goto end;

    ndev->ifs_ispeed = p2->ifi_ibytes - p1->ifi_ibytes;

    ndev->ifs_ospeed = p2->ifi_obytes - p1->ifi_obytes;

end:

    free(p1);

    free(p2);

    return 0;
}

float kdk_real_get_net_speed(const char *nc)
{
    if(!nc)
        return -1;
    struct if_speed ndev;
    memset(&ndev, 0, sizeof(struct if_speed));

    int ret = 0;
    memset(&ndev, 0, sizeof(ndev));
    sprintf(ndev.ifs_name, "%s", nc);
    ndev.ifs_us = 1000000;
    ret = get_if_speed(&ndev);
    if (ret < 0)
        return -1;
    return ndev.ifs_ospeed * 1.0 / (ndev.ifs_us / 1000 * 0.001) / 1024.0f;
}

double kdk_real_get_cpu_temperature()
{
    char tmp[1024] = "\0";
    char path[64] = "\0";
    for (int i = 0;; i++)
    {
        memset(path, 0, 64);
        snprintf(path, 64, "/sys/class/hwmon/hwmon%d", i);
        if (0 != access(path, F_OK))
        {
            snprintf(path, 64, "/sys/class/hwmon/hwmon0/temp1_input");
            break;
        }
        snprintf(path, 64, "/sys/class/hwmon/hwmon%d/name", i);
        if (0 != access(path, F_OK))
            continue;
        snprintf(path, 64, "/sys/class/hwmon/hwmon%d/temp1_input", i);
        if (0 != access(path, F_OK))
            continue;
        break;
    }
    double temp = 0.0;
    FILE *fp = fopen(path, "r");
    if (fp)
    {
        fgets(tmp, sizeof(tmp), fp);
        fclose(fp);
        temp = atof(tmp);
        return temp < 100 ? temp : temp / 1000;
    }
    if(fabs(temp - 0.0) < 0.000001) 
    {
        fp = popen("sensors","r");
        if(fp)
        {
            while (fgets(tmp, sizeof(tmp), fp))
            {
                if (0 == strncmp(tmp, "Core", 4))
                {
                    char *pos = tmp, *start = NULL;
                    while (*pos != '+' && *pos != '\0')
                    {
                        pos++;
                        continue;
                    }
                    start = pos++;
                    while (*pos != '\0')
                    {
                        if(*pos == 0xc2 && *(pos + 1) == 0xb0) //字符"°"
                            break;
                        pos++;
                    }
                    *pos = '\0';
                    temp = atof(start);
                    fclose(fp);
                    return temp;
                }
            }
            fclose(fp);
        }
    }
    return -1;
}

int kdk_real_get_disk_temperature(const char *name)
{
    char cmd[128] = {"smartctl -A "};
    char tmp[1024] = {"\0"};
    strcat(cmd,name);
    FILE *fp = popen(cmd,"r");
    if(fp)
    {
        int temp = 0;
        while (fgets(tmp, 1024, fp))
        {
            //sata接口返回数据格式
            if(0 == strncmp(tmp,"194",3))
            {
                sscanf(tmp, "%*s %*s %*s %*s %*s %*s %*s %*s %*s %d", &temp);
                break;
            }
            //nvme接口返回数据格式
            if (0 == strncmp(tmp, "Temperature:", 12))
            {
                sscanf(tmp, "%*s %d %*s", &temp);
                break;
            }
        }
        return temp;
    }
    return -1;
}

int kdk_real_get_disk_rate(const char *name)
{
    char cmd[128] = {"smartctl -i "};
    char tmp[1024] = {"\0"};
    strcat(cmd, name);
    FILE *fp = popen(cmd, "r");
    if (fp)
    {
        int rate = 0;
        while (fgets(tmp, 1024, fp))
        {
            // sata接口返回数据格式
            if (0 == strncmp(tmp, "Rotation Rate:", 14))
            {
                sscanf(tmp, "%*s %*s %d %*s", &rate);
                break;
            }
        }
        return rate;
    }
    return -1;
}

float kdk_real_get_if_speed(const char *nc)
{
    if (!nc)
        return -1;
    struct if_speed ndev;
    memset(&ndev, 0, sizeof(struct if_speed));

    int ret = 0;
    memset(&ndev, 0, sizeof(ndev));
    sprintf(ndev.ifs_name, "%s", nc);
    ndev.ifs_us = 100000;
    ret = get_if_speed(&ndev);
    if (ret < 0)
        return -1;
    return ndev.ifs_ispeed * 1.0 / (ndev.ifs_us / 1000 * 0.001) / 1024.0f;
}
