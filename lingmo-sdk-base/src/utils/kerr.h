#ifndef KYSDK_BASE_UTILS_ERR_H__
#define KYSDK_BASE_UTILS_ERR_H__

#ifdef  __linux__
#include <errno.h>
#define KDK_EINVALIDARGS    EINVAL
#define KDK_ENOMEM          ENOMEM
#define KDK_PERM            EPERM      
#define KDK_EUNKNOW         65535
#endif

#endif