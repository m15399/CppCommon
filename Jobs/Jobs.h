
#pragma once

#include <Cpp.h>

using JobFunc = std::function<void(void)>;

class Job {
	JobFunc m_func;
public:
	Job();
	Job(JobFunc func);
	void Execute();
};

class JobQueue {
	std::mutex m_mtx;
	std::list<Job> m_jobs;	
public:
	void Queue(Job job);
	void ExecuteLoop();
	void Execute(int numThreads);
};

