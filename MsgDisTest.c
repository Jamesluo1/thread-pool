/*
 * This file presents how to use thread pool. and hardcode some test statistics to verify it
 * */
#include "MsgDis.h"
#include "ThreadPool.h"

#define		INPUT_STR_LEN		2048

/* Test statistics */
static char *test[]={"key1@3$","key2222", "key333","key444", "56622113","ggggg","hhhhhhhh","llllll","key555",
"key66", "key99","key0000","sssssss","ggggg","hhhhhhhh","llllll",};

void
main(int argc, char* argv[])
{
	int i,k = 0;
	char* buf;

	setvbuf(stdout, NULL, _IONBF, 0);
	InitDispatchCtx();
	sleep(2);

	for(k = 0; k  < 100;k++) {
		for(i = 0 ; i < sizeof(test)/sizeof(char*); i++) {
			buf = calloc(1, INPUT_STR_LEN);
			memset(buf, 0x0, sizeof(INPUT_STR_LEN));
			strcpy(buf, test[i]);
			ProcessMsg(buf);
		}
	}
	sleep(5);
	buf = calloc(1, INPUT_STR_LEN);
	strcpy(buf, EXIT_MSG);
	ProcessMsg(buf);
}
