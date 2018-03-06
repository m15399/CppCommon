
#pragma once

#include <CppCommon.h>

using JobFunc = std::function<void(void)>;

class Job {
public:
	JobFunc m_func;

	Job();
	Job(JobFunc func);
	void Execute();
};

class JobQueue {
	std::mutex m_mtx;
	std::list<Job> m_jobs;	
public:
	void Queue(Job job);
	void Queue(JobFunc func);
	void ExecuteLoop();
	void Execute(int numThreads);
};

