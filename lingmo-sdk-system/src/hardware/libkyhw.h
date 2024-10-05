#ifndef KDK_SYSTEM_HW_ASSISTANT_H
#define KDK_SYSTEM_HW_ASSISTANT_H
#define ATTRSIZE 128
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     *
     *                      libkyassistant data structures
     *
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

    struct HWInfo
    {
        char model[ATTRSIZE];
        char vendor[ATTRSIZE];
        char device[ATTRSIZE];
        char driver[ATTRSIZE];
        char revision[ATTRSIZE];
        char busid[ATTRSIZE];
        char devicenum[ATTRSIZE];
        char width[8];
        char clock[8];
        struct HWInfo *next;
    };

    struct power_device
    {
        char name[256];
        char native_path[32];
        bool power_supply;
        char updated[64];
        bool has_history;
        bool has_statistics;
        bool is_persent;
        bool is_rechargeable;
        char state[32];
        char warning_level[32];
        double energy;
        double energy_empty;
        double energy_full;
        double energy_full_design;
        double energy_rate;
        double voltage;
        long time_to_empty;
        long time_to_full;
        double percentage;
        double temperature;
        double capacity;
        char technology[32];
        bool online;
        char icon_name[64];
        struct power_device *next;
    };

    struct Power
    {
        char daemon_version[32];
        bool on_battery;
        bool lid_is_closed;
        bool lid_is_present;
        char critical_action[32];
        struct power_device *devices;
    };

    struct KPci
    {
        char slot_path[16];
        char class_name[128];
        char product_name[128];
        unsigned char rev;
        char ss_name[256];
        char driver_use[1024];
        char **modules;
        int module_count;
        struct KPci *next;
    };

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     *
     *                      libkyassistant interface function
     *
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

    /**
     * @brief 获取硬件信息
     * @arg type 3 keyboard; 5 mouse; 15 sound; 23 cdrom; 27 usb
     * @return 硬件信息，具体参考返回结构体
     */
    extern struct HWInfo *kdk_hw_get_hwinfo(int type);
    extern void kdk_hw_free_hw(struct HWInfo *list);

    /**
     * @brief 获取电源信息
     * @return 电源信息，具体参考返回结构体
     */
    extern struct Power *kdk_hw_get_powerinfo();
    extern void kdk_hw_free_power_info(struct Power *info);

#ifdef __cplusplus
}
#endif

#endif // KDK_SYSTEM_HW_ASSISTANT_H
