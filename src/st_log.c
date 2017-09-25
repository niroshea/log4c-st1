#define  _CRT_SECURE_NO_WARNINGS 
#include "st_log.h"

//配置文件键值缓冲大小
const static int MaxLine = 1024;
//日志文件路径，请在配置文件中配置，覆盖默认设置
static char LOG_FILE_PATH[256] = "/tmp/";
//使用该插件的应用名称，在配置文件中配置
static char APP_NAME[256] = "appName";
//日志的配置文件路径
static char LOG_CONF_PATH[128] = "/tmp";
//日志初始化线程
static pthread_t t_1;
//日志文件大小（MB）
static int FILE_SIZE_NUM = 1;
//形成M的样子
static long FILE_MAX_SIZE = 1024*1024;
//获取本地时间，字符串大小
static int TNUM = 64; 
//日志文件名称（绝对路径）大小
static int LOGNUM = 256;
//本地时间
static char LOG_TIME_JYLS[64];
static char LOG_TIME_ERROR[64];
//
static char LOG_ABS_PATH_JYLS[256] = "/tmp";
static char LOG_ABS_PATH_ERROR[256] = "/tmp";
//单条日志总长度
static int LOG_SIZE = 1024;
//键值长度
static int CONFLEN = 128;
//传入线程的参数
typedef struct threadArg{
	int ts;
}tArg;

void *moLogfile(void *arg);

//日志等级转换成字符串
char *log_level_2_str(LOG_LEVEL level){  
    switch (level)  
    {  
    case YYJYLS:  
        return (char*)"YYJYLS";  
    case YYERROR:  
        return (char*)"YYERROR";  
    default:  
        return (char*)"UNKNOW";  
    }  
    return (char*)"UNKNOW";  
}

//日志系统初始化
void log_init(char * logConfPath){

	//获取日志配置文件路径
	memcpy(LOG_CONF_PATH,logConfPath,strlen(logConfPath));
	memset(LOG_ABS_PATH_JYLS, 0, LOGNUM);
	memset(LOG_ABS_PATH_ERROR, 0, LOGNUM);
	char buffer[CONFLEN];
	if(getValue("LOG_FILE_PATH",buffer) == 1){
		int i,j;
		char str[LOGNUM];
		memset(str,0,LOGNUM);
		memset(LOG_FILE_PATH,0,LOGNUM);
		for(i = 0,j = 0;i < strlen(buffer);i++){
			if(buffer[i] == ' '){
				continue;
			}else{
				str[j] = buffer[i];
				j++;
			}
		}
		str[j] = '\0';
		snprintf(LOG_FILE_PATH,LOGNUM,"%s%s",str,"/");
		//printf("LOG_FILE_PATH_OK---->%s\n",LOG_FILE_PATH);
	}
	if(getValue("APP_NAME",buffer) == 1){
		memset(APP_NAME,0,LOGNUM);
		snprintf(APP_NAME,LOGNUM,"%s",buffer);
		//printf("APP_NAME_OK---->%s\n",APP_NAME);
	}

	if(getValue("FILE_SIZE_NUM",buffer) == 1){
		int tmp = atoi(buffer);
			
		if(tmp > 0)FILE_SIZE_NUM = tmp;
		
		//printf("FILE_SIZE_NUM_OK---->%d\n",FILE_SIZE_NUM);
		
	}
	get_local_time(LOG_TIME_JYLS);
	get_local_time(LOG_TIME_ERROR);

	//tArg *x;
	//x->ts = 36;
	int x = 1;
	if(pthread_create(&t_1,NULL,moLogfile,&x))perror("funcA_t create failed!\n");

	char log_tmp_path[LOGNUM]; 
	char log_tmp_path2[LOGNUM];
	memset(log_tmp_path,0,LOGNUM);
	memset(log_tmp_path2,0,LOGNUM);
	snprintf(log_tmp_path,LOGNUM,"%s%s",LOG_FILE_PATH,"YYERROR/");
	snprintf(log_tmp_path2,LOGNUM,"%s%s",LOG_FILE_PATH,"YYJYLS/");

	if( access(log_tmp_path,F_OK) == -1)createDir(log_tmp_path);
	if( access(log_tmp_path2,F_OK) == -1)createDir(log_tmp_path2);
}

//封装终-记日志01
int stLog(LOG_LEVEL level,char* fmt,...){
	
	char buffer[LOG_SIZE];
	memset(buffer, 0, LOG_SIZE);
	
	snprintf(buffer, LOG_SIZE ," %s == ",log_level_2_str(level));
	
	va_list ap;
	va_start(ap,fmt);
	
	char*p = fmt;
	while(*p){
		if(*p == '%' && *(p+1) == 'd'){
			char num[16];
			memset(num,0,sizeof(num));
			snprintf(num,sizeof(num),"%d",va_arg(ap, int));
			strcat(buffer,num);
		}else if(*p == '%' && *(p+1) == 's'){
			char num[LOGNUM];
			memset(num,0,LOGNUM);
			snprintf(num,LOGNUM,"%s",va_arg(ap, char*));
			strcat(buffer,num);
		}
		p++;
	}
	va_end(ap);
	
	strcat(buffer,"\n");
	o_write_file(level,buffer);
	return 0;
}


//封装终-记日志 for test
int stLog_t(LOG_LEVEL level,int pid,char* fmt,...){
	
	//char* str ,
	char buffer[LOG_SIZE];
	memset(buffer, 0, LOG_SIZE);
	
	snprintf(buffer, LOG_SIZE ," %s ==> PID_%d =>",log_level_2_str(level), pid);
	
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
			char num[LOGNUM];
			memset(num,0,LOGNUM);
			snprintf(num,LOGNUM,"%s",va_arg(ap, char*));
			strcat(buffer,num);
		}
		p++;
	}
	va_end(ap);
	
	strcat(buffer,"\n");
	o_write_file(level,buffer);
	return 0;
}

//封装1-写文件
int o_write_file(LOG_LEVEL level,char* buffer){
	
	char s_buf[LOG_SIZE];
	memset(s_buf, 0, LOG_SIZE);
	
	char now[TNUM];
	memset(now, 0, TNUM);
	
	get_local_time(now);
	
	snprintf(s_buf,LOG_SIZE,"%s %s",now,buffer);
	
	int f = -1;
	if(level == YYJYLS){
		f = aio_w_file(LOG_ABS_PATH_JYLS,s_buf);		
	}else if(level == YYERROR){
		f = aio_w_file(LOG_ABS_PATH_ERROR,s_buf);
	}else return 3;

	if(f == 1){
		return 0;
	}else{
		return 2;
	}
	return 2;
}

//AIO回调函数
void aio_return_handl(sigval_t sigval){
	struct aiocb *prd;
    prd = (struct aiocb *)sigval.sival_ptr;
    
	if(aio_error(prd) == 0)aio_return(prd);
	//关闭文件描述符
	if((*prd).aio_fildes)close((*prd).aio_fildes);
}


//异步写文件
int aio_w_file(char * const filename,char * const str){
	
	struct aiocb   my_aio;
	struct aiocb *cblist[1];
	
	memset((char*)&my_aio,0,sizeof(struct aiocb));
	memset((char*)cblist,0,sizeof(cblist));
	cblist[0] = &my_aio;
	
	int fd = open(filename, O_WRONLY | O_APPEND | O_CREAT , 0644);
	if(fd < 0)return 2;
	
	my_aio.aio_fildes = fd;
	my_aio.aio_buf = str;
	my_aio.aio_nbytes = strlen(str);

	my_aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
	my_aio.aio_sigevent.sigev_notify_function = aio_return_handl;
	my_aio.aio_sigevent.sigev_notify_attributes = NULL;
	my_aio.aio_sigevent.sigev_value.sival_ptr = &my_aio;
	
	//开始写文件
	if(aio_write((void *)&my_aio) < 0)return 3;	
	//等待文件返回
	if( aio_suspend((const struct aiocb * const*)cblist, 1, NULL) < 0 )return 4;
	
	return 1;
}

//级联创建文件夹
int createDir(const char *sPathName){
	char DirName[256];  
	strcpy( DirName,sPathName);  
	int i,len = strlen(DirName);  
	
	if(DirName[len-1]!='/')strcat(DirName, "/");  
	len = strlen(DirName);  
	
	for(i=1;   i<len;   i++){  
		if(DirName[i]=='/'){
			DirName[i] = 0;
			if( access(DirName, W_OK) != 0   ){  
				if(mkdir(DirName,   0644)==-1){   
					//perror("mkdir   error");   
					return   -1; 
				}
			}  
			DirName[i] = '/';  
		}  
	}  
	return 0;  
} 

//获取本地时间
int get_local_time(char * buffer){
	
	memset(buffer, 0, TNUM);
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	snprintf(buffer, TNUM ,"%04d-%02d-%02d_%02d-%02d-%02d",
			(timeinfo->tm_year+1900), timeinfo->tm_mon, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	return 0;
}

//获取键值
int getValue(char*key,char*value){
	int len = 0;
	char*tempValue = NULL;
	memset(value,0,CONFLEN);
	int ret = intput_config_value(LOG_CONF_PATH,key,&tempValue,&len);
	snprintf(value,CONFLEN,"%s",tempValue);
	if(tempValue != NULL){
		free(tempValue);
		tempValue = NULL;
	}
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
        
		if(*lineBuf == '#' || *lineBuf == ' ' || *lineBuf == '\n' || *lineBuf == '\r'){
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
		if(*tempP == ' ' || *tempP == '\n' || *tempP == '\r')break;
		tempP++;
        }
        end = tempP;
	*tempP = '\0';
        //计算出value长度
        *len = end - start + 1;
        tempValue = (char*)malloc((*len) * sizeof(char));
	int s = 0;
	for(;s < *len ; s++){
		tempValue[s] = start[s];
	}
	*value = tempValue;
	if(*value == "" || *value == NULL){
		ret = 0;
	}else{
		ret = 1;
		return ret;
	}
    }
    return ret;
}

//检测文件大小，如果过大获取新的日志文件名
void *moLogfile(void *arg){
	int count1 = 1 ;
	int count2 = 1 ;
	while(1){
		//该线程函数管理日志名,文件夹是否存在，是否被误删
		//printf("start!\n");
		memset(LOG_ABS_PATH_JYLS,0,LOGNUM);
		memset(LOG_ABS_PATH_ERROR,0,LOGNUM);
		
		snprintf(LOG_ABS_PATH_JYLS, LOGNUM ,"%s%s",LOG_FILE_PATH,"YYJYLS/");
		snprintf(LOG_ABS_PATH_ERROR, LOGNUM ,"%s%s",LOG_FILE_PATH,"YYERROR/");
		
		//printf("LOG_ABS_PATH_JYLS==========1======>%s\n",LOG_ABS_PATH_JYLS);
		//printf("LOG_ABS_PATH_ERROR=========1====>%s\n",LOG_ABS_PATH_ERROR);
		
		if( access(LOG_ABS_PATH_JYLS,F_OK) == -1 )createDir(LOG_ABS_PATH_JYLS);
		if( access(LOG_ABS_PATH_ERROR,F_OK) == -1 )createDir(LOG_ABS_PATH_ERROR);		
		
		snprintf(LOG_ABS_PATH_JYLS, LOGNUM ,"%s%s%s_%s_%s_%04d.log",LOG_FILE_PATH,"YYJYLS/",APP_NAME,"YYJYLS",LOG_TIME_JYLS,count1);
		snprintf(LOG_ABS_PATH_ERROR, LOGNUM ,"%s%s%s_%s_%s_%04d.log",LOG_FILE_PATH,"YYERROR/",APP_NAME,"YYERROR",LOG_TIME_ERROR,count2);
		//snp_str_log(loglevel,LOG_TIME,logname);
		//printf("LOG_ABS_PATH_JYLS========2========>%s\n",LOG_ABS_PATH_JYLS);
		//printf("LOG_ABS_PATH_ERROR========2=====>%s\n",LOG_ABS_PATH_ERROR);
		//abort();
		
		//计算长度
		long length1 = 0;
		long length2 = 0;
		FILE *fp = NULL;
		
		fp = fopen(LOG_ABS_PATH_JYLS, "rb");
		if (fp != NULL){
			fseek(fp, 0, SEEK_END);
			length1 = ftell(fp);
			fclose(fp);
			fp = NULL;
		}
		
		fp = fopen(LOG_ABS_PATH_ERROR,"rb");
		if (fp != NULL){
			fseek(fp, 0, SEEK_END);
			length2 = ftell(fp);
			fclose(fp);
			fp = NULL;
		}
		
		//printf("==============> File is too large,start to split.\n");
		
 		//tArg *x;
		//x = (tArg *)arg;

		//printf("-=s-df-sdfx%s\n",LOG_ABS_PATH_JYLS);
		//printf("-=s-df-sdfx%s\n",LOG_ABS_PATH_ERROR);
		//printf("====================>%d\n", 1);
		
		if ( length1 > FILE_SIZE_NUM * FILE_MAX_SIZE ){//检测到文件太大
			if(count1 > 9999)count1 = 1;else count1++;
			get_local_time(LOG_TIME_JYLS);
		}
		if ( length2 > FILE_SIZE_NUM * FILE_MAX_SIZE ){//检测到文件太大
			if(count2 > 9999)count2 = 1;else count2++;
			get_local_time(LOG_TIME_ERROR);
		}
		sleep(5);
	}
}
