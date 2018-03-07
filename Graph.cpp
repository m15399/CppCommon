
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
void* Graph<T>::CreateNode(const T& data){
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
	N* a = (N*)g.CreateNode(0);
	N* b = (N*)g.CreateNode(1);
	N* c = (N*)g.CreateNode(2);

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

	// TODO try to return task note<T>
	// TODO same with GraphNode
	void* CreateTaskNode(const T& data);
	void DependsOn(TaskNode<T>* a, TaskNode<T>* b);

	void* GrabNextTask();
	void CompleteTask(TaskNode<T>* task);

protected:
	Graph<TaskNode<T>> m_graph;
	mutex m_mtx;
};


template<typename T>
TaskNode<T>::TaskNode(const T& data):
	m_data(data){
	
}

template<typename T>
void* TaskGraph<T>::CreateTaskNode(const T& data){
	LOCK(m_mtx);

	GN* gNode = (GN*) m_graph.CreateNode(TaskNode<T>(data));
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
void* TaskGraph<T>::GrabNextTask(){
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

	m_graph.RemoveNode((GN*)tn->m_gNode);
}

TEST(TaskGraph, Basic){
	using T = TaskGraph<Job>;
	using N = TaskNode<Job>;

	char ch = ' ';

	T tg;	
	N* a = (N*)tg.CreateTaskNode(Job([&ch](){ch = 'a';}));
	N* b = (N*)tg.CreateTaskNode(Job([&ch](){ch = 'b';}));
	N* c = (N*)tg.CreateTaskNode(Job([&ch](){ch = 'c';}));
	N* d = (N*)tg.CreateTaskNode(Job([&ch](){ch = 'd';}));

	tg.DependsOn(a, c);
	tg.DependsOn(b, a);
	tg.DependsOn(b, c);
	tg.DependsOn(c, d);
	
	N* task;

	task = (N*)tg.GrabNextTask();
	ASSERT_EQ(task, d);
}

