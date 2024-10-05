#ifndef	__KLOG_FMT_H__
#define	__KLOG_FMT_H__
#include <stdbool.h>
#include "core.h"

typedef struct format
{
	bool	vis_identifier;
	bool	vis_execName;
	bool	vis_pid;
	bool	vis_tid;
	bool	vis_filename;
	bool	vis_funcline;
}PrintFormat;

extern PrintFormat	klog_printformat;

#ifndef KLOG_FORMATSET
#define	KLOG_FORMATSET
#define	FORMAT_LOGTYPE	0x0001
#define	FORMAT_PROCESSNAME	0x0002
#define	FORMAT_PID		0x0004
#define	FORMAT_FILENAME	0x0008
#define	FORMAT_LINENUM	0x0010
#endif

extern int	getRecordDate(char* date) NOTNULL();
extern void	loadFormatOptions();
extern int	formatMessage(int lvl , const char *filename , const char *func , int linenum , const char *message , char* result , unsigned int resultSize) NOTNULL();

#endif