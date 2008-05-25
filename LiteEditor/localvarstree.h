//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : localvarstree.h              
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
 #ifndef LOCALVARSTREE_H
#define LOCALVARSTREE_H

#include "wx/treectrl.h"
#include "debuggerobserver.h"
#include "tree_node.h"

class LocalVarData : public wxTreeItemData
{
	wxString m_displayString;
public:
	LocalVarData(const wxString &displayStr):m_displayString(displayStr){}
	virtual ~LocalVarData(){}
};

class LocalVarsTree : public wxTreeCtrl
{
	TreeNode<wxString, NodeData> *m_tree;
	std::map<void*, bool> m_sortItems;

protected:
	void SortTree(std::map<void*, bool> & nodes);

public:
	LocalVarsTree(wxWindow *parent, wxWindowID id, long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
	virtual ~LocalVarsTree();

	void BuildTree(TreeNode<wxString, NodeData> *tree);
	TreeNode<wxString, NodeData> *GetTree() {return m_tree;}
	void Clear();
};

#endif //LOCALVARSTREE_H


