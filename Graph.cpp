
#include <CppCommon.h>

template<typename T>
class Graph {
public:

	class Node {
	public:
		T m_data;
		list<Node*> m_parents;
		list<Node*> m_children;

		Node(const T& value);
		
		void AddChild(Node* child);
		void RemoveChild(Node* child);

		void RemoveAllChildren();
		void RemoveAllParents();

		bool HasParents();
		bool HasChildren();
	};

	list<Node> m_nodes;

	void* CreateNode(const T& data);
	void DeleteNode(Node* node);
};

template<typename T>
Graph<T>::Node::Node(const T& data):
	m_data(data){

}

template<typename T>
void Graph<T>::Node::AddChild(Node* child){
	m_children.push_back(child);
	child->m_parents.push_back(this);
}

template<typename T>
void Graph<T>::Node::RemoveChild(Node* child){
	m_children.remove(child);
	child->m_parents.remove(this);
}

template<typename T>
void Graph<T>::Node::RemoveAllParents(){
	for(Node* parent : m_parents){
		parent->m_children.remove(this);
	}
	m_parents.clear();
}

template<typename T>
void Graph<T>::Node::RemoveAllChildren(){
	for(Node* child : m_children){
		child->m_parents.remove(this);
	}
	m_children.clear();
}

template<typename T>
bool Graph<T>::Node::HasParents(){
	return !m_parents.empty();
}

template<typename T>
bool Graph<T>::Node::HasChildren(){
	return !m_children.empty();
}

template<typename T>
void* Graph<T>::CreateNode(const T& data){
	m_nodes.emplace_back(data);
	return &m_nodes.back();
}

template<typename T>
void Graph<T>::DeleteNode(Graph<T>::Node* node){
	node->RemoveAllChildren();
	node->RemoveAllParents();

	m_nodes.remove_if([node](const Graph<T>::Node& value){ return &value == node; });
}



TEST(Graph, Basic){
	using G = Graph<int>;
	using N = G::Node;
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

