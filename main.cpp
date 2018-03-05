
#include <CppCommon.h>

int main()
{
	JobFunc helloFunc = [](){
		const char* buf = "HelloWorld";	
		for(int i = 0; i < strlen(buf); i++){
			printf("%c", buf[i]);
			usleep(10000);
		}
	};
	JobQueue q;
	for(int i = 0; i < 10; i++)
		q.Queue(Job(helloFunc));

	q.Execute(8);
	printf("\n");

	return 0;
}

