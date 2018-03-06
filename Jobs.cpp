
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

void JobQueue::Queue(JobFunc func){
	Queue(Job(func));
}

void JobQueue::ExecuteLoop(){
	while(true){
		bool haveJob = false;
		Job nextJob;
		{
			LOCK(m_mtx);
			LogDebug(JobLogger, "%p Looking for job", this);
			if(m_jobs.size() > 0){
				haveJob = true;
				nextJob = m_jobs.front();
				m_jobs.pop_front();
				LogDebug(JobLogger, "%p Found a job", this);
			}
		}
		if (haveJob){
			LogDebug(JobLogger, "%p Executing a job", this);
			nextJob.Execute();
		} else {
			LogDebug(JobLogger, "%p No more jobs", this);
			break;
		}
	}
}

void JobQueue::Execute(int numThreads){
	std::vector<Thread> threads;

	LogDebug(JobLogger, "%p Enqueueing jobs...", this);

	for(int i = 0; i < numThreads; i++){
		LogDebug(JobLogger, "%p Enqueueing job %d", this, i);
		threads.emplace_back([this](){ExecuteLoop();});
	}

	LogDebug(JobLogger, "%p Waiting on jobs...", this);

	for(Thread thread : threads){
		thread.Join();
	}

	LogDebug(JobLogger, "%p Done", this);
}


TEST(Job, Basic){
	int x = 0;
	Job j([&x](){x=4;});
	j.Execute();
	ASSERT_EQ(x, 4);
}

TEST(JobQueue, Basic){
	int x = 0;

	JobQueue q;
	q.Queue([&x](){x = 4;});
	q.Execute(1);

	ASSERT_EQ(x, 4);
}

TEST(JobQueue, Parallel){
	std::mutex mtx;
	volatile char output[4096];
	volatile int loc = 0;

	constexpr const char* msg = "HelloWorld";	

	JobFunc helloFunc = [&mtx, &output, &loc](){
		LogDebugSz(JobLogger, "doin stuff");
		for(size_t i = 0; i < strlen(msg);){
			{
				LOCK(mtx);
				for(int j = 0; j < 4; j++)
					output[loc++] = (char) msg[i++];
			}
			usleep(10000);
		}
	};

	JobQueue q;
	for(int i = 0; i < 2; i++)
		q.Queue(Job(helloFunc));

	q.Execute(4);

	char* o = (char*) output;
	string s(o);
	s = s.substr(0, strlen(msg));
	ASSERT_FALSE(s == string(msg));
}

