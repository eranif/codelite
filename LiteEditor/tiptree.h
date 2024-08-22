//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : tiptree.h              
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
 #ifndef TIPTREE_H
#define TIPTREE_H

#include "debuggerobserver.h"
#include "tree_node.h"

#include <wx/treectrl.h>

class TipTreeData : public wxTreeItemData
{
	wxString m_displayString;
public:
	TipTreeData(const wxString &displayStr) : m_displayString(displayStr){}
	virtual ~TipTreeData(){}
};

class TipTree : public wxTreeCtrl
{
protected:
	void SortTree(std::map<void*, bool> & nodes);

public:
	TipTree(wxWindow *parent, wxWindowID id, long style = wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);
	virtual ~TipTree();

	void BuildTree(TreeNode<wxString, NodeData> *tree);
};

#endif //TIPTREE_H
