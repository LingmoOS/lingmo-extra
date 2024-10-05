#include "libkynetinfo.h"
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <fcntl.h>
#include <paths.h>
#include <unistd.h>
#include <ctype.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <linux/route.h>
#include <sys/ioctl.h>
#include <lingmosdk/lingmosdk-base/cstring-extension.h>

enum cardstat
{
    NCSTAT_LINK_UP,
    NCSTAT_LINK_DOWN,
    NCSTAT_ERROR
};

#define PROGNAME_WIDTH 20
#define SELINUX_WIDTH 50
#define PRG_HASH_SIZE 211

static struct prg_node
{
    struct prg_node *next;
    unsigned long inode;
    char name[PROGNAME_WIDTH];
    char scon[SELINUX_WIDTH];
} *prg_hash[PRG_HASH_SIZE];

static char prg_cache_loaded = 0;

#define PRG_HASHIT(x) ((x) % PRG_HASH_SIZE)

#define PRG_LOCAL_ADDRESS "local_address"
#define PRG_INODE "inode"
#define PRG_SOCKET_PFX "socket:["
#define PRG_SOCKET_PFXl (strlen(PRG_SOCKET_PFX))
#define PRG_SOCKET_PFX2 "[0000]:"
#define PRG_SOCKET_PFX2l (strlen(PRG_SOCKET_PFX2))

#ifndef LINE_MAX
#define LINE_MAX 4096
#endif

#define PATH_PROC "/proc"
#define PATH_FD_SUFF "fd"
#define PATH_FD_SUFFl strlen(PATH_FD_SUFF)
#define PATH_PROC_X_FD PATH_PROC "/%s/" PATH_FD_SUFF
#define PATH_CMDLINE "cmdline"
#define PATH_CMDLINEl strlen(PATH_CMDLINE)

uint verify_file(char *pFileName)
{
    return !strncmp(pFileName, "/proc", strlen("/proc"));
}

static char *safe_strncpy(char *dst, const char *src, size_t size)
{
    dst[size - 1] = '\0';
    return strncpy(dst, src, size - 1);
}

static void prg_cache_add(unsigned long inode, char *name, const char *scon)
{
    unsigned hi = PRG_HASHIT(inode);
    struct prg_node **pnp, *pn;

    prg_cache_loaded = 2;
    for (pnp = prg_hash + hi; (pn = *pnp); pnp = &pn->next)
    {
        if (pn->inode == inode)
        {
            /* Some warning should be appropriate here
               as we got multiple processes for one i-node */
            return;
        }
    }
    if (!(*pnp = (struct prg_node *)malloc(sizeof(**pnp))))
        return;
    pn = *pnp;
    pn->next = NULL;
    pn->inode = inode;
    safe_strncpy(pn->name, name, sizeof(pn->name));

    {
        int len = (strlen(scon) - sizeof(pn->scon)) + 1;
        if (len > 0)
            safe_strncpy(pn->scon, &scon[len + 1], sizeof(pn->scon));
        else
            safe_strncpy(pn->scon, scon, sizeof(pn->scon));
    }
}

static int extract_type_1_socket_inode(const char lname[], unsigned long *inode_p)
{

    /* If lname is of the form "socket:[12345]", extract the "12345"
       as *inode_p.  Otherwise, return -1 as *inode_p.
       */

    if (strlen(lname) < PRG_SOCKET_PFXl + 3)
        return (-1);

    if (memcmp(lname, PRG_SOCKET_PFX, PRG_SOCKET_PFXl))
        return (-1);
    if (lname[strlen(lname) - 1] != ']')
        return (-1);

    {
        char inode_str[strlen(lname + 1)]; /* e.g. "12345" */
        const int inode_str_len = strlen(lname) - PRG_SOCKET_PFXl - 1;
        char *serr;

        strncpy(inode_str, lname + PRG_SOCKET_PFXl, inode_str_len);
        inode_str[inode_str_len] = '\0';
        *inode_p = strtoul(inode_str, &serr, 0);
        if (!serr || *serr || *inode_p == ~0)
            return (-1);
    }
    return (0);
}

static int extract_type_2_socket_inode(const char lname[], unsigned long *inode_p)
{

    /* If lname is of the form "[0000]:12345", extract the "12345"
       as *inode_p.  Otherwise, return -1 as *inode_p.
       */

    if (strlen(lname) < PRG_SOCKET_PFX2l + 1)
        return (-1);
    if (memcmp(lname, PRG_SOCKET_PFX2, PRG_SOCKET_PFX2l))
        return (-1);

    {
        char *serr;

        *inode_p = strtoul(lname + PRG_SOCKET_PFX2l, &serr, 0);
        if (!serr || *serr || *inode_p == ~0)
            return (-1);
    }
    return (0);
}

static void parse_port_state(int *st, int port, FILE *fp)
{
    char line[1024] = "\0";
    int lnr = 0;
    unsigned long rxq, txq, time_len, retr, inode;
    int num, local_port, rem_port, d, state, uid, timer_run, timeout;
    char rem_addr[128] = "0", local_addr[128] = "0";
    struct sockaddr_storage localsas, remsas;
    struct sockaddr_in *localaddr = (struct sockaddr_in *)&localsas;
    struct sockaddr_in *remaddr = (struct sockaddr_in *)&remsas;

    // #if HAVE_AFINET6
    //     char addr6[INET6_ADDRSTRLEN];
    //     struct in6_addr in6;
    //     extern struct aftype inet6_aftype;
    // #endif

    while (fgets(line, 1024, fp))
    {
        if (lnr++ == 0)
            continue;
        num = sscanf(line,
                     "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
        if (local_port == port)
        {
            *st = state;
            break;
        }
    }
    //     if (strlen(local_addr) > 8)
    //     {
    // #if HAVE_AFINET6
    //         /* Demangle what the kernel gives us */
    //         sscanf(local_addr, "%08X%08X%08X%08X",
    //                &in6.s6_addr32[0], &in6.s6_addr32[1],
    //                 &in6.s6_addr32[2], &in6.s6_addr32[3]);
    //         inet_ntop(AF_INET6, &in6, addr6, sizeof(addr6));
    //         inet6_aftype.input(1, addr6, &localsas);
    //         sscanf(rem_addr, "%08X%08X%08X%08X",
    //                &in6.s6_addr32[0], &in6.s6_addr32[1],
    //                 &in6.s6_addr32[2], &in6.s6_addr32[3]);
    //         inet_ntop(AF_INET6, &in6, addr6, sizeof(addr6));
    //         inet6_aftype.input(1, addr6, &remsas);
    //         localsas.ss_family = AF_INET6;
    //         remsas.ss_family = AF_INET6;
    // #endif
    //     }
    //     else
    //     {
    //         sscanf(local_addr, "%X", &localaddr->sin_addr.s_addr);
    //         sscanf(rem_addr, "%X", &remaddr->sin_addr.s_addr);
    //         localsas.ss_family = AF_INET;
    //         remsas.ss_family = AF_INET;
    //     }
}

int kdk_net_get_port_stat(int port)
{
    FILE *fp = fopen("/proc/net/tcp", "r");
    if (!fp)
        return -1;
    int state = 0;
    parse_port_state(&state, port, fp);
    fclose(fp);

    fp = fopen("/proc/net/tcp6", "r");
    if (!fp)
        return -1;
    parse_port_state(&state, port, fp);
    fclose(fp);

    return state;
}

int kdk_net_get_multiple_port_stat(int start, int end, int *result)
{
    if (end < start)
        return -1;
    for (int i = start; i <= end; i++)
    {
        result[i - start] = kdk_net_get_port_stat(i);
        if (-1 == result[i - start])
            return i - start;
    }
    return 0;
}

prouteMapList kdk_net_get_route()
{
    prouteMapList list = NULL, currnode = NULL, prenode = NULL;
    FILE *fp;
    char devname[64];
    unsigned long d, g, m;
    int r;
    int flgs, ref, use, metric, mtu, win, ir;

    fp = fopen("/proc/net/route", "r");

    /* Skip the first line. */
    r = fscanf(fp, "%*[^\n]\n");

    if (r < 0)
    {
        /* Empty line, read error, or EOF. Yes, if routing table
         * is completely empty, /proc/net/route has no header.
         */
        goto ERROR;
    }

    while (1)
    {
        r = fscanf(fp, "%63s%lx%lx%X%d%d%d%lx%d%d%d\n",
                   devname, &d, &g, &flgs, &ref, &use, &metric, &m,
                   &mtu, &win, &ir);

        if (r != 11)
        {
            if ((r < 0) && feof(fp))
            {
                /* EOF with no (nonspace) chars read. */
                break;
            }
        }
        /*
         * # cat /proc/net/route
         * Iface   Destination     Gateway         Flags   RefCnt  Use     Metric  Mask            MTU     Window  IRTT
         * eth0    00000000        013CA8C0        0003    0       0       0       00000000        0       0       0
         * eth0    003CA8C0        00000000        0001    0       0       0       00FFFFFF        0       0       0
         * 默认网关的特性: dst为0，netmask为0, Gateway不为0
         */
        if ((flgs & (RTF_GATEWAY | RTF_UP)) &&
            g != 0 && d == 0 && m == 0)
        {
            currnode = (prouteMapList)calloc(1, sizeof(routeMapList));
            if (!currnode)
            {
                kdk_net_free_route(list);
                goto ERROR;
            }
            struct sockaddr_in sin;
            memset(&sin, 0, sizeof(struct sockaddr_in));
            memcpy(&sin.sin_addr, &g, 4);
            strcpy(currnode->name, devname);
            strcpy(currnode->addr, inet_ntoa(sin.sin_addr));
            if (!list)
                list = currnode;
            else
                prenode->next = currnode;
            prenode = currnode;
        }
    }

ERROR:
    fclose(fp);
    return list;
}

void kdk_net_free_route(prouteMapList list)
{
    prouteMapList tmp = NULL;
    while (list)
    {
        tmp = list;
        list = list->next;
        free(tmp);
    }
}

pChain kdk_net_get_iptable_rules()
{
    pChain result = NULL, tmp = NULL;
    char line[512] = "\0";
    FILE *fp = popen("iptables -L -n", "r");
    if (NULL == fp)
    {
        pclose(fp);
        return NULL;
    }
    while (fgets(line, 512, fp))
    {
        if (strstr(line, "Chain"))
        {
            // 申请Chain内存
            if (!result)
            {
                result = (pChain)calloc(1, sizeof(Chain));
                tmp = result;
            }
            else
            {
                tmp->next = (pChain)calloc(1, sizeof(Chain));
                tmp = tmp->next;
            }
            if (NULL == tmp)
            {
                kdk_net_free_chain(result);
                result = NULL;
                break;
            }
            int count = sscanf(line, "%*s %s (%[^)]", tmp->total, tmp->policy);
            continue;
        }

        if (!tmp)
            continue;

        if (strstr(line, "target     prot opt source               destination"))
            continue;
        char target[8] = "0", prot[4 + 1] = "0", opt[4 + 1] = "0", source[16] = "0", destination[16] = "0", option[32] = "0";
        pChainData tmpData;
        int count = sscanf(line, "%s %s %s %s %s %[^\n]", target, prot, opt, source, destination, option);
        if (count < 5)
        {
            continue;
        }
        // Chain下每条规则申请一个chainData结构体
        if (!tmp->data)
        {
            tmp->data = (pChainData)calloc(1, sizeof(chainData));
            tmpData = tmp->data;
        }
        else
        {
            tmpData->next = (pChainData)calloc(1, sizeof(chainData));
            tmpData = tmpData->next;
        }
        if (NULL == tmpData)
        {
            kdk_net_free_chain(result);
            result = NULL;
            break;
        }
        if (target)
            strcpy(tmpData->target, target);
        if (prot)
            strcpy(tmpData->prot, prot);
        if (opt)
            strcpy(tmpData->opt, opt);
        if (source)
            strcpy(tmpData->source, source);
        if (destination)
            strcpy(tmpData->destination, destination);
        if (option)
            strcpy(tmpData->option, option);
    }
    pclose(fp);
    return result;
}

void kdk_net_free_chain(pChain list)
{
    pChain tmpChain = NULL;
    pChainData tmpData = NULL;
    while (list)
    {
        tmpChain = list;
        list = list->next;
        while (tmpChain->data)
        {
            tmpData = tmpChain->data;
            tmpChain->data = tmpData->next;
            free(tmpData);
        }

        free(tmpChain);
    }
}

static enum cardstat _get_card_stat(const char *nc)
{
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (!sfd)
    {
        // klog_err("网卡[%s]状态获取失败：%s\n", nc, strerror(errno));
        return NCSTAT_ERROR;
    }

    struct ifreq stIf;
    strcpy(stIf.ifr_ifrn.ifrn_name, nc);

    if (ioctl(sfd, SIOCGIFFLAGS, &stIf) < 0)
    {
        // klog_err("网卡[%s]状态获取失败：%s\n", nc, strerror(errno));
        close(sfd);
        return NCSTAT_ERROR;
    }

    // close(sfd);
    if (stIf.ifr_ifru.ifru_flags & IFF_RUNNING)
        return NCSTAT_LINK_UP;

    return NCSTAT_LINK_DOWN;
}

void kdk_net_get_netmask(IN const char *nc, OUT char *mask)
{
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, nc);
    int ret = ioctl(fd, SIOCGIFNETMASK, &ifr);
    if (0 != ret)
    {
        return;
    }
    struct sockaddr_in sin_addr;
    if (NCSTAT_LINK_UP == _get_card_stat(nc))
        strcpy(mask, (char *)inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_netmask))->sin_addr));
    else
        strcpy(mask, "-");
}

static const char *prg_cache_get(unsigned long inode)
{
    unsigned hi = PRG_HASHIT(inode);
    struct prg_node *pn;

    for (pn = prg_hash[hi]; pn; pn = pn->next)
        if (pn->inode == inode)
            return (pn->name);
    return ("-");
}

static void prg_cache_clear()
{
    struct prg_node **pnp, *pn;

    if (prg_cache_loaded == 2)
        for (pnp = prg_hash; pnp < prg_hash + PRG_HASH_SIZE; pnp++)
            while ((pn = *pnp))
            {
                *pnp = pn->next;
                free(pn);
            }
    prg_cache_loaded = 0;
}

static void prg_cache_load(void)
{
    char line[LINE_MAX], eacces = 0;
    int procfdlen, fd, cmdllen, lnamelen;
    char lname[30], cmdlbuf[512], finbuf[PROGNAME_WIDTH];
    unsigned long inode;
    const char *cs, *cmdlp;
    DIR *dirproc = NULL, *dirfd = NULL;
    struct dirent *direproc, *direfd;
#if HAVE_SELINUX
    security_context_t scon = NULL;
#endif

    prg_cache_loaded = 1;
    cmdlbuf[sizeof(cmdlbuf) - 1] = '\0';
    if (!(dirproc = opendir(PATH_PROC)))
        goto fail;
    while (errno = 0, direproc = readdir(dirproc))
    {
        for (cs = direproc->d_name; *cs; cs++)
            if (!isdigit(*cs))
                break;
        if (*cs)
            continue;
        procfdlen = snprintf(line, sizeof(line), PATH_PROC_X_FD, direproc->d_name);
        if (procfdlen <= 0 || procfdlen >= sizeof(line) - 5)
            continue;
        errno = 0;
        char canonical_filename[4096] = {"\0"};
        char filename[4096] = {"\0"};
        if (!realpath(line, canonical_filename) || !verify_file(canonical_filename))
        {
            closedir(dirproc);
            return;
        }
        dirfd = opendir(canonical_filename);
        if (!dirfd)
        {
            if (errno == EACCES)
                eacces = 1;
            continue;
        }
        canonical_filename[procfdlen] = '/';
        cmdlp = NULL;
        while ((direfd = readdir(dirfd)))
        {
            /* Skip . and .. */
            if (!isdigit(direfd->d_name[0]))
                continue;
            if (procfdlen + 1 + strlen(direfd->d_name) + 1 > sizeof(canonical_filename))
                continue;
            memcpy(canonical_filename + procfdlen - PATH_FD_SUFFl, PATH_FD_SUFF "/",
                   PATH_FD_SUFFl + 1);
            safe_strncpy(canonical_filename + procfdlen + 1, direfd->d_name,
                         sizeof(canonical_filename) - procfdlen - 1);
            lnamelen = readlink(canonical_filename, lname, sizeof(lname) - 1);
            if (lnamelen == -1)
                continue;
            lname[lnamelen] = '\0'; /*make it a null-terminated string*/

            if (extract_type_1_socket_inode(lname, &inode) < 0)
                if (extract_type_2_socket_inode(lname, &inode) < 0)
                    continue;

            if (!cmdlp)
            {
                if (procfdlen - PATH_FD_SUFFl + PATH_CMDLINEl >=
                    sizeof(canonical_filename) - 5)
                    continue;
                safe_strncpy(canonical_filename + procfdlen - PATH_FD_SUFFl, PATH_CMDLINE,
                             sizeof(canonical_filename) - procfdlen + PATH_FD_SUFFl);
                if (!realpath(canonical_filename, filename) || !verify_file(filename))
                {
                    closedir(dirfd);
                    closedir(dirproc);
                    return;
                }
                fd = open(filename, O_RDONLY);
                if (fd < 0)
                    continue;
                cmdllen = read(fd, cmdlbuf, sizeof(cmdlbuf) - 1);
                if (close(fd))
                    continue;
                if (cmdllen == -1)
                    continue;
                if (cmdllen < sizeof(cmdlbuf) - 1)
                    cmdlbuf[cmdllen] = '\0';
                if (cmdlbuf[0] == '/' && (cmdlp = strrchr(cmdlbuf, '/')))
                    cmdlp++;
                else
                    cmdlp = cmdlbuf;
            }

            snprintf(finbuf, sizeof(finbuf), "%s/%s", direproc->d_name, cmdlp);
#if HAVE_SELINUX
            if (getpidcon(atoi(direproc->d_name), &scon) == -1)
            {
                scon = xstrdup("-");
            }
            prg_cache_add(inode, finbuf, scon);
            freecon(scon);
#else
            prg_cache_add(inode, finbuf, "-");
#endif
        }
        closedir(dirfd);
        dirfd = NULL;
    }
    if (dirproc)
        closedir(dirproc);
    if (dirfd)
        closedir(dirfd);
    if (!eacces)
        return;
    if (prg_cache_loaded == 1)
    {
    fail:
        fprintf(stderr, "(No info could be read for \"-p\": geteuid()=%d but you should be root.)\n",
                geteuid());
    }
    else
        fprintf(stderr, "(Not all processes could be identified, non-owned process info\n"
                        " will not be shown, you would have to be root to see it all.)\n");
}

char **kdk_net_get_proc_port()
{
    prg_cache_load();
    char **result = NULL;
    int index = -1;

    char line[1024] = "\0";
    unsigned long rxq, txq, time_len, retr, inode;
    int num, local_port, rem_port, d, state, uid, timer_run, timeout;
    char rem_addr[128] = "0", local_addr[128] = "0";

    FILE *fp = fopen("/proc/net/tcp", "r");
    if (!fp)
        return NULL;
    while (fgets(line, 1024, fp))
    {
        num = sscanf(line,
                     "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
        const char *proc = prg_cache_get(inode);
        if (0 == strcmp(proc, "-"))
            continue;

        index++;
        result = (char **)realloc(result, sizeof(char *) * (index + 1 + 1)); // 以NULL结尾
        result[index] = (char *)calloc(1, 64);
        sprintf(result[index], "%d/%s/%s", local_port, proc, "tcp");
    }
    fclose(fp);

    fp = fopen("/proc/net/tcp6", "r");
    if (!fp)
    {
        while (result && (index >= 0))
        {
            free(result[index--]);
        }
        free(result);
        return NULL;
    }
    while (fgets(line, 1024, fp))
    {
        num = sscanf(line,
                     "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
        const char *proc = prg_cache_get(inode);
        if (0 == strcmp(proc, "-"))
            continue;

        index++;
        result = (char **)realloc(result, sizeof(char *) * (index + 1 + 1)); // 以NULL结尾
        result[index] = (char *)calloc(1, 64);
        sprintf(result[index], "%d/%s/%s", local_port, proc, "tcp6");
    }
    fclose(fp);

    fp = fopen("/proc/net/udp", "r");
    if (!fp)
    {
        while (result && (index >= 0))
        {
            free(result[index--]);
        }
        free(result);
        return NULL;
    }
    while (fgets(line, 1024, fp))
    {
        num = sscanf(line,
                     "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
        const char *proc = prg_cache_get(inode);
        if (0 == strcmp(proc, "-"))
            continue;

        index++;
        result = (char **)realloc(result, sizeof(char *) * (index + 1 + 1)); // 以NULL结尾
        result[index] = (char *)calloc(1, 64);
        sprintf(result[index], "%d/%s/%s", local_port, proc, "udp");
    }
    fclose(fp);

    fp = fopen("/proc/net/udp6", "r");
    if (!fp)
    {
        while (result && (index >= 0))
        {
            free(result[index--]);
        }
        free(result);
        return NULL;
    }
    while (fgets(line, 1024, fp))
    {
        num = sscanf(line,
                     "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
        const char *proc = prg_cache_get(inode);
        if (0 == strcmp(proc, "-"))
            continue;

        index++;
        result = (char **)realloc(result, sizeof(char *) * (index + 1 + 1)); // 以NULL结尾
        result[index] = (char *)calloc(1, 64);
        sprintf(result[index], "%d/%s/%s", local_port, proc, "udp6");
    }
    fclose(fp);

    result[index + 1] = NULL;

    prg_cache_clear();
    return result;
}

char **kdk_net_get_up_port()
{
    char **result = NULL;
    int index = -1;

    char line[1024] = "\0";
    unsigned long rxq, txq, time_len, retr, inode;
    int num, local_port, rem_port, d, state, uid, timer_run, timeout;
    char rem_addr[128] = "0", local_addr[128] = "0";

    FILE *fp = fopen("/proc/net/tcp", "r");
    if (!fp)
        return NULL;
    while (fgets(line, 1024, fp))
    {
        num = sscanf(line,
                     "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
        if (0x0a == state)
        {
            int i = 0;
            int flag = 1;
            while (i <= index) // 查重
            {
                if (local_port == atoi(result[i++]))
                    flag = 0; // 有重复  flag置为1
            }

            if (flag)
            {
                index++;
                char **tmp = (char **)realloc(result, sizeof(char *) * (index + 1 + 1)); // 以NULL结尾
                if (!tmp)
                    goto error;
                result = tmp;
                result[index] = (char *)calloc(1, 8);
                if (!result[index])
                    goto error;
                sprintf(result[index], "%d", local_port);
            }
        }
    }
    fclose(fp);

    fp = fopen("/proc/net/tcp6", "r");
    if (!fp)
        return NULL;
    while (fgets(line, 1024, fp))
    {
        num = sscanf(line,
                     "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %*s\n",
                     &d, local_addr, &local_port, rem_addr, &rem_port, &state,
                     &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode);
        if (0x0a == state)
        {
            int i = 0;
            int flag = 1;
            while (i <= index) // 查重
            {
                if (local_port == atoi(result[i++]))
                    flag = 0; // 有重复  flag置为1
            }

            if (flag)
            {
                index++;
                result = (char **)realloc(result, sizeof(char *) * (index + 1 + 1)); // 以NULL结尾
                if (!result)
                    goto error;
                result[index] = (char *)calloc(1, 8);
                if (!result[index])
                    goto error;
                sprintf(result[index], "%d", local_port);
            }
        }
    }
    fclose(fp);
    result[index + 1] = NULL;

    return result;
error:
    // for (int i = 0; result[i]; i++)
    // {
    //     free(result[i]);
    // }
    while (index)
    {
        free(result[index - 1]);
        index--;
    }
    free(result);
    result = NULL;
    return result;
}

char *kdk_net_get_hosts()
{
    FILE *fp = fopen("/etc/hosts", "r");
    if (fp == NULL)
    {
        return NULL;
    }

    char *hosts = (char *)malloc(sizeof(char) * 64);
    if (hosts == NULL)
    {
        fclose(fp);
        return NULL;
    }
    memset(hosts, 0, 64);
    char line[4096] = "\0";
    memset(hosts, 0, sizeof(hosts));
    while (fgets(line, sizeof(line), fp))
    {
        strcat(hosts, line);
        if (line[0] == '\n')
        {
            break;
        }
    }
    fclose(fp);
    strstripspace(hosts);
    return hosts;
}

char *kdk_net_get_hosts_domain()
{
    FILE *fp = fopen("/etc/hosts", "r");
    if (fp == NULL)
    {
        return NULL;
    }

    char line[4096] = "\0";
    char strtmp[4096] = "\0";
    int i = 0;
    int j = 0;
    while (fgets(line, sizeof(line), fp))
    {
        if(i == 1)
        {
            if(j != 6)
            {
                j++;
            }
        }
        else{
            if(strstr(line, "IPv6 capable hosts"))
            {
                i = 1;
            }
            else{
                continue;
            }
        }
        if((j == 6) && (strlen(line) != 0))
        {
            strcat(strtmp, line);
        }
    }

    int len = strlen(strtmp);

    char *hosts = (char *)malloc(sizeof(char) * len + 1);
    if (hosts == NULL)
    {
        fclose(fp);
        return NULL;
    }
    memset(hosts, 0, len + 1);
    strcpy(hosts, strtmp);
    fclose(fp);
    strstripspace(hosts);
    return hosts;
}

char **kdk_net_get_resolv_conf()
{
    char nserver[64] = "\0";
    char addr[64] = "\0";
    char **result = NULL;
    result = (char **)realloc(result, sizeof(char *) * 256); // 以NULL结尾
    if (!result)
    {
        return NULL;
    }
    int index = 0;
    FILE *fp = fopen("/etc/resolv.conf", "r");
    if (fp == NULL)
    {
        free(result);
        return NULL;
    }

    char line[4096] = "\0";
    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "nameserver"))
        {
            result[index] = (char *)malloc(sizeof(char) * 256);
            if (!result[index])
            {
                fclose(fp);
                free(result);
                return NULL;
            }
            sscanf(line, "%s %s", nserver, addr);
            strcpy(result[index], addr);
            index++;
        }
    }
    fclose(fp);
    result[index] = NULL;
    return result;
}

inline void kdk_net_freeall(char **list)
{
    if (!list)
        return;
    size_t index = 0;
    while (list[index])
    {
        free(list[index]);
        index++;
    }
    free(list);
}
