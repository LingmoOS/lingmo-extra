#include "../libkync.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char **cards = kdk_nc_get_cardlist();
    size_t index = 0;
    printf("所有网卡：\n");
    while (cards[index])
    {
        char *mac = kdk_nc_get_phymac(cards[index]);
        char *ipv4 = kdk_nc_get_private_ipv4(cards[index]);
        char *ipv6 = kdk_nc_get_private_ipv6(cards[index]);
        char *driver = kdk_nc_get_driver(cards[index]);
        char vendor[256] = "\0", product[256] = "\0";
        kdk_nc_get_vendor_and_product(cards[index], vendor, product);
        printf("Card %zd: %s\tStatus: %s\tMac: %s\tIPv4: %s\tIPv6: %s\t Vendor: %s\t Product: %s\t Type: %s\t driver: %s\n",
               index + 1, cards[index], kdk_nc_is_up(cards[index]) == 1 ? "Up" : "Down",
               mac, ipv4, ipv6, vendor, product,
               kdk_nc_is_wireless(cards[index]) ? "wireless" : "ethernet", driver);

        char **list4 = kdk_nc_get_ipv4(cards[index]);
        int i = 0;
        printf("AllIpV4: ");
        while (list4 && list4[i])
        {
            printf("%s\t", list4[i++]);
        }
        printf("\n");
        char **list6 = kdk_nc_get_ipv6(cards[index]);
        printf("AllIpV6: ");
        i = 0;
        while (list6 && list6[i])
        {
            printf("%s\t", list6[i++]);
        }
        printf("\n");
        free(mac);
        free(ipv4);
        free(ipv6);
        free(list4);
        free(list6);
        free(driver);
        index++;
    }
    kdk_nc_freeall(cards);
    char **upcards = kdk_nc_get_upcards();
    index = 0;
    printf("工作网卡：\n");
    while (upcards[index])
    {
        printf("Card %zd: %s\n", index + 1, upcards[index]);
        index++;
    }
    kdk_nc_freeall(upcards);
    return 0;
}
