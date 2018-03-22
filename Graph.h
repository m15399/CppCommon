
#pragma once

#include <CppCommon.h>

template<typename T>
class GraphNode {
public:
	T m_data;
	list<GraphNode*> m_parents;
	list<GraphNode*> m_children;

	GraphNode(const T& value);

	void AddChild(GraphNode* child);
	void RemoveChild(GraphNode* child);

	void RemoveAllChildren();
	void RemoveAllParents();

	bool HasParents();
	bool HasChildren();
};

template<typename T>
class Graph {
public:
	list<GraphNode<T>> m_nodes;

	GraphNode<T>* CreateNode(const T& data);
	void DeleteNode(GraphNode<T>* node);
};

