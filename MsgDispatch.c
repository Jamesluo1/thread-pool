/*
 * This file represents how to dispatch the msg to relative thread pool
 * */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include "ThreadPool.h"
#include "MsgDis.h"

#define		THREAD_NUM_PER_POOL	3
#define		MAX(X,Y)	((X) > (Y)) ? (X):(Y)


ThreadPool* key_thread_pool = NULL;
ThreadPool* other_thread_pool = NULL;

void
InitDispatchCtx()
{
	key_thread_pool = CreateThreadPool("key", THREAD_NUM_PER_POOL);
	other_thread_pool = CreateThreadPool("other", THREAD_NUM_PER_POOL);
}

static
void EchoString(char* str)
{
	int len;
	pthread_t tid = pthread_self();
	char buf[200];
	int ret;
	int count;

	snprintf(buf, sizeof(buf), "thread(%u) received string:%s \n", (unsigned int)pthread_self(),str);
	count = strlen(buf);
	ret = write(STDOUT_FILENO, buf, count);
}

void
ProcessMsg(char* str)
{
	ASSERT(str);
	int ret;

	if((ret = strncmp(str, KEY_MSG_BEGIN, strlen(KEY_MSG_BEGIN)) == 0)) {
		if(key_thread_pool)
			PushWork(key_thread_pool, EchoString, str);
	} else if((ret = strncmp(str, EXIT_MSG, strlen(EXIT_MSG)) == 0)) {
		DestroyThreadPool(key_thread_pool);
		DestroyThreadPool(other_thread_pool);
	} else {
		if(other_thread_pool)
			PushWork(other_thread_pool, EchoString, str);
	}
}

void EnableLogToFile()
{


}
