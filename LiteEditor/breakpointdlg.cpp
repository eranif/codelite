//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : breakpointdlg.cpp              
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
 #include "breakpointdlg.h" 
#include "debuggermanager.h"
#include "manager.h"
#include "macros.h"
#include "globals.h"

BreakpointDlg::BreakpointDlg( wxWindow* parent )
		: BreakpointTab( parent )
		, m_selectedItem(wxNOT_FOUND)
{
	Initialize();
	ConnectButton(m_buttonDelete, BreakpointDlg::OnDelete);
	ConnectButton(m_buttonDeleteAll, BreakpointDlg::OnDeleteAll);

}

void BreakpointDlg::Initialize()
{
	m_listCtrlBreakpoints->ClearAll();
	m_listCtrlBreakpoints->Freeze();

	m_listCtrlBreakpoints->InsertColumn(0, wxT("File"));
	m_listCtrlBreakpoints->InsertColumn(1, wxT("Line"));

	std::vector<BreakpointInfo> bps, pbps;
	DebuggerMgr::Get().GetBreakpoints(bps);
	std::vector<BreakpointInfo>::iterator iter = bps.begin();
	for (; iter != bps.end(); iter++) {
		long item = AppendListCtrlRow(m_listCtrlBreakpoints);
		SetColumnText(m_listCtrlBreakpoints, item, 0, iter->file, wxNOT_FOUND);

		wxString line;
		line << iter->lineno;
		SetColumnText(m_listCtrlBreakpoints, item, 1, line, wxNOT_FOUND);
	}

	iter = pbps.begin();
	for (; iter != pbps.end(); iter++) {
		long item = AppendListCtrlRow(m_listCtrlBreakpoints);
		SetColumnText(m_listCtrlBreakpoints, item, 0, iter->file);
		wxString line;
		line << iter->lineno;
		SetColumnText(m_listCtrlBreakpoints, item, 1, line);
	}

	m_listCtrlBreakpoints->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_listCtrlBreakpoints->SetColumnWidth(1, wxLIST_AUTOSIZE);

	//TODO:: select the first item
	m_listCtrlBreakpoints->Thaw();
}

void BreakpointDlg::OnDelete(wxCommandEvent &e)
{
	if (m_selectedItem != wxNOT_FOUND) {
		
		wxString fileName = GetColumnText(m_listCtrlBreakpoints, m_selectedItem, 0);
		wxString strLine  = GetColumnText(m_listCtrlBreakpoints, m_selectedItem, 1);
		
		long lineno;
		strLine.ToLong(&lineno);

		BreakpointInfo bp;
		bp.file = fileName;
		bp.lineno = lineno;
		
		ManagerST::Get()->DbgDeleteBreakpoint(bp);
		m_listCtrlBreakpoints->DeleteItem(m_selectedItem);
		m_selectedItem = wxNOT_FOUND;
	}
}

void BreakpointDlg::OnDeleteAll(wxCommandEvent &e)
{
	wxUnusedVar(e);
	ManagerST::Get()->DbgDeleteAllBreakpoints();
	Initialize();
}

void BreakpointDlg::OnItemSelected(wxListEvent &e)
{
	m_selectedItem = e.m_itemIndex;
}

void BreakpointDlg::OnItemDeselected(wxListEvent &e)
{
	wxUnusedVar(e);
	m_selectedItem = wxNOT_FOUND;
}

void BreakpointDlg::OnItemActivated(wxListEvent &e)
{
	wxString file = GetColumnText(m_listCtrlBreakpoints, e.m_itemIndex, 0);
	wxString line = GetColumnText(m_listCtrlBreakpoints, e.m_itemIndex, 1);
	long line_number;
	line.ToLong(&line_number);
	
	ManagerST::Get()->OpenFile(file, wxEmptyString, line_number-1);
}
