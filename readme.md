README
===========================
1、`main.c`      为测试用代码，可编译运行，`CentOS6.9`编译成功，命令:  `gcc -o main main.c st_log.c -lrt -lpthread`  执行`./main`二进制文件即可，查看配置文件中路径，即可看到日志。
***
2、`st_log.c`    为日志插件实现代码，最好不要变动。
***
3、`st_log.h`    为日志插件头文件，要使用插件的时候，直接引用该头文件即可： `#include <"st_log.h">`  一行即可。
***
4、`log.config`  为配置文件，可用文本编辑器打开配置。
***

### 当需要该插件时引用 `st_log.h` 头文件

```c
#include <"st_log.h">

int main(){
	
	log_init("配置文件路径");  //一定要有，这是日志插件初始化，读取配置的进程，先启动才能运行
	
	stLog(YYJYLS,"%s%s%d","string1","string2","number");// 日志写入函数，写入等级有两个  一个为 YYJYLS  一个为 YYERROR 后面的 "%s%d" 为格式化输出，有几个%s 后面就可跟几个字符串变量。现在仅支持字符串 char* 和 整型int。
	
	return 0
}
```