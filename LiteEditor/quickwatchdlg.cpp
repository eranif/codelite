//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : quickwatchdlg.cpp              
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
 #include "quickwatchdlg.h"
#include "localvarstree.h"
#include "manager.h"

QuickWatchDlg::QuickWatchDlg( wxWindow* parent, const wxString &expression,  TreeNode<wxString, NodeData> *tree )
:
QuickWatchBaseDlg( parent )
{
	Init(expression, tree);
}

void QuickWatchDlg::OnButtonClose( wxCommandEvent& event )
{
	wxUnusedVar(event);
	EndModal(wxID_CANCEL);
}

void QuickWatchDlg::Init(const wxString &expression, TreeNode<wxString, NodeData> *tree)
{
	m_localVarsTree->BuildTree(tree);
	m_textCtrl1->SetValue(expression);
} 

void QuickWatchDlg::OnReEvaluate(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString expression = m_textCtrl1->GetValue();
	if(expression.IsEmpty()){
		return;
	}

	// Call the manager to update the expression
	ManagerST::Get()->DbgQuickWatch(expression);
}
