#ifndef LIBKYUSBINFO_H
#define LIBKYUSBINFO_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*
     * usb设备信息
     */
    typedef struct driver_info
    {
        char name[32];
        char type[2 + 1];
        char pid[4 + 1];
        char vid[4 + 1];
        char serialNo[32];
        char devNode[32];
    } DriverInfo, *pDriverInfo;

    /*
     * usb设备信息链表
     */
    typedef struct driver_list
    {
        struct driver_info *data;
        struct driver_list *next;
    } DriverInfoList, *pDriverInfoList;

    extern pDriverInfoList kdk_usb_get_list();

    extern void kdk_usb_free(pDriverInfoList list);

#ifdef __cplusplus
}
#endif

#endif