#include "write.h"
#include "klog_mqueue.h"
#include "writeFile.h"
#include <stdarg.h>
#include <errno.h>
#include <syslog.h>
#include <systemd/sd-journal.h>

static inline void	writeSyslog(int lvl , const char *message)
{
#ifndef __loongarch__
	sd_journal_send("MESSAGE=%s", message , "PRIORITY=%i" , lvl ,\
							"SYSLOG_FACILITY=%i" , 19 , NULL);
#else
	// TODO: loongarch64 's syslog write
	return;
#endif
}

static int writeSpecFile(int lvl , const char *filename , const char *func , int linenum , const char *message)
{
	char buffer[KLOG_MAXMSGSIZE + 1]	= {0};
	formatMessage(lvl , filename , func , linenum , message , buffer , KLOG_MAXMSGSIZE);
	if (logger->stype == SYNC)
	{
		if (logger->levelBasedStorage)
			return writeFile(lvl , buffer , strlen(buffer)) || fflush(logger->fp.classfiedfp[lvl]);
		else
			return writeFile(lvl , buffer , strlen(buffer)) || fflush(logger->fp.commonfp);
	}
	else
	{
		insertMessage(lvl , buffer);
	}
	return 0;
}

static void writeStdout(int lvl , const char *filename , const char *func , int linenum , const char *message)
{
	char buffer[KLOG_MAXMSGSIZE + 1]	= {0};
	formatMessage(lvl , filename , func , linenum , message , buffer , KLOG_MAXMSGSIZE);
	fprintf(stdout , "%s" , buffer);
}

int writeLog(int lvl , const char *filename , const char *func , int linenum , const char *message)
{
	switch (logger->otype)
	{
		case OUT_SYSLOG:
		{
			writeSyslog(lvl , message);
		}break;
		case OUT_SPECFILE:{
			writeSpecFile(lvl , filename , func , linenum , message);
		}break;
		case OUT_STDOUT:{
			writeStdout(lvl , filename , func , linenum , message);
		}break;
		default:{
			printf("输出类别无效！%d\n" , logger->otype);
			return EINVAL;
		}break;
	}

	return 0;	
}