#include "../libkynetinfo.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[])
{
    prouteMapList list = kdk_net_get_route(), tmp = list;
    while (tmp)
    {
        printf("%s default route %s\n", tmp->name, tmp->addr);
        tmp = tmp->next;
    }
    kdk_net_free_route(list);
    if (argc < 2)
        printf("please input test port num\n");
    else
        printf("port:%s's state %d\n", argv[1], kdk_net_get_port_stat(atoi(argv[1])));
    
    pChain chain = kdk_net_get_iptable_rules();
    pChain tmpchain = chain;
    while(tmpchain)
    {
        printf("Chain: %s\tpolice: %s\n",tmpchain->total,tmpchain->policy);
        pChainData tmpData = tmpchain->data;
        if(tmpData)
            printf("target: %s\tprot: %s\topt: %s\tsource: %s\tdestination: %s\toption: %s\n",tmpData->target,tmpData->prot,tmpData->opt,
                                tmpData->source,tmpData->destination,tmpData->option);
        printf("\n");
        tmpchain = tmpchain->next;
    }
    kdk_net_free_chain(chain);

    int result[65536];
    int net = kdk_net_get_multiple_port_stat(0, 1000, result);
    if(net == 0)
    {
        size_t count = 0;
        for(;count < 1000 ;count++)
        {
            printf("%d\t", result[count]);
        }
    }
    else
        printf("Failed With %d", net);
     printf("\n");
    
    char mask[32] = "\0";
    kdk_net_get_netmask("enaftgm1i0", mask);
    printf("mask = %s\n", mask);

    char **port = kdk_net_get_proc_port();
    size_t index = 0;
    while(port[index])
    {
        printf("No.%ld, port = %s\n", index+1, port[index]);
        index++;
    }
    kdk_net_freeall(port);

    port = kdk_net_get_up_port();
    index = 0;
    while(port[index])
    {
        printf("No.%ld, port = %s\n", index+1, port[index]);
        index++;
    }
    kdk_net_freeall(port);

    char *hosts = kdk_net_get_hosts();
    if (hosts != NULL)
    {
        printf("hosts = %s\n", hosts);
        free(hosts);
    }

    char *domain = kdk_net_get_hosts_domain();
    if (domain != NULL)
    {
        printf("domain = %s\n", domain);
        free(domain);
    }

    char **resolv = kdk_net_get_resolv_conf();
    index = 0;
    while(resolv[index])
    {
        printf("No.%ld, resolv = %s\n", index+1, resolv[index]);
        index++;
    }
    kdk_net_freeall(resolv);
    return 0;
}
