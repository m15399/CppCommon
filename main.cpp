
#include <CppCommon.h>

bool CheckVar(string name){
	char* testVar = secure_getenv(name.c_str());
	return testVar && strlen(testVar) > 0;
}

int main(){
	int status = 0;

	if(CheckVar("TEST")){
		bool loop = CheckVar("LOOP");
		while(true){

			status = RunTests();

			if(status != 0 || !loop)
				break;
		}
	}

	return status;
}

