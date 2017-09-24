#pragma once

#ifndef _ST_LOG_H__
#define __ST_LOG_H__


#ifdef  __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <aio.h>

typedef enum {
	YYJYLS,
	YYERROR
/*     DEBUG,  
    INFO,  
    WARN,  
    ERROR,  
    OFF  */ 
}LOG_LEVEL; 

//写日志
int stLog(LOG_LEVEL level,int pid,char* fmt,...);

//日志系统初始化
void log_init(char *logConfPath);

#ifdef  __cplusplus
}
#endif
#endif