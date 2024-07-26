//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tree.h
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

#ifndef CODELITE_TREE_H
#define CODELITE_TREE_H

#include "tree_node.h"

template <typename TKey, typename TData>
class Tree
{
	std::map<TKey, TreeNode<TKey, TData>*> m_nodes;
	TreeNode<TKey, TData>* m_root;
public:
	/**
	 * Construct a tree with root node with Key and data.
	 * \param key Root node key
	 * \param data Root node data
	 */
	Tree(const TKey& key, const TData& data);

	/**
	 * Destructor, deletes all tree and its sub nodes.
	 */
	virtual ~Tree();

	/**
	 * Find node in the tree with Key.
	 * \param key Search key
	 * \return node
	 */
	TreeNode<TKey, TData>* Find(const TKey& key) ;

	/**
	 * Remove node (and all its sub nodes) from the tree using key as the criteria.
	 * This function does not delete the removed node.
	 * \param key search key
	 * \return remove node
	 */
	TreeNode<TKey, TData>* Remove(const TKey& key);

	/**
	 * Add child node to the tree.
	 * \param key New node key
	 * \param data New node data
	 * \param parent New node parent, if left NULL parent is set to root
	 * \return newly added node
	 */
	TreeNode<TKey, TData>* AddChild(const TKey& key, const TData& data, TreeNode<TKey, TData>* parent = NULL) ;

	/**
	 * Returns tree root.
	 * \return root node
	 */
	TreeNode<TKey, TData>* GetRoot() { return m_root;}

	/**
	 * Prints the tree in tabulated format to stream.
	 * \param stream Output stream (default is set to stdout)
	 * \param depth Tab depth (for internal use)
	 */
	void Print(std::ostream& stream = std::cout , int depth = 0);

	/**
	 * Compare this tree against another tree.
	 * \param targetTree Target tree to compare with
	 * \param deletedItems Array of pairs of items which exist in this tree and not in target tree
	 * \param modifiedItems Array of pairs of items which have the same key but differnt data
	 * \param newItems Aarray of pairs of items which exist in the target tree but not in this tree
	 * \param fromNode If its set to null, comparison will start from this tree root node, if not null,
	 * comparison will compare sub-tree which root of its fromNode
	 */
	void Compare(Tree* targetTree, std::vector<std::pair<TKey, TData> >& deletedItems, std::vector<std::pair<TKey, TData> >& modifiedItems, std::vector<std::pair<TKey, TData> >& newItems, TreeNode<TKey, TData>* fromNode = NULL);

};

template <typename TKey, typename TData>
Tree<TKey, TData>::Tree(const TKey& key, const TData& data)
{
	m_root = new TreeNode<TKey, TData>(key, data);
}

template <typename TKey, typename TData>
Tree<TKey, TData>::~Tree()
{
	delete m_root;
}

template <typename TKey, typename TData>
TreeNode<TKey, TData>* Tree<TKey, TData>::Find(const TKey& key)
{
	typename std::map<TKey, TreeNode<TKey, TData>*>::const_iterator iter = m_nodes.find(key);
	if(iter == m_nodes.end())
		return NULL;
	return iter->second;
}

template <typename TKey, typename TData>
TreeNode<TKey, TData>* Tree<TKey, TData>::AddChild(const TKey& key, const TData& data, TreeNode<TKey, TData>* parent /*NULL*/)
{
	TreeNode<TKey, TData>* parentNode;
	(parent == NULL) ? parentNode = m_root : parentNode = parent;
	TreeNode<TKey, TData>* node = parentNode->AddChild(key, data);
	m_nodes[key] = node;
	return node;
}

template <typename TKey, typename TData>
TreeNode<TKey, TData>* Tree<TKey, TData>::Remove(const TKey& key)
{
	typename std::map<TKey, TreeNode<TKey, TData>*>::const_iterator iter = m_nodes.find(key);
	if(iter == m_nodes.end())
		return NULL;

	// Remove from the map all sub-nodes of the tree
	TreeWalker<TKey, TData> walker(iter->second);

	for(; !walker.End(); walker++)
	{
		typename std::map<TKey, TreeNode<TKey, TData>*>::iterator it = m_nodes.find(walker.GetNode()->GetKey());
		if(it != m_nodes.end())
			m_nodes.erase(it);
	}
	return m_root->Remove(key);
}
template <typename TKey, typename TData>
void Tree<TKey, TData>::Print(std::ostream& stream , int depth)
{
	m_root->Print(stream, depth);
}

template <typename TKey, typename TData>
void Tree<TKey, TData>::Compare(Tree* targetTree, std::vector<std::pair<TKey, TData> >& deletedItems, std::vector<std::pair<TKey, TData> >& modifiedItems, std::vector<std::pair<TKey, TData> >& newItems, TreeNode<TKey, TData>* fromNode)
{
	// we break generic for the sake of thread safety:
	// we explicitly calling TKey.c_str(), which means that we assume that TKey has such member
	if(!targetTree){
		return;
	}

	deletedItems.clear(); newItems.clear(); modifiedItems.clear();

	TreeNode<TKey, TData>* node;

	fromNode == NULL ? node = GetRoot() : node = fromNode;
	TreeWalker<TKey, TData> sourceTreeWalker(node);
	TreeWalker<TKey, TData> targetTreeWalker(targetTree->GetRoot());

	for(; !sourceTreeWalker.End(); sourceTreeWalker++)
	{
		if( sourceTreeWalker.GetNode()->IsRoot() )
			continue;

		TreeNode<TKey, TData>* node = targetTree->Find(sourceTreeWalker.GetNode()->GetKey());
		if(node == NULL)
		{
			// Item does not exist in target tree which means it must been deleted
			std::pair<TKey, TData> itemPair;

			itemPair.first = sourceTreeWalker.GetNode()->GetKey().c_str();
			itemPair.second = sourceTreeWalker.GetNode()->GetData();
			deletedItems.push_back( itemPair );
		}
		else
		{
			// Compare data
			if(node->GetData() == sourceTreeWalker.GetNode()->GetData())
				continue;

			// Data was modified
			std::pair<TKey, TData> itemPair;
			itemPair.first = sourceTreeWalker.GetNode()->GetKey().c_str();
			itemPair.second = node->GetData();
			modifiedItems.push_back( itemPair );
		}
	}

	for(; !targetTreeWalker.End(); targetTreeWalker++)
	{
		if(targetTreeWalker.GetNode()->IsRoot())
			continue;

		if(Find(targetTreeWalker.GetNode()->GetKey()) == NULL)
		{
			// Node from target tree does not exist in this tree
			// which means that this node is new
			// Data was modified
			std::pair<TKey, TData> itemPair;
			itemPair.first = targetTreeWalker.GetNode()->GetKey().c_str();
			itemPair.second = targetTreeWalker.GetNode()->GetData();
			newItems.push_back( itemPair );
		}
	}
}

#endif // CODELITE_TREE_H
