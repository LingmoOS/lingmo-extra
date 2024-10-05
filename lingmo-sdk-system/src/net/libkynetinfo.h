#ifndef LIBKYNETINFO_H
#define LIBKYNETINFO_H

#define IN 
#define OUT

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct route
    {
        char name[32];
        char addr[16];
        struct route *next;
    } routeMapList, *prouteMapList;

    typedef struct chain_data
    {
        char target[8];
        char prot[4 + 1];
        char opt[4 + 1];
        char source[32];
        char destination[64];
        char option[128];
        struct chain_data *next;
    } chainData, *pChainData;

    typedef struct chain
    {
        char total[16];
        char policy[32];
        pChainData data;
        struct chain *next;
    } Chain, *pChain;

    /// @brief 获取指定网络端口的状态
    /// @param port ： 端口号
     /// @return 端口状态    TCP_ESTABLISHED:0x1   TCP_SYN_SENT:0x2    TCP_SYN_RECV:0x3    TCP_FIN_WAIT1:0x4    TCP_FIN_WAIT2:0x5 \
        TCP_TIME_WAIT:0x6    TCP_CLOSE:0x7    TCP_CLOSE_WAIT:0x8    TCP_LAST_ACL:0x9    TCP_LISTEN:0xa    TCP_CLOSING:0xb
    extern int kdk_net_get_port_stat(int port);

    extern int kdk_net_get_multiple_port_stat(int start, int end, int *result);

    extern prouteMapList kdk_net_get_route();

    extern void kdk_net_free_route(prouteMapList list);

    extern pChain kdk_net_get_iptable_rules();

    extern void kdk_net_free_chain(pChain list);

/**
 * @brief 获取子网信息
 * 
 * @param nc 网卡名称，如eno1
 * @param mask 子网掩码
 * 
 */
extern void kdk_net_get_netmask(IN const char *nc, OUT char *mask);

/**
 * @brief 获取进程对应端口
 * 
 * 
 * @return char ** 进程对应端口
 */
extern char **kdk_net_get_proc_port();

/**
 * @brief 获取所有UP状态的端口号
 * 
 * 
 * @return char ** 所有UP状态的端口号
 */
extern char **kdk_net_get_up_port();

/**
 * @brief  获取hosts配置
 * 
 * @return char* hosts配置
 */
extern char* kdk_net_get_hosts();

/**
 * @brief  获取hosts配置的域名映射
 * 
 * @return char* hosts配置的域名映射
 */
extern char* kdk_net_get_hosts_domain();

/**
 * @brief  获取DNS配置文件
 * 
 * @return char** DNS配置文件
 */
extern char** kdk_net_get_resolv_conf();

/**
 * @brief 用于回收字符串列表
 * 
 * @param ptr 字符串列表
 */
extern inline void kdk_net_freeall(char **ptr);

#ifdef __cplusplus
}
#endif

#endif
