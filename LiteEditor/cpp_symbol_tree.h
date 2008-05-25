//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : cpp_symbol_tree.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #ifndef LITEEDITOR_CPP_SYMBOL_TREE_H
#define LITEEDITOR_CPP_SYMBOL_TREE_H

#include "symbol_tree.h"
#include "stack"

/// This class represents the GUI tree for the C++ symbols
class CppSymbolTree : public SymbolTree
{
	std::stack<wxTreeItemId> m_itemsStack;
	
public:

	CppSymbolTree();
	
	/// Nothing special here, just call our parent constructor
	CppSymbolTree(wxWindow *parent, const wxWindowID id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxTR_HIDE_ROOT | wxTR_HAS_BUTTONS);
		

	/// destructor
	virtual ~CppSymbolTree()
	{};

	//activate the selected item.
	//If there is no selection, retun false,
	//else try to open the file associated with the 
	//selected item
	//return true on successfull operation false otherwise
	bool ActivateSelectedItem();
	
	//advance the selection to next item (if possible)
	void AdvanceSelection(bool forward = true);

protected:
	virtual void OnMouseDblClick(wxMouseEvent& event);
	virtual void OnMouseRightUp(wxTreeEvent& event);
	virtual void OnItemActivated(wxTreeEvent &event);
	bool DoItemActivated(wxTreeItemId item, wxEvent &event);
	wxTreeItemId TryGetPrevItem(wxTreeItemId item);

	DECLARE_DYNAMIC_CLASS(CppSymbolTree)
};

#endif // LITEEDITOR_CPP_SYMBOL_TREE_H
