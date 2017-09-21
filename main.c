#include "st_log.h"
//#include <sys/stat.h> 
//static pthread_t t_1;  
//static pthread_t t_2;

void *funcA(void *p){
	int count = *((int*)p);
	while(1){
		stLog(WARN,count,"%s%s","zhe shi yi tiao WARN!=================================================================================","hfdkjahkfhakshdfjk");
		sleep(1);
	}
}

int main(int argc,char** argv){
	log_init("log.config");
	int a = 16;
	int count = 3;
	
	int i = 0;
	for(; i < 10 ; i++){
		pthread_t t_1; 
		int *p = malloc(sizeof(int));
		*p = i;
		if(pthread_create(&t_1,NULL,funcA,(void*)p)){
			printf("funcA_t create failed!\n"); 
		}
	}
	
	while(1){
		sleep(5);
	}
	return 0;
}