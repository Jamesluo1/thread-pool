#include "../include/CppUTest/TestHarness.h"
#include "../include/CppUTest/CommandLineTestRunner.h"
extern "C" {
#include "../ThreadPool.h"
#include <string.h>
#include <stdlib.h>
}

TEST_GROUP(ThreadPool)
{
	void setup()
	{
	}

	void teardown()
	{


	}
};

TEST(ThreadPool, CreateAndDestoryPool)
{
	ThreadPool *pool = NULL;
	pool = CreateThreadPool("key", 3);
	STRCMP_EQUAL(pool->name, "key");
	LONGS_EQUAL(pool->threadQueue.totalThreadCount, 3);
	DestroyThreadPool(pool);
	CHECK_FALSE(pool == NULL)
}

void* test(void* arg)
{

}

TEST(ThreadPool, JobQueue)
{
	char *arg = (char*)malloc(strlen("test") +1);
	ThreadPool *pool = NULL;
	if(arg == NULL) {
		FAIL("malloc failed");
	}
	strncpy(arg, "test", strlen("test"));
	pool = CreateThreadPool("other", 0);
	CHECK_TEXT(TAILQ_EMPTY(&pool->jobQueue.head), "job queue should empty");
	PushWork(pool, test, (void*)arg);
	CHECK_TEXT(pool->jobQueue.count == 1, "number of job is 1");
	PushWork(pool, test, (void*)arg);
	CHECK_TEXT(pool->jobQueue.count == 2, "number of job is 2");
	PullWork(pool);
	CHECK_TEXT(pool->jobQueue.count == 1, "number of job is 1");
	PullWork(pool);
	CHECK_TEXT(pool->jobQueue.count == 0, "number of job is 0");
}

TEST(ThreadPool, DispatchJob)
{
	int i = 0;
	ThreadPool *pool_key = NULL;
	pool_key = pool = CreateThreadPool("other", 1);
	pool = CreateThreadPool("other", 0);

	char* test[]= {"key!!2@", "k2334"};
	for(i = 0; i < sizeof(test); i++) {
		char *arg = (char*)malloc(strlen(test[i]) +1);
		if(arg == NULL) {
			FAIL("malloc failed");
		}
	}
	ThreadPool *pool = NULL;
	strncpy(arg, "test", strlen("test"));
}

int main(int ac, char** av)
{
    return CommandLineTestRunner::RunAllTests(ac, av);
}
