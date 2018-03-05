
#include <CppCommon.h>

static void* ThreadExecuteFunction(void* arg){
	ThreadFunc* func = (ThreadFunc*) arg;
	(*func)();
	return nullptr;
}

Thread::Thread(ThreadFunc func){
	m_func = func;
	Execute();
}

void Thread::Execute(){
	if (pthread_create(&m_pid, NULL, &ThreadExecuteFunction, &m_func)){
		FatalSz("Thread creation failed");
	}
}

void Thread::Join(){
	if (pthread_join(m_pid, NULL)){
		FatalSz("Thread join failed");
	}
}

