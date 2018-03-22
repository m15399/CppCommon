
#include <CppCommon.h>

// Node

template<typename T>
GraphNode<T>::GraphNode(const T& data):
	m_data(data){

}

template<typename T>
void GraphNode<T>::AddChild(GraphNode* child){
	m_children.push_back(child);
	child->m_parents.push_back(this);
}

template<typename T>
void GraphNode<T>::RemoveChild(GraphNode* child){
	m_children.remove(child);
	child->m_parents.remove(this);
}

template<typename T>
void GraphNode<T>::RemoveAllParents(){
	for(GraphNode* parent : m_parents){
		parent->m_children.remove(this);
	}
	m_parents.clear();
}

template<typename T>
void GraphNode<T>::RemoveAllChildren(){
	for(GraphNode* child : m_children){
		child->m_parents.remove(this);
	}
	m_children.clear();
}

template<typename T>
bool GraphNode<T>::HasParents(){
	return !m_parents.empty();
}

template<typename T>
bool GraphNode<T>::HasChildren(){
	return !m_children.empty();
}

// Graph

template<typename T>
GraphNode<T>* Graph<T>::CreateNode(const T& data){
	m_nodes.emplace_back(data);
	return &m_nodes.back();
}

template<typename T>
void Graph<T>::DeleteNode(GraphNode<T>* node){
	node->RemoveAllChildren();
	node->RemoveAllParents();

	m_nodes.remove_if([node](const GraphNode<T>& value){ return &value == node; });
}

// Tests

TEST(Graph, Basic){
	using G = Graph<int>;
	using N = GraphNode<int>;
	G g;
	N* a = g.CreateNode(0);
	N* b = g.CreateNode(1);
	N* c = g.CreateNode(2);

	ASSERT_FALSE(a->HasParents());
	ASSERT_FALSE(a->HasChildren());

	a->AddChild(b);
	ASSERT_FALSE(a->HasParents());
	ASSERT_TRUE(a->HasChildren());
	ASSERT_TRUE(b->HasParents());
	ASSERT_FALSE(b->HasChildren());

	a->RemoveChild(b);
	ASSERT_FALSE(a->HasChildren());
	ASSERT_FALSE(b->HasParents());

	a->AddChild(b);
	b->AddChild(c);
	ASSERT_TRUE(a->HasChildren());
	ASSERT_TRUE(c->HasParents());
	g.DeleteNode(b);
	ASSERT_FALSE(a->HasChildren());
	ASSERT_FALSE(c->HasParents());
}

template<typename T>
class TaskNode {
public:

	bool m_running = false;
	T m_data;	
	void* m_gNode = nullptr;

	TaskNode(const T& data);
};

template<typename T>
class TaskGraph {
	using GN = GraphNode<TaskNode<T>>;

public:

	TaskGraph(std::function<void(T&)> executeFunction);

	TaskNode<T>* CreateTaskNode(const T& data);
	void DependsOn(TaskNode<T>* a, TaskNode<T>* b);

	bool RunOneTask();
	void RunTaskLoop(int numThreads);

protected:
	TaskNode<T>* GrabNextTask();
	void CompleteTask(TaskNode<T>* task);

	Graph<TaskNode<T>> m_graph;
	std::function<void(T&)> m_executeFunction;
	mutex m_mtx;
};


template<typename T>
TaskNode<T>::TaskNode(const T& data):
	m_data(data){
	
}


template<typename T>
TaskGraph<T>::TaskGraph(std::function<void(T&)> executeFunction):
	m_executeFunction(executeFunction){
	
}

template<typename T>
TaskNode<T>* TaskGraph<T>::CreateTaskNode(const T& data){
	LOCK(m_mtx);

	GN* gNode = m_graph.CreateNode(TaskNode<T>(data));
	TaskNode<T>* tn = &gNode->m_data;
	tn->m_gNode = gNode;
	return tn;
}

template<typename T>
void TaskGraph<T>::DependsOn(TaskNode<T>* a, TaskNode<T>* b){
	LOCK(m_mtx);

	GN* agn = (GN*)a->m_gNode;
	GN* bgn = (GN*)b->m_gNode;
	bgn->AddChild(agn);
}

template<typename T>
bool TaskGraph<T>::RunOneTask(){
	TaskNode<T>* task = GrabNextTask();
	if (task){
		m_executeFunction(task->m_data);
		CompleteTask(task);
		return true;
	} else {
		return false;
	}
}

template<typename T>
void TaskGraph<T>::RunTaskLoop(int numThreads){
	std::list<Thread> threads;

	for(int i = 0; i < numThreads; i++){
		threads.emplace_back([this](){
				// TODO this is very flawed, it will exit a thread as soon as a task can't be found, even if we're just blocked waiting for a bottleneck task to finish
				// Need to check if nodes are empty. If not, sleep for a bit
				while(RunOneTask())
				;
			});
	}

	for(Thread thread : threads){
		thread.Join();
	}
}

template<typename T>
TaskNode<T>* TaskGraph<T>::GrabNextTask(){
	LOCK(m_mtx);
	
	for(GN& gn : m_graph.m_nodes){
		TaskNode<T>* tn = &gn.m_data;
		if(!gn.HasParents() && !tn->m_running){
			tn->m_running = true;	

			return tn;
		}
	}
	return nullptr;
}

// TODO test this - maybe not needed if always running from our own ExecuteLoop
template<typename T>
void TaskGraph<T>::CompleteTask(TaskNode<T>* tn){
	LOCK(m_mtx);
	
	assert(tn->m_running);

	m_graph.DeleteNode((GN*) tn->m_gNode);
}



TEST(TaskGraph, Basic){
	using T = TaskGraph<Job>;
	using N = TaskNode<Job>;

	char ch = ' ';

	T tg([](Job& job){job.Execute();});
	N* a = tg.CreateTaskNode(Job([&ch](){ch = 'a';}));
	N* b = tg.CreateTaskNode(Job([&ch](){ch = 'b';}));
	N* c = tg.CreateTaskNode(Job([&ch](){ch = 'c';}));
	N* d = tg.CreateTaskNode(Job([&ch](){ch = 'd';}));

	tg.DependsOn(a, c);
	tg.DependsOn(b, a);
	tg.DependsOn(b, c);
	tg.DependsOn(c, d);
	
	auto doTaskAndExpect = [&](char c){
		tg.RunOneTask();
		ASSERT_EQ(ch, c);
	};

	doTaskAndExpect('d');
	doTaskAndExpect('c');
	doTaskAndExpect('a');
	doTaskAndExpect('b');
	ASSERT_FALSE(tg.RunOneTask());
}

TEST(TaskGraph, MultiThreaded){
	using T = TaskGraph<Job>;
	using N = TaskNode<Job>;

	constexpr int numTasks = 64;

	atomic<uint64_t> loc(0);
	uint64_t buf[numTasks];

	T tg([](Job& job){job.Execute();});

	N* prevTask;

	for(int i = 0; i < numTasks; i++){
		N* task = tg.CreateTaskNode(Job([&buf, &loc]{
					usleep(100000);
					uint64_t locVal = loc++;
					buf[locVal] = locVal;
				}));
		// TODO fix work loop and then test that this slows the test down
		if(i % 2 != 0){
			tg.DependsOn(prevTask, task);
		}
		prevTask = task;
	}

	tg.RunTaskLoop(32);

	for(uint64_t i = 0; i < numTasks; i++){
		ASSERT_EQ(buf[i], i);
	}
}

