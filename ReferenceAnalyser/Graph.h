#ifndef __graph__
#define __graph__

#include <set>
#include <vector>
#include <map>
#include <stack>

template<class T>
class Graph {
public:
	typedef std::set<T> Nodes;
	typedef std::vector<T> NodeVector;
	typedef std::map<T, Nodes> Edges;
	
	typedef const NodeVector& ConstNodeVectorRef;
	
	typedef typename Nodes::const_iterator NodesIterator;
	typedef typename NodeVector::const_iterator NodeVectorIterator;
	typedef typename Edges::const_iterator EdgesIterator;
	
	Graph();
	~Graph();
	
	void addNode(T node);
	void addEdge(T from, T to);
	
	void clear();
	
	bool isEmpty();
	bool isAcyclic();
	bool isAcyclicFrom(T node);
	bool canReach(T from, T target, Nodes& checked, std::stack<T>& path);

	ConstNodeVectorRef getPath();
	
	EdgesIterator begin();
	EdgesIterator end();
	EdgesIterator find(T value); 
	size_t size();
	
private:
	Edges m_edges;
	NodeVector m_path;
};

template <class T> 
Graph<T>::Graph()
{
}

template <class T> 
Graph<T>::~Graph()
{
}

template <class T>
void Graph<T>::addNode(T node)
{
	// Trigger the insert function of map if it's not already there
	m_edges[node];
}

template <class T> 
void Graph<T>::addEdge(T from, T to)
{
	if(m_edges.find(from) == m_edges.end())
	{
		// Log
		return;
	}
	
	if(m_edges.find(to) == m_edges.end())
	{
		// Log
		return;
	}
	
	m_edges[from].insert(to);
}

template <class T> 
void Graph<T>::clear()
{
	m_edges.clear();
}

template <class T> 
typename Graph<T>::Edges::const_iterator Graph<T>::begin()
{
	return m_edges.begin(); 
}

template <class T> 
typename Graph<T>::Edges::const_iterator Graph<T>::end()
{
	return m_edges.end(); 
}

template <class T> 
typename Graph<T>::Edges::const_iterator Graph<T>::find(T value)
{
	return m_edges.find(value); 
}

template <class T> 
size_t Graph<T>::size() 
{ 
	return m_edges.size(); 
}

template <class T> 
bool Graph<T>::isEmpty()
{
	for(EdgesIterator it = m_edges.begin(); it != m_edges.end(); it++)
	{
		if(it->second.size())
			return false;
	}
	
	return true;
}

template <class T> 
bool Graph<T>::isAcyclic()
{
	for(EdgesIterator it = m_edges.begin(); it != m_edges.end(); it++)
	{
		if(!isAcyclicFrom(it->first))
			return false;
	}
	
	return true;
}

template <class T> 
bool Graph<T>::isAcyclicFrom(T node)
{
	Nodes checked;
	std::stack<T> path;
	
	if(canReach(node, node, checked, path))
	{
		NodeVector result;
		while(!path.empty())
		{
			result.push_back(path.top());
			path.pop();
		}
		
		m_path = result;
		return false;
	}
		
	return true;
}

template <class T> 
bool Graph<T>::canReach(T from, T target, Nodes& checked, std::stack<T>& path)
{
	for(NodesIterator it = m_edges[from].begin(); it != m_edges[from].end(); it++)
	{
		T current = *it;
		if(current == target)		
			return true;
			
		// Don't check this node if we have already
		if(checked.count(current) == 1)
			continue;
			
		checked.insert(current);
		path.push(current);
			
		if(canReach(current, target, checked, path))
			return true;
			
		path.pop();
	}
	
	return false;
}

template <class T> 
typename Graph<T>::ConstNodeVectorRef Graph<T>::getPath()
{
	return m_path;
}

#endif // __graph__
