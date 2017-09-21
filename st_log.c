#define  _CRT_SECURE_NO_WARNINGS 
#include "st_log.h"

/* 行缓冲大小 */
const static int MaxLine = 1024;
const int AIO_BUFFER_SIZE = 1024;

static char FILE_ABS_PATH[128];
static pthread_t t_1;
static long FILE_MAX_SIZE = 10*1024;
static int TNUM = 64; 
static int LOGNUM = 64;
static char LOG_NAME[64];
static pthread_mutex_t sMux; 
//fopen缓冲区大小
static int FOPEN_BUFF_SIZE = 1024;
//单条日志总长度
static int LOG_SIZE = 1024;
static int CONFLEN = 128;
//struct aiocb *cblist[1];
//typedef struct aiocb  aiocb_t;

int getValue(char*key,char*value){
	int len = 0;
	char*tempValue = NULL;
	memset(value,0,sizeof(value));
	int ret = intput_config_value(FILE_ABS_PATH,key,&tempValue,&len);
	snprintf(value,sizeof(value),"%s",tempValue);
	if(tempValue != NULL){
		free(tempValue);
		tempValue = NULL;
	}
	printf("getValue=================>%s\n",value);
	return ret;
}

//读键值
int intput_config_value(char *file_absolute_path, char *key, char **value, int *len) {

	int ret = 0;
    FILE *file = NULL; //文件流
    char lineBuf[MaxLine];//行缓冲
    char *tempP = NULL; // 临时字符串指针
    char *start = NULL; //键值开始位置
    char *end = NULL;  //键值结束位置
    char *tempValue = NULL;

    if (file_absolute_path == NULL || key == NULL || value == NULL) {
        ret = -1;
        return ret;
    }
    //读取文件
    file = fopen(file_absolute_path,"r");
    if (file == NULL) {
        ret = -2;
        return ret;
    }

    while (!feof(file))
    {
        memset(lineBuf,0,sizeof(lineBuf));
        //读取行
        tempP = fgets(lineBuf,MaxLine,file);
        
		if(*lineBuf == '#' || *lineBuf == ' '){
			continue;
		}
		
        //是否有"="
		tempP = strchr(lineBuf,'=');
        if (tempP == NULL) {
            //没有读取到键值
            continue;
        }
        //判断是否此行有包含我要的key
        tempP = strstr(lineBuf,key);
        if (tempP == NULL) {
            continue;
        }
        //获取值
        tempP = strchr(lineBuf, '=');
        //+1超过'='位置
        tempP = tempP + 1;
        while (1)
        {
            if (*tempP == ' ') {
                tempP++;
            }
            else {
                start = tempP;
                if (*start == '\n') { //表示只有键 没有键值的情况
                    ret = -3;
					if (file != NULL) {
						fclose(file);
						file = NULL;
					}
					return ret;
				}
                break;
            }
        }
        while (1) {
            if ((*tempP == ' ' || *tempP == '\n')) {
                break;
            }
            else {
                tempP++;
            }
        }
        end = tempP;
        //计算出value长度
        *len = end - start;
        tempValue = (char*)malloc((*len) * sizeof(char));
        memcpy(tempValue, start, (*len));
        *value = tempValue;
		//printf("tempValue:%s\n",*value);
		/* if(tempValue != NULL){
			free(tempValue);
			tempValue = NULL;
		} */
		if(*value == "" || *value == NULL){
			ret = 0;
		}else{
			ret = 1;
			//printf("\n========------>%s\n",*value);
			return ret;
		}
    }

    return ret;
}

//日志等级转换成字符串
char* log_level_2_str(LOG_LEVEL level){  
    switch (level)  
    {  
    case DEBUG:  
        return (char*)"DEBUG";  
    case INFO:  
        return (char*)"INFO";  
    case WARN:  
        return (char*)"WARN";  
    case ERROR:  
        return (char*)"ERROR";  
    case OFF:  
        return (char*)"OFF";  
    default:  
        return (char*)"UNKNOW";  
    }  
    return (char*)"UNKNOW";  
} 

//检测文件大小，如果过大获取新的日志文件名
void *lockLogfile(void *p){
	char logname[LOGNUM];
	int count = 0 ;
	while(1){
		//char *str = (char*)p;
		//在日志名后添加.log
		memset(logname, 0, sizeof(logname));
		snprintf(logname, LOGNUM ,"%s_%s.log","WARN",LOG_NAME);

		//计算长度
		long length = 0;
		FILE *fp = NULL;
		fp = fopen(logname, "rb");
		
		if (fp != NULL){
			fseek(fp, 0, SEEK_END);
			length = ftell(fp);
		}
		
		if (fp != NULL){
			fclose(fp);
			fp = NULL;
		}
		
		printf("111111111111====> %s\n",logname);
		
		if ( length > FILE_MAX_SIZE ){//检测到文件太大
			count++;
			printf("111111111111====> File is too large,start to split.\n");
			o_get_new_name(LOG_NAME,count);
		}
		sleep(3);
	}
}

//根据时间创建一个新日志名
int o_get_new_name(char* str,int i){
	memset(str, 0, sizeof(str));
	char timebuffer[TNUM];
	memset(timebuffer, 0, sizeof(timebuffer));
	get_local_time(timebuffer);
	snprintf(str,TNUM,"%s_%d",timebuffer,i);
	return 0;
}

//获取本地时间
int get_local_time(char * buffer){
	memset(buffer, 0, sizeof(buffer));
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	snprintf(buffer, TNUM ,"%04d-%02d-%02d_%02d-%02d-%02d",
			(timeinfo->tm_year+1900), timeinfo->tm_mon, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	return 0;
}

//在日志文件后添加.log
void snp_str_log(LOG_LEVEL level,char *sour,char *dest){
	memset(dest, 0, sizeof(dest));
	snprintf(dest, LOGNUM ,"%s_%s.log",log_level_2_str(level),sour);
}

//AIO回调函数
void aio_return_handl(sigval_t sigval){
	struct aiocb *prd;
    prd = (struct aiocb *)sigval.sival_ptr;
    
	if(aio_error(prd) == 0){
		int i = aio_return(prd);
		printf("aio_return %d\n",i);
	}else{
		printf("aio_error\n");
	}
	
	if((*prd).aio_fildes){
		close((*prd).aio_fildes);
		printf("is closing...\n");
	}else{
		printf("file closed\n");
	}
}


//异步写文件
int aio_w_file(char * const filename,char * const str){
	
	struct aiocb   my_aio;
	struct aiocb *cblist[1];
	//struct aiocb  *my_aio_list[1] = {&my_aio};
	
	memset((char*)&my_aio,0,sizeof(struct aiocb));
	memset((char*)cblist,0,sizeof(cblist));
	cblist[0] = &my_aio;
	
	int fd = open(filename, O_WRONLY | O_APPEND | O_CREAT , 0666);
	if (-1 == fd){
		printf("Error %d: Failed to open file\n", errno);
		return 0;
	}
	
	if(fd < 0){
		//printf("---------9-20----2---%s\n",filename);
		return 2;
	}
	
	//wr.aio_buf = (char *)malloc(AIO_BUFFER_SIZE + 1);
	//if(wr.aio_buf == NULL) perror("buf");
	
	my_aio.aio_fildes = fd;
    my_aio.aio_buf = str;
	my_aio.aio_nbytes = strlen(str);

	my_aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
    my_aio.aio_sigevent.sigev_notify_function = aio_return_handl;
    my_aio.aio_sigevent.sigev_notify_attributes = NULL;
    my_aio.aio_sigevent.sigev_value.sival_ptr = &my_aio;
	
	//int ret = aio_write((void *)&wr);
	if(aio_write((void *)&my_aio) < 0){
		printf("---------9-20---3----%s\n",filename);
		return 3;	
	}
	
	if( aio_suspend((const struct aiocb * const*)cblist, 1, NULL) < 0 ){
		printf("---------9-20---4----%s\n",filename);
		return 4;
	}
	//sleep(1);
	return 1;
}

//封装1-写文件
int o_write_file(char* filename,char* buffer){
	
	char s_buf[LOG_SIZE];
	char now[TNUM];
	memset(now, 0, TNUM);
	memset(s_buf, 0, LOG_SIZE);
	
	get_local_time(now);
	
	snprintf(s_buf,LOG_SIZE,"%s_%s",now,buffer);
	
	int f = aio_w_file(filename,s_buf);
	
	if(f == 1){
		//printf("异步写入成功！\n");
		return 0;
	}
	else if(f == 2)printf("打开日志文件失败！\n");
	else if(f == 3)printf("异步写入失败！\n");
	
	return 0;
}

//封装2-写文件
void write_log_file(LOG_LEVEL level,char* buffer){
	char logname[LOGNUM];
	snp_str_log(level,LOG_NAME,logname);
	o_write_file(logname,buffer);
}

//封装终-记日志
int stLog(LOG_LEVEL level,int pid,char* fmt,...){
	
	//char* str ,
	char buffer[LOG_SIZE];
	memset(buffer, 0, sizeof(buffer));
	
	snprintf(buffer, sizeof(buffer) ," %s ==> PID_%d =>",log_level_2_str(level), pid);
	//snprintf(buffer, sizeof(buffer) ," %s ==> ",log_level_2_str(level));
	//strcat(buffer);
	
	
	va_list ap;
	va_start(ap,fmt);
	
	char*p = fmt;
	while(*p){
		if(*p == '%' && *(p+1) == 'd'){
			char num[16];
			memset(num,0,sizeof(num));
			snprintf(num,sizeof(num),"%d",va_arg(ap, int));
			strcat(buffer,num);
		}
		else if(*p == '%' && *(p+1) == 's'){
			char num[128];
			memset(num,0,sizeof(num));
			snprintf(num,sizeof(num),"%s",va_arg(ap, char*));
			strcat(buffer,num);
		}
		p++;
	}
	va_end(ap);
	
	strcat(buffer,"\n");
	write_log_file(level,buffer);
	return 0;
}

//日志系统初始化
void log_init(char * log_conf){

	memcpy(FILE_ABS_PATH,log_conf,strlen(log_conf));
	
	char buffer[CONFLEN];
	if(getValue("FILE_MAX_SIZE",buffer) == 1){
		FILE_MAX_SIZE = atol(buffer);
		printf("FILE_MAX_SIZE_OK---->%ld\n",FILE_MAX_SIZE);
	}
	if(getValue("TNUM",buffer) == 1){
		TNUM = atoi(buffer);
		printf("TNUM_OK---->%d\n",TNUM);
	}
	if(getValue("LOGNUM",buffer) == 1){
		LOGNUM = atoi(buffer);
		printf("LOGNUM_OK---->%s\n",buffer);
	}
	if(getValue("FOPEN_BUFF_SIZE",buffer) == 1){
		FOPEN_BUFF_SIZE = atoi(buffer);
		printf("FOPEN_BUFF_SIZE_OK---->%d\n",FOPEN_BUFF_SIZE);
	}
	if(getValue("LOG_SIZE",buffer) == 1){
		LOG_SIZE = atoi(buffer);
		printf("LOG_SIZE_OK---->%d\n",LOG_SIZE);
	}

	get_local_time(LOG_NAME);
	int x = 1;
	if(pthread_create(&t_1,NULL,lockLogfile,&x)){
		printf("funcA_t create failed!\n"); 
	}
}