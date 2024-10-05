#ifndef DBUS_M_SYSTIME_SERVER_H__
#define DBUS_M_SYSTIME_SERVER_H__

// extern void *startBroadcastSystemTimePerMin();

/**
 * @brief 当系统时间被修改时，该函数会返回 1
 * 
 * @return int 
 */
extern int monitorSystemTimeChange();

#endif