#ifndef MSG_DIS_MAIN_H
#define MSG_DIS_MAIN_H

#include <stdio.h>

#define		KEY_MSG_BEGIN		"key"
#define 	EXIT_MSG		"exit"

typedef enum {
	NO_ERROR = 0,
	ERROR = 1,
}MsgError;

typedef enum {
	EMERGER_LOG = 1,
	CRITICAL_LOG,
	ERROR_LOG,
	NOTICE_LOG,
	INFO_LOG,
	DEBUG_LOG,
}DIS_LOG_LEVEL;

//#define		DIS_LOG(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#define DIS_LOG(format,...) printf(format, ##__VA_ARGS__)

#define ASSERTRC(exp, rc)  \
	do { \
		if(!(exp)) { \
			DIS_LOG("error %s, %d", __FUNCTION__, __LINE__); \
			return rc;\
		} \
	} while(0)

#define ASSERT(exp)  \
	do { \
		if(!(exp)) { \
			DIS_LOG("error %s, %d", __FUNCTION__, __LINE__); \
			return;\
		} \
	} while(0)

extern void
ProcessMsg(char* str);
extern void
InitDispatchCtx(void)

extern ThreadPool* key_thread_pool;
extern ThreadPool* other_thread_pool;

#endif
