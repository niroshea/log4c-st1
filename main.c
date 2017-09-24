#include "st_log.h"

void *funcA(void *p){
	int count = *((int*)p);
	while(1){
		stLog(YYJYLS,count,"%s%s%d","zhe shi yi tiao YYJYLS!==================AHSDKFJHAKJSDHFJKASDF65456454=====================","=======hfdkjahkfhakshdfjk===",count);
		sleep(1);
	}
}

void *funcB(void *p){
	int count = *((int*)p);
	while(1){
		stLog(YYERROR,count,"%s%s%d","zhe shi yi tiao YYERROR!=========SDF88888888888888888888888888888888888888888888888888==========","=======0hf6545466dsdfsdxxxx==",count);
		sleep(1);
	}
}

int main(int argc,char** argv){
	log_init("log.config");
		
	int i = 0;
	for(; i < 1000 ; i++){
		pthread_t t_1; 
		int *p = malloc(sizeof(int));
		*p = i;
		if(pthread_create(&t_1,NULL,funcA,(void*)p)){
			printf("funcA_t create failed!\n"); 
		}
	}
		
	int j = 1000;
	for(;j < 2000 ; j++){
		pthread_t t_2;
		int *m = malloc(sizeof(int));
		*m = j;
		if(pthread_create(&t_2,NULL,funcB,(void*)m)){
			printf("sdfjksjdfkjksdjfk!");
		}
	}

	
	while(1){
		sleep(5);
	}
	return 0;
}