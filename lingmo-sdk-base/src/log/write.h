#ifndef	__KLOG_WRITE_H__
#define	__KLOG_WRITE_H__
#include "format.h"
#include "core.h"

extern int	writeLog(int lvl , const char *filename , const char *func , int linenum , const char *message) NOTNULL();

#endif