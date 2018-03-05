
#pragma once

#include <Cpp.h>

using ThreadFunc = std::function<void(void)>;

class Thread {
	ThreadFunc m_func;	
	pthread_t m_pid;
	void Execute();
public:
	Thread(ThreadFunc func);
	void Join();
};

