//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : localvarstree.cpp              
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
 #include "localvarstree.h"
#include "map"

LocalVarsTree::LocalVarsTree(wxWindow *parent, wxWindowID id, long style)
: wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, style)
, m_tree(NULL)
{
}

LocalVarsTree::~LocalVarsTree()
{
	Clear();
}

void LocalVarsTree::Clear()
{
	if(m_tree){
		delete m_tree;
		m_tree = NULL;
	}
	DeleteAllItems();
}

void LocalVarsTree::BuildTree(TreeNode<wxString, NodeData> *tree)
{
	if(!tree){
		return;
	}
	int counter(0);
	bool expandRoot(false);
	
	std::map<wxString, bool> expandItems;
	if(m_tree && m_tree->GetData().name == tree->GetData().name){
		//same stack tree, compare the trees to get its status
		//(i.e. expand/fold state)
		TreeWalker<wxString, NodeData> tmpWalker(m_tree);
		for(; !tmpWalker.End(); tmpWalker++){
			TreeNode<wxString, NodeData>* node = tmpWalker.GetNode();
			wxString name = node->GetData().name;
			bool is_ok = node->GetData().itemId.IsOk();
			bool is_root = GetRootItem() == node->GetData().itemId;
			bool is_expanded( false );
			if(!is_root && is_ok){
				is_expanded = IsExpanded(node->GetData().itemId);
			}
			
			if(is_ok && !is_root && is_expanded){
				expandItems[name] = true;
			}
			counter++;
		}
	}else{
		expandRoot = true;
	}

	//Walk over the tree and construct it
	Freeze();
	if(m_tree != tree){
		Clear();
	}else{
		DeleteAllItems();
	}
	m_tree = tree;
	
	//create the tree
	wxTreeItemId root = AddRoot(m_tree->GetData().name);
	tree->GetData().itemId = root;
	TreeWalker<wxString, NodeData> walker(tree);

	for(; !walker.End(); walker++)
	{
		// Add the item to the tree
		TreeNode<wxString, NodeData>* node = walker.GetNode();

		// Skip root node
		if(node->IsRoot())
			continue;
		
		wxTreeItemId parentHti = node->GetParent()->GetData().itemId;
		if(parentHti.IsOk() == false){
			parentHti = root;
		}

		//add the item to the tree
		node->GetData().itemId = AppendItem(
												parentHti,				// parent
												node->GetData().name,	// display name
												wxNOT_FOUND,			// item image index
												wxNOT_FOUND,			// selected item image
												new LocalVarData(node->GetData().name)
											);
		m_sortItems[parentHti.m_pItem] = true;
	}
	SortTree(m_sortItems);

	//reset tree state
	counter = 0;
	TreeWalker<wxString, NodeData> secWalker(tree);
	for(; !secWalker.End(); secWalker++)
	{
		TreeNode<wxString, NodeData>* node = secWalker.GetNode();
		if(expandItems.find(node->GetData().name) != expandItems.end()){
			//expand this item
			if(node->GetData().itemId.IsOk()){
				Expand(node->GetData().itemId);
			}
		}
		counter++;
	}
	
	m_sortItems.clear();

	if(!HasFlag(wxTR_HIDE_ROOT) && ItemHasChildren(root)){ //exclude the root
		Expand(root);
	}
	
	Thaw();
}

void LocalVarsTree::SortTree(std::map<void*, bool> & nodes)
{
	std::map<void*, bool>::iterator iter = nodes.begin();
	for(; iter != nodes.end(); iter++){
		wxTreeItemId item = iter->first;
		if(item.IsOk()){
			// Does this node has children?
			if( GetChildrenCount( item ) == 0 )
				continue;
			SortChildren(item); 
		}
	}
}


