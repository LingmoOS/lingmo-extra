#ifndef KYSDK_BASE_PROCESSDAEMON_H__
#define KYSDK_BASE_PROCESSDAEMON_H__

namespace KYSDK_BASE
{
typedef void (*daemon_callback)(void *args);
extern int startProcDaemon(unsigned int interval, daemon_callback func, void* args);
extern void feedDog();
extern int stopProcDaemon(); 
} // namespace KYSDK_BASE

#endif

/**
 * \example lingmosdk-base/src/process/test/main.cpp
 * 
 */