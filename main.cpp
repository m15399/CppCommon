
#include <CppCommon.h>

int main()
{
	char* testVar = secure_getenv("TEST");
	if(testVar && strlen(testVar) > 0){
		int status = RunTests();
		return status;
	}

	return 0;
}

