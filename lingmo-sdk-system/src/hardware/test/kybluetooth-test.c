#include "../libkybluetooth.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i = 0;
    char *manufacturer = "\0";
    char *version = "\0";
    char *name = "\0";
    char *address = "\0";
    char *link_mode  ="\0"; 
    char *link_policy ="\0";
    char *scomtu = "\0";
    char *alcmtu = "\0";
    char *packettype = "\0";
    char *features = "\0";
    char *bus = "\0";
    int **id = kdk_bluetooth_get_device_id();
    do
    { 
        printf("id = %d\n",id[i]);
        manufacturer = kdk_bluetooth_get_manufacturer(i);
        printf("manufacturer: %s\n", manufacturer);
        free(manufacturer);
        version = kdk_bluetooth_get_dev_version(i);
        printf("version: %s\n", version);
        free(version);
        name = kdk_bluetooth_get_name(id[i]);
        printf("name: %s\n", name);
        address = kdk_bluetooth_get_address(id[i]);
        printf("address: %s\n", address);
        link_mode = kdk_bluetooth_get_link_mode(id[i]);
        printf("link_mode: %s\n", link_mode);
        link_policy = kdk_bluetooth_get_link_policy(id[i]);
        printf("link_policy: %s\n", link_policy);
        scomtu = kdk_bluetooth_get_scomtu(id[i]);
        printf("scomtu: %s\n", scomtu);
        alcmtu = kdk_bluetooth_get_alcmtu(id[i]);
        printf("alcmtu: %s\n", alcmtu);
        packettype = kdk_bluetooth_get_packettype(id[i]);
        printf("packettype: %s\n", packettype);
        features = kdk_bluetooth_get_features(id[i]);
        printf("features: %s\n", features);
        bus = kdk_bluetooth_get_bus(id[i]);
        printf("bus: %s\n", bus);
        i++;
    }while(id[i]);
    return 0;
}
