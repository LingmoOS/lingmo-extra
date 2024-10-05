#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include "libkybluetooth.h"
#include "hd.h"

int** kdk_bluetooth_get_device_id()
{
	static struct hci_dev_info di;
	int **id = (int *)malloc(sizeof(int) * 16);
	if(!id)
	{
		return 0;
	}
	struct hci_dev_list_req *dl;
	struct hci_dev_req *dr;
	int i, ctl;
	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0) {
		free(id);
		return 0;
	}

	if (!(dl = malloc(HCI_MAX_DEV * sizeof(struct hci_dev_req) + sizeof(uint16_t)))) {
		free(id);
		return 0;
	}
	dl->dev_num = HCI_MAX_DEV;
	dr = dl->dev_req;

	if (ioctl(ctl, HCIGETDEVLIST, (void *) dl) < 0) {
		free(dl);
		free(id);
		return 0;
	}

	for (i = 0; i< dl->dev_num; i++) {
		
		di.dev_id = (dr+i)->dev_id;
		id[i] = malloc(sizeof(int) * 16);
		if(!id[i])
		{
			free(dl);
			free(id);
			return 0;
		}
		id[i] = di.dev_id;
	}
	// i = i + 1;
	id[i] = 0;
	return id;
}

char *kdk_bluetooth_get_manufacturer(int id)
{
	char *manufacturer = (char *)malloc(64 * sizeof(char));
    if (!manufacturer)
    {
        return NULL;
    }
    char name[64] = "\0";

    hd_data_t *hd_data;
    hd_data = (hd_data_t *)calloc(1, sizeof *hd_data);
    if (!hd_data)
    {
        free(manufacturer);
        return NULL;
    }

    hd_data->progress = NULL;
    hd_data->debug = ~(HD_DEB_DRIVER_INFO | HD_DEB_HDDB);
	int i = 0;
    hd_t *hd, *hd0;
    hd0 = hd_list(hd_data, hw_bluetooth, 1, NULL);
    for (hd = hd0; hd; hd = hd->next)
    {
		if(i == id)
		{
			if(hd->vendor.name)
			{
				strcpy(name, hd->vendor.name);
			}
		}
		i++;
    }
	if(name[0] == '\0')
	{
		free(manufacturer);
		return NULL;
	}
	strcpy(manufacturer, name);
    return manufacturer;
}

char *kdk_bluetooth_get_dev_version(int id)
{
	char *version = (char *)malloc(12 * sizeof(char));
    if (!version)
    {
        return NULL;
    }
    char name[64] = "\0";

    hd_data_t *hd_data;
    hd_data = (hd_data_t *)calloc(1, sizeof *hd_data);
    if (!hd_data)
    {
        free(version);
        return NULL;
    }

    hd_data->progress = NULL;
    hd_data->debug = ~(HD_DEB_DRIVER_INFO | HD_DEB_HDDB);

	int i = 0;
    hd_t *hd, *hd0;
    hd0 = hd_list(hd_data, hw_bluetooth, 1, NULL);
    for (hd = hd0; hd; hd = hd->next)
    {
		if(i == id)
		{
			if(hd->revision.name)
			{
				strcpy(name, hd->revision.name);
			}
		}
        i++;
	}
	if(name[0] == '\0')
	{
		int ctl;
		static struct hci_dev_info di;
		struct hci_version ver;
		int dd;
		char tmp[30] = "\0";

		if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
		{
			free(version);
			return NULL;
		}

		if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
		{
			free(version);
			return NULL;
		}

		// print_dev_info(ctl, &di);
		dd = hci_open_dev(di.dev_id);
		if (dd < 0)
		{
			free(version);
			return NULL;
		}

		if (hci_read_local_version(dd, &ver, 1000) < 0)
		{
			free(version);
			return NULL;
		}
		if(ver.hci_rev)
		{
			sprintf(tmp, "0x%x", ver.hci_rev);
			strcpy(version, tmp);
		}
		
		hci_close_dev(dd);
	}
	else{
		strcpy(version, name);
	}
    return version;
}

char* kdk_bluetooth_get_name(int id)
{
	int ctl;
	static struct hci_dev_info di;
	int dd;
	char name[249];
	int i;
	char *version = (char *)malloc(sizeof(char) * 64);
	if(!version)
	{
		return NULL;
	}

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(version);
		return NULL;
	}
	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(version);
		return NULL;
	}

	dd = hci_open_dev(di.dev_id);
	if (dd < 0) {
		free(version);
		return NULL;
	}

	if (hci_read_local_name(dd, sizeof(name), name, 1000) < 0) {
		free(version);
		return NULL;
	}

	for (i = 0; i < 248 && name[i]; i++) {
		if ((unsigned char) name[i] < 32 || name[i] == 127)
			name[i] = '.';
	}

	name[248] = '\0';
	strcpy(version, name);
	hci_close_dev(dd);
	return version;
}

char* kdk_bluetooth_get_address(int id)
{
	int ctl;
	static struct hci_dev_info di;
	char addr[18] = "\0";
	char *address = (char *)malloc(sizeof(char) * 64);

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(address);
		return NULL;
	}

	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(address);
		return NULL;
	}

	ba2str(&di.bdaddr, addr);
	if(addr[0] != '\0')
	{
		strcpy(address, addr);
		return address;
	}
	else{
		free(address);
		return NULL;
	}
}

char* kdk_bluetooth_get_link_mode(int id)
{
	char *str = NULL;
	int ctl;
	static struct hci_dev_info di;
	char *link_mode = (char *)malloc(sizeof(char) * 64);
	if(!link_mode)
	{
		return NULL;
	}

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(link_mode);
		return NULL;
	}

	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(link_mode);
		return NULL;
	}
	str =  hci_lmtostr(di.link_mode);
	if(str)
	{
		strcpy(link_mode, str);
		bt_free(str);
		return link_mode;
	}
	else{
		free(link_mode);
		return NULL;
	}
}

char* kdk_bluetooth_get_link_policy(int id)
{
	int ctl;
	static struct hci_dev_info di;
	char *link_policy = (char *)malloc(sizeof(char) * 64);
	if(!link_policy)
	{
		return NULL;
	}

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(link_policy);
		return NULL;
	}

	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(link_policy);
		return NULL;
	}

	strcpy(link_policy, hci_lptostr(di.link_policy));
	return link_policy;
}

char* kdk_bluetooth_get_bus(int id)
{
	int ctl;
	static struct hci_dev_info di;
	char *bus = (char *)malloc(sizeof(char) * 64);
	if(!bus)
	{
		return NULL;
	}

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(bus);
		return NULL;
	}
	
	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(bus);
		return NULL;
	}

	strcpy(bus, hci_bustostr(di.type & 0x0f));
	return bus;
}

char* kdk_bluetooth_get_scomtu(int id)
{
	int ctl;
	static struct hci_dev_info di;
	char addr[18];
	char tmp[20] = "\0";
	char *scomtu = (char *)malloc(sizeof(char) * 64);
	if(!scomtu)
	{
		return NULL;
	}

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(scomtu);
		return NULL;
	}

	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(scomtu);
		return NULL;
	}

	ba2str(&di.bdaddr, addr);
	sprintf(tmp, "%d:%d", di.sco_mtu, di.sco_pkts);
	strcpy(scomtu, tmp);
	return scomtu;
}

char* kdk_bluetooth_get_alcmtu(int id)
{
	int ctl;
	static struct hci_dev_info di;
	char tmp[20] = "\0";
	char *alcmtu = (char *)malloc(sizeof(char) * 64);
	if(!alcmtu)
	{
		return NULL;
	}

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(alcmtu);
		return NULL;
	}

	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(alcmtu);
		return NULL;
	}

	sprintf(tmp, "%d:%d", di.acl_mtu, di.acl_pkts);
	strcpy(alcmtu, tmp);
	return alcmtu;
}

char* kdk_bluetooth_get_packettype(int id)
{
	char *str;
	int ctl;
	static struct hci_dev_info di;

	char *packettype = (char *)malloc(sizeof(char) * 64);
	if(!packettype)
		return NULL;

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(packettype);
		return NULL;
	}

	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(packettype);
		return NULL;
	}
	str = hci_ptypetostr(di.pkt_type);
	strcpy(packettype, str);
	bt_free(str);
	return packettype;
}

char* kdk_bluetooth_get_features(int id)
{
	int ctl;
	static struct hci_dev_info di;
	char tmp[50] = "\0";
	char *features = (char *)malloc(sizeof(char) * 64);
	if(!features)
		return NULL;

	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
	{
		free(features);
		return NULL;
	}

	di.dev_id = id;

	if (ioctl(ctl, HCIGETDEVINFO, (void *)&di))
	{
		free(features);
		return NULL;
	}

	sprintf(tmp, "0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x 0x%2.2x",
			di.features[0], di.features[1], di.features[2], di.features[3], 
			di.features[4], di.features[5], di.features[6], di.features[7]);
	strcpy(features, tmp);
	return features;
}
