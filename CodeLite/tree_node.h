//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tree_node.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CODELITE_TREE_NODE_H
#define CODELITE_TREE_NODE_H


#include <map>
#include <iostream>
#include <vector>

template <class TKey, class TData>
class TreeNode
{
	TKey  m_key;
	TData m_data;
	TreeNode* m_parent;
	std::map<TreeNode*, TreeNode*> m_childs;
	typename std::map<TreeNode*, TreeNode*>::iterator m_pos;

public:
	/**
	* Constructor, constructs a tree node with key and data.
	* If parent is left NULL, node is assumed to be root.
	* \param key Node key, must have operator =, <
	* \param data Node data, this class must have operator=
	* \param parent Parent node
	*/
	TreeNode(const TKey& key, const TData& data, TreeNode* parent = NULL);

	/**
	* Destructor, deletes this node and all its children.
	*/
	virtual ~TreeNode();

	//---------------------------------------------------
	// Setters / Getters
	//---------------------------------------------------

	/**
	* Get node data.
	* \return node data
	*/
	TData& GetData()
	{
		return m_data;
	};

	/**
	* Get node key.
	* \return key
	*/
	TKey& GetKey()
	{
		return m_key;
	};

	/**
	* Set node data.
	* \param data data
	*/
	void SetData(const TData& data)
	{
		m_data = data;
	};

	/**
	* Set node key.
	* \param key Key
	*/
	void SetKey(const TKey& key)
	{
		m_key = key;
	};

	/**
	 * Return the parent node of this node (or null if root).
	 * \return pointer to parent node
	 */
	TreeNode* GetParent()
	{
		return m_parent;
	}

	//---------------------------------------------
	// Misc
	//---------------------------------------------

	/**
	* Print the tree to stream (default is stdout).
	*/
	void Print(std::ostream& stream = std::cout , int depth = 0);

	/**
	* Check if this node is root.
	* \return true if node is root
	*/
	bool IsRoot() const
	{
		return m_parent == NULL;
	};

	//----------------------------------------------
	// Operations
	//----------------------------------------------
	/**
	* Add new child node to this node.
	* Duplicate nodes are allowed. However, Remove() will remove the first occurrence of a node by a given key.
	* \param key Node key
	* \param data Node data
	* \return newly added node
	*/
	TreeNode* AddChild(const TKey& key, const TData& data) ;

	/**
	 * \brief Append new child to this tree node
	 * \param newNode node to append, must be allocated on the heap
	 * \return the newly added tree node
	 */
	TreeNode* AddChild(TreeNode* newNode);

	/**
	* Remove first occurrence of node with a given key.
	* If the node to be removed is the root, a std::exception* is thrown, which must be deleted by caller.
	* Remove does not delete the memory allocated for the node. The user must delete the removed node.
	* \param key Node key
	* \return removed node
	*/
	TreeNode* Remove(const TKey& key);

	/**
	* Find a node by a key.
	* \param key Node key
	* \return node, or NULL if non exist
	*/
	TreeNode* Find(const TKey& key) ;

	/**
	 * Set the pointer to the first child of this node .
	 */
	void First();

	/**
	 * Return the next child of this node.
	 */
	TreeNode* Next();

	/**
	 * Test if this node has more childs.
	 * This call is usually used before issuing a Next() call.
	 * \return true if the next call to Next() will return a valid child pointer
	 */
	bool HasMore();
	std::map<TreeNode*, TreeNode*>& GetChilds() { return m_childs; }
};

template <class TKey, class TData>
TreeNode<TKey, TData>::TreeNode(const TKey& key, const TData& data, TreeNode* parent)
: m_key(key), m_data(data), m_parent(parent)
{
}

template <class TKey, class TData>
TreeNode<TKey, TData>::~TreeNode()
{
	typename std::map<TreeNode*, TreeNode*>::iterator iter = m_childs.begin();
	for(; iter != m_childs.end(); iter++)
	{
		delete iter->second;
	}
	m_childs.clear();
}

template <class TKey, class TData>
TreeNode<TKey, TData>* TreeNode<TKey, TData>::AddChild(const TKey& key, const TData& data)
{
	TreeNode* newNode = new TreeNode(key, data, this);
	m_childs[newNode] = newNode;
	return newNode;
}

template <class TKey, class TData>
TreeNode<TKey, TData>* TreeNode<TKey, TData>::AddChild(TreeNode* newNode)
{
	m_childs[newNode] = newNode;
	return newNode;
}

template <class TKey, class TData>
TreeNode<TKey, TData>* TreeNode<TKey, TData>::Remove(const TKey& key)
{
	TreeNode* node = Find(key);
	if( node )
	{
		if(NULL == node->m_parent)
		{
			// Cant remove root
			return NULL;
		}

		typename std::map<TreeNode*, TreeNode*>::iterator iter = node->m_parent->m_childs.find(node);
		if(iter != node->m_parent->m_childs.end())
			node->m_parent->m_childs.erase(iter);
		return node;
	}
	return NULL;
}

template <class TKey, class TData>
TreeNode<TKey, TData>* TreeNode<TKey, TData>::Find(const TKey& key)
{
	if(m_key == key)
		return this;

	typename std::map<TreeNode*, TreeNode*>::iterator iter;

	// Scan first the childs of this node
	for(iter = m_childs.begin(); iter != m_childs.end(); iter++)
	{
		if(((TreeNode*)iter->second)->GetKey() == key)
			return iter->second;
	}

	// Scan level below
	for(iter = m_childs.begin(); iter != m_childs.end(); iter++)
	{
		TreeNode* node = NULL;
		node = iter->second->Find(key);
		if(node) return node;
	}
	return NULL;
}

template <class TKey, class TData>
void TreeNode<TKey, TData>::Print(std::ostream& stream, int depth)
{
	std::string tab = " ";
	for(int i=0; i<depth; i++)
		tab += "    ";

	stream << tab.c_str() << m_key << std::endl;
	depth++;

	typename std::map<TreeNode*, TreeNode*>::iterator iter = m_childs.begin();
	for(; iter != m_childs.end(); iter++)
		iter->second->Print(stream, depth);
}

/// Prepare for tree iteration in the current node
template <class TKey, class TData>
void TreeNode<TKey, TData>::First()
{
	m_pos = m_childs.begin();
}

template <class TKey, class TData>
TreeNode<TKey, TData>* TreeNode<TKey, TData>::Next()
{
	if(!HasMore())
		return NULL;
	TreeNode* nextElem = m_pos->second;
	m_pos++;
	return nextElem;
}

template <class TKey, class TData>
bool TreeNode<TKey, TData>::HasMore()
{
	return (m_pos != m_childs.end());
}

//----------------------------------------------------------------
// TreeWalker class
//----------------------------------------------------------------

template <class TKey, class TData>
class TreeWalker
{
private:
	void GetChildren(TreeNode<TKey, TData>* node);
	std::vector<TreeNode<TKey, TData>* > m_children;
	size_t m_pos;

public:
	TreeWalker(TreeNode<TKey, TData>* node);
	virtual ~TreeWalker();
	bool End();
	void operator++(int);
	TreeNode<TKey, TData>* GetNode()
	{
		if(m_pos < m_children.size())
			return m_children[m_pos];
		return NULL;
	}
};

template <class TKey, class TData>
TreeWalker<TKey, TData>::TreeWalker(TreeNode<TKey, TData>* node)
: m_pos(0)
{
	m_children.push_back(node);
	GetChildren(node);
}

template <class TKey, class TData>
TreeWalker<TKey, TData>::~TreeWalker()
{
}

template <class TKey, class TData>
void TreeWalker<TKey, TData>::GetChildren(TreeNode<TKey, TData>* node)
{
	if(node == NULL)
		return;
	typename std::map<TreeNode<TKey, TData>*, TreeNode<TKey, TData>*>::iterator iter = node->GetChilds().begin();
	for(; iter != node->GetChilds().end(); iter++)
	{
		m_children.push_back(iter->second);
		GetChildren(iter->second);
	}
}

template <class TKey, class TData>
bool TreeWalker<TKey, TData>::End()
{
	return m_pos == m_children.size();
}

template <class TKey, class TData>
void TreeWalker<TKey, TData>::operator++(int)
{
	m_pos++;
}

#endif // CODELITE_TREE_NODE_H
