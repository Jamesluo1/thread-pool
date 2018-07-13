#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <sys/queue.h>

typedef struct Job Job;
struct Job {
	void* (*do_work)(void*);
	void* arg;
	TAILQ_ENTRY(Job) jobLink;
};

typedef struct JobQueue JobQueue;
struct JobQueue {
	pthread_mutex_t rwlock;
	TAILQ_HEAD(JobQueueHead, Job) head;
	pthread_mutex_t cond_lock;
	pthread_cond_t cond;
	int count;
};

typedef struct thread_pool ThreadPool;

typedef struct ThreadInfo ThreadInfo;
struct ThreadInfo {
	int id;
	pthread_t tid;
	ThreadPool *threadPool;
	TAILQ_ENTRY(ThreadInfo) threadLink;
};

typedef struct ThreadQueue ThreadQueue;
struct ThreadQueue {
	pthread_mutex_t rwlock;
	TAILQ_HEAD(threadHead, ThreadInfo) head;
	int totalThreadCount;
};

#define POOL_NAME_LEN		20
struct thread_pool{
	char name[POOL_NAME_LEN];
	JobQueue jobQueue;
	ThreadQueue threadQueue;
	char keepalive;
};

extern ThreadPool *
CreateThreadPool(char *pool_name, unsigned int num_threads);
extern void
DestroyThreadPool(ThreadPool *pool);
extern void
PushWork(ThreadPool *pool, void*(*work)(void*), void *arg);
extern Job*
PullWork(ThreadPool *pool);
extern void
WaitThread(ThreadPool *pool);

#endif
