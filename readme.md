1��main.c      Ϊ�����ô��룬�ɱ������У�CentOS6.9����ɹ�������:  gcc -o main main.c st_log.c -lrt -lpthread  ִ��main�������ļ����ɣ��鿴�����ļ���·�������ɿ�����־
2��st_log.c    Ϊ��־���ʵ�ִ��룬��ò�Ҫ�䶯
3��st_log.h    Ϊ��־���ͷ�ļ���Ҫʹ�ò����ʱ��ֱ�����ø�ͷ�ļ����ɣ� #include <"st_log.h">  һ�м���
4��log.config  Ϊ�����ļ��������ı��༭�������á�


����Ҫ�ò��ʱ���� st_log.h ͷ�ļ�


#include <"st_log.h">

int main(){
	
	log_init("�����ļ�·��");  //һ��Ҫ�У�������־�����ʼ������ȡ���õĽ��̣���������������
	
	stLog(YYJYLS,"%s%s%d","string1","string2","number");// ��־д�뺯����д��ȼ�������  һ��Ϊ YYJYLS  һ��Ϊ YYERROR ����� "%s%d" Ϊ��ʽ�������
	
	return 0
}