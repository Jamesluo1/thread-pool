/*
 * This file implements the thread pool functionality.
 * .Create one pool
 * .Add work to pool
 * .Destroy pool
 * */
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "ThreadPool.h"
#include "MsgDisMain.h"

static void
PostJob(ThreadPool *pool)
{
	pthread_mutex_lock(&pool->jobQueue.cond_lock);
	pthread_cond_signal(&pool->jobQueue.cond);
	pthread_mutex_unlock(&pool->jobQueue.cond_lock);
}

static void
WaitJob(ThreadPool *pool)
{
	pthread_mutex_lock(&pool->jobQueue.cond_lock);
	while(pool->jobQueue.count == 0 && pool->keepalive == 1) {
		pthread_cond_wait(&pool->jobQueue.cond, &pool->jobQueue.cond_lock);
	}
	pthread_mutex_unlock(&pool->jobQueue.cond_lock);
}

void*
ThreadMain(void* arg)
{
	ASSERTRC(arg != NULL, NULL);
	ThreadInfo *t_info = (ThreadInfo*)arg;
	ThreadPool *pool = t_info->threadPool;
	Job *job;

	t_info->tid = pthread_self();
	DIS_LOG("No.%d thread id:%u contained in pool(%s) created \n", t_info->id, (unsigned int)t_info->tid, pool->name);

	pthread_mutex_lock(&pool->threadQueue.rwlock);
	TAILQ_INSERT_TAIL(&pool->threadQueue.head, t_info, threadLink);
	pool->threadQueue.totalThreadCount++;
	pthread_mutex_unlock(&pool->threadQueue.rwlock);

	while(pool->keepalive) {
		WaitJob(pool);
		job = PullWork(pool);

		if(job != NULL) {
			DIS_LOG("No.%d pthread id:%u contained in pool(%s) start work!!!\n", t_info->id, t_info->id, pool->name);
			job->do_work(job->arg);
			if(job->arg)
				free(job->arg);
			free(job);
		}
	}

	pthread_mutex_lock(&pool->threadQueue.rwlock);
	TAILQ_REMOVE(&pool->threadQueue.head, t_info, threadLink);
	pool->threadQueue.totalThreadCount--;
	pthread_mutex_unlock(&pool->threadQueue.rwlock);

	DIS_LOG("No.%d pthread id:%ul contained in pool(%s) died\n", t_info->id, (unsigned int)pthread_self(), pool->name);
	return NULL;
}


/*
 * Initialize one thread
 * */
static MsgError
init_thread(int id, ThreadPool *pool)
{
	pthread_t tid;
	ThreadInfo *t_info;
	int ret;

	ASSERTRC(pool != NULL, ERROR);

	t_info = (ThreadInfo*)calloc(sizeof(ThreadInfo), 1);
	ASSERTRC(t_info != NULL, ERROR);

	ret = pthread_create(&tid, NULL, ThreadMain, t_info);
	ASSERTRC(ret == NO_ERROR, ERROR);

	t_info->id = id;
	t_info->threadPool = pool;

	return NO_ERROR;
}
/*
 * Initialize one thread pool
 * Parameter
 * 	num_threads: the number of threads contained in the pool
 * */
ThreadPool *
CreateThreadPool(char *pool_name, unsigned int num_threads)
{
	int i = 0;
	MsgError ret;
	ThreadPool *threadPool;

	ASSERTRC(pool_name != NULL, NULL);

	threadPool = calloc(sizeof(ThreadPool), 1);
	ASSERTRC(threadPool != NULL, NULL);
	threadPool->keepalive = 1;

	pthread_mutex_init(&threadPool->jobQueue.rwlock, NULL);
	pthread_mutex_init(&threadPool->jobQueue.cond_lock, NULL);
	pthread_mutex_init(&threadPool->threadQueue.rwlock, NULL);
	pthread_cond_init(&threadPool->jobQueue.cond, NULL);
	TAILQ_INIT(&threadPool->jobQueue.head);
	TAILQ_INIT(&threadPool->threadQueue.head);
	strncpy(threadPool->name, pool_name, POOL_NAME_LEN - 1);
	do {
		for(i = 0; i < num_threads; i++) {
			ret = init_thread(i, threadPool);
			ASSERTRC(ret == NO_ERROR, NULL);
		}

	} while(0);

	return threadPool;
}

/*
 * Push one work into one thread pool tail
 **/
void
PushWork(ThreadPool *pool, void* (*work)(void*), void *arg)
{
	int ret;
	Job *job = NULL;

	ASSERT(pool != NULL);
	ASSERT(work != NULL);

	job = (Job*)calloc(sizeof(Job), 1);
	ASSERT(job != NULL);

	job->do_work = work;
	job->arg = arg;

	pthread_mutex_lock(&pool->jobQueue.rwlock);
	TAILQ_INSERT_TAIL(&pool->jobQueue.head, job, jobLink);
	pool->jobQueue.count++;
	pthread_mutex_unlock(&pool->jobQueue.rwlock);

	PostJob(pool);
}

/*
 * Pull one work from the head of thread pool
 * */
Job*
PullWork(ThreadPool *pool)
{
	Job *job = NULL;

	pthread_mutex_lock(&pool->jobQueue.rwlock);
	job = TAILQ_FIRST(&pool->jobQueue.head);
	if(job) {
		TAILQ_REMOVE(&pool->jobQueue.head, job, jobLink);
		pool->jobQueue.count--;
	}
	pthread_mutex_unlock(&pool->jobQueue.rwlock);

	return job;
}

static void
freeJob(ThreadPool *pool)
{
	Job *job = NULL;
	pthread_mutex_lock(&pool->jobQueue.rwlock);
	while(!TAILQ_EMPTY(&pool->jobQueue.head)) {
		job = TAILQ_FIRST(&pool->jobQueue.head);
		TAILQ_REMOVE(&pool->jobQueue.head, job, jobLink);
		if(job->arg)
			free(job->arg);
		free(job);
	}
	pthread_mutex_unlock(&pool->jobQueue.rwlock);
	pthread_mutex_destroy(&pool->jobQueue.rwlock);
	pthread_mutex_destroy(&pool->jobQueue.cond_lock);
	pthread_cond_destroy(&pool->jobQueue.cond);
}

static void
cancelThread(ThreadPool *pool)
{
	ThreadInfo* t_info;
	pool->keepalive = 0;

	pthread_mutex_lock(&pool->jobQueue.cond_lock);
	pthread_cond_broadcast(&pool->jobQueue.cond);
	pthread_mutex_unlock(&pool->jobQueue.cond_lock);
	while(pool->threadQueue.totalThreadCount != 0);
}

void
WaitThread(ThreadPool *pool)
{
	int r;
	ThreadInfo* t_info;
	TAILQ_FOREACH(t_info, &pool->threadQueue.head, threadLink) {
		r = pthread_join(t_info->tid, NULL);
		if(r != 0) {
			DIS_LOG("%s\n", strerror(r));
		}
	}
}

void
DestroyThreadPool(ThreadPool *pool)
{
	ASSERT(pool != NULL);

	cancelThread(pool);
	freeJob(pool);
	DIS_LOG("pool(%s) is destroyed\n", pool->name);
	free(pool);
}

