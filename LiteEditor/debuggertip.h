//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : debuggertip.h              
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
 #ifndef DEBUGGERTIP_H
#define DEBUGGERTIP_H

#if 0
#include "wx/popupwin.h"
#include "tree_node.h"
#include "debuggerobserver.h"

class TipTree;

class DebuggerTip : public wxPopupWindow
{
	TipTree *m_localVarsTree;
protected:
	void OnTreeSized(wxCommandEvent &event);
	
public:
	DebuggerTip(wxWindow *parent, const wxString &expression, TreeNode<wxString, NodeData> *tree, long pos);
	~DebuggerTip();
	
	void Initialize(const wxString &expression, TreeNode<wxString, NodeData> *tree, long pos);
};

#endif //DEBUGGERTIP_H


#endif

