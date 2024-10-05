#ifndef __KLOG_MQ_H__
#define	__KLOG_MQ_H__
#include "core.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct _KL_MessageNode{
	int	lvl;
	unsigned int bufSize;
	char buf[KLOG_MAXMSGSIZE];
}KL_MessageNode;

typedef struct _KL_MessageQueue{
	pthread_mutex_t* mlock;		//用于在操作队列时锁定
	int				autoIncrementQueueSize;	//是否自动扩充消息队列
	unsigned long	maxmessage;	//当前最大可接受消息数量，该数量超限后会以每次KLOG_DEFAULT_MSGGROWTH的数量扩充
	unsigned long	mnum;	//队列中消息的数量
	pthread_t thread_id;	//后台刷新线程ID
	int interval;			//刷新间隔
	KL_MessageNode**	message;	//消息缓冲
}KLMessageQueue;

extern int	initMessageQueue(int flushInterval , int autoIncrement);
extern int	insertMessage(int lvl , const char *message);
extern int	flushMessageQueue(int locked);
extern void	emptyMessageQueue();
extern void	destroyMessageQueue();

#endif