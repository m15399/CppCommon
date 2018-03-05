
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
	char output[1024];
	int loc = 0;

	constexpr const char* msg = "HelloWorld";	

	JobFunc helloFunc = [&mtx, &output, &loc](){
		for(int i = 0; i < strlen(msg);){
			{
				LOCK(mtx);
				for(int j = 0; j < 4; j++)
					output[loc++] = (char) msg[i++];
			}
			usleep(10000);
		}
	};

	JobQueue q;
	for(int i = 0; i < 10; i++)
		q.Queue(Job(helloFunc));

	q.Execute(80);

	string s(output);
	s = s.substr(0, strlen(msg));
	ASSERT_FALSE(s == string(msg));
}
