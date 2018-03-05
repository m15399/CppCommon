
#include <CppCommon.h>

Job::Job(){}

Job::Job(JobFunc func){
	m_func = func;
}

void Job::Execute(){
	m_func();
}

void JobQueue::Queue(Job job){
	LOCK(m_mtx);
	m_jobs.push_back(job);
}

void JobQueue::ExecuteLoop(){
	while(true){
		bool haveJob = false;
		Job nextJob;
		{
			LOCK(m_mtx);
			if(m_jobs.size() > 0){
				haveJob = true;
				nextJob = m_jobs.front();
				m_jobs.pop_front();
			}
		}
		if (haveJob){
			nextJob.Execute();
		} else {
			break;
		}
	}
}

void JobQueue::Execute(int numThreads){
	std::vector<Thread> threads;

	LogDebug(JobLogger, "JobQueue %p Enqueueing jobs...", this);

	for(int i = 0; i < numThreads; i++){
		threads.emplace_back([this](){ExecuteLoop();});
	}

	LogDebug(JobLogger, "JobQueue %p Waiting on jobs...", this);

	for(Thread thread : threads){
		thread.Join();
	}

	LogDebug(JobLogger, "JobQueue %p Done", this);
}

