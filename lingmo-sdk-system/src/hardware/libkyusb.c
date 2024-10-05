#include "libkyusb.h"
#include <stdio.h>
#include <string.h>
#include <libudev.h>
#include "libkylog.h"

/// @brief 
/// @return usb device info 
pDriverInfoList kdk_usb_get_list()
{
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *list_entry, *dev_list_entry;
    struct udev_device *dev, *device, *interface;

    pDriverInfoList list = NULL;
    pDriverInfoList prevnode = NULL;
    pDriverInfoList curnode = NULL;

    /* Create the udev object */
    udev = udev_new();
    if (!udev)
    {
        klog_err("Can't create udev");
        return list;
    }

    /*enum devices*/
    enumerate = udev_enumerate_new(udev);

    // 添加筛选条件 普通usb设备路径/sys/class/hidraw，U盘路径/sys/class/block
    //    udev_enumerate_add_match_subsystem(enumerate, "hidraw");
    //    udev_enumerate_add_match_subsystem(enumerate, "block");

    //扫描设备
    udev_enumerate_scan_devices(enumerate);

    //获取设备列表
    list_entry = udev_enumerate_get_list_entry(enumerate);
    //遍历
    udev_list_entry_foreach(dev_list_entry, list_entry)
    {
        const char *path;

        //创建dev_device设备对象
        path = udev_list_entry_get_name(dev_list_entry);
        device = udev_device_new_from_syspath(udev, path);
        if(!device)
        {
            kdk_usb_free(list);
            break;
        }

        //获取interface父节点，此节点记录设备类型
        interface = udev_device_get_parent_with_subsystem_devtype(device, "usb", "usb_interface");
        if (!interface)
        {
            goto free;
        }

        //非usb设备直接进入下一轮循环
        dev = udev_device_get_parent_with_subsystem_devtype(device, "usb", "usb_device");
        if (!dev)
        {
            goto free;
        }

        //同一款外设会生成两个设备文件，原因不明。暂时先手动去重
        pDriverInfoList tmp = list;
        int isRepetive = 0;
        while (tmp)
        {
            if(0 == strcmp(tmp->data->devNode,udev_device_get_devnode(dev)))
            {
                isRepetive = 1;
            }
            tmp = tmp->next;
        }
        if(isRepetive)
        {
            goto free;
        }

        //申请节点内存
        if(NULL == list)
        {
            curnode = (pDriverInfoList)calloc(1, sizeof(DriverInfoList));
            list = curnode;
        }
        else
            curnode = (pDriverInfoList)calloc(1, sizeof(DriverInfoList));

        if (NULL == curnode)
        {
            klog_err("Request Memory For List Failed");
            kdk_usb_free(list);
            return NULL;
        }

        curnode->data = (pDriverInfo)calloc(1, sizeof(DriverInfo));
        if (NULL == curnode->data)
        {
            klog_err("Request Memory For Data Failed");
            free(curnode);
            kdk_usb_free(list);
            return NULL;
        }

        sprintf(curnode->data->name, "%s", udev_device_get_sysattr_value(dev, "product") ? udev_device_get_sysattr_value(dev, "product"): "null");
        sprintf(curnode->data->type, "%s", udev_device_get_sysattr_value(interface, "bInterfaceClass") ? udev_device_get_sysattr_value(interface, "bInterfaceClass") : "null");
        sprintf(curnode->data->pid, "%s", udev_device_get_sysattr_value(dev, "idProduct") ? udev_device_get_sysattr_value(dev, "idProduct") : "null");
        sprintf(curnode->data->vid, "%s", udev_device_get_sysattr_value(dev, "idVendor") ? udev_device_get_sysattr_value(dev, "idVendor") : "null");
        sprintf(curnode->data->serialNo, "%s", udev_device_get_sysattr_value(dev, "serial") ? udev_device_get_sysattr_value(dev, "serial") : "null");
        sprintf(curnode->data->devNode ,"%s", udev_device_get_devnode(dev));
        // sprintf(curnode->data->vendor, "%s", udev_device_get_sysattr_value(dev, "manufacturer"));

        if (NULL != prevnode)
            prevnode->next = curnode;
        prevnode = curnode;
        curnode = NULL;
free:
        if(device)
            udev_device_unref(device);
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return list;
}

void kdk_usb_free(pDriverInfoList list)
{
    pDriverInfoList tmp = NULL;
    while (list)
    {
        tmp = list;
        list = list->next;
        free(tmp->data);
        free(tmp);
    }
}
