//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : simpletable.cpp
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
#include "simpletable.h"
#include <wx/textdlg.h> //wxGetTextFromUser
#include "manager.h"
#include <wx/xrc/xmlres.h>
#include "debuggermanager.h"
#include "globals.h"

WatchesTable::WatchesTable( wxWindow* parent )
		:
		SimpleTableBase( parent )
		, m_selectedId(wxNOT_FOUND)
{
	InitTable();

	//Load the right click menu
	m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("dbg_watch_rmenu"));
	Connect(XRCID("del_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnDeleteWatch), NULL, this);
	Connect(XRCID("del_expr_all"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnDeleteAll), NULL, this);
	Connect(XRCID("expand_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuExpandExpr), NULL, this);
	Connect(XRCID("edit_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuEditExpr), NULL, this);
	Connect(XRCID("copy_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuCopyExpr), NULL, this);
	Connect(XRCID("copy_both"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuCopyBoth), NULL, this);
	Connect(XRCID("copy_value"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuCopyValue), NULL, this);
	Connect(XRCID("dereference_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuDerefExpr), NULL, this);
	Connect(XRCID("add_watch"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnNewWatch_Iternal), NULL, this);
}

WatchesTable::~WatchesTable()
{
	if (m_rclickMenu) {
		delete m_rclickMenu;
		m_rclickMenu = NULL;
	}
}

void WatchesTable::Clear()
{
	m_listTable->DeleteAllItems();
}

void WatchesTable::InitTable()
{
	//add two columns to the list ctrl
	m_listTable->InsertColumn(0, wxT("Expression"));
	m_listTable->InsertColumn(1, wxT("Value"));
}

void WatchesTable::OnItemActivated( wxListEvent& event )
{
	m_selectedId = event.m_itemIndex;
	DoShowMoreDetails(m_selectedId);
}

void WatchesTable::OnItemSelected( wxListEvent& event )
{
	// TODO: Implement OnItemSelected
	m_selectedId = event.m_itemIndex;
}

void WatchesTable::OnListKeyDown( wxListEvent& event )
{
	if (event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {
		//remove the entry
		if (m_selectedId != wxNOT_FOUND) {
			m_listTable->DeleteItem(m_selectedId);
			m_selectedId = wxNOT_FOUND;
		}
	}
}

void WatchesTable::OnItemRightClick(wxListEvent &event)
{
	wxUnusedVar(event);
	if (m_rclickMenu) {
		PopupMenu(m_rclickMenu);
	}
}

void WatchesTable::OnItemDeSelected(wxListEvent &event)
{
	m_selectedId = wxNOT_FOUND;
	wxUnusedVar(event);
}

void WatchesTable::OnNewWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString expr = wxGetTextFromUser(wxT("Expression to watch:"), wxT("New watch"));
	if (expr.IsEmpty() == false) {
		AddExpression(expr);
		RefreshValues();
	}
}

void WatchesTable::OnNewWatchUI(wxUpdateUIEvent &event)
{
	event.Enable(true);
}


void WatchesTable::OnDeleteAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_listTable->GetItemCount()>0) {
		m_listTable->DeleteAllItems();
		m_selectedId = wxNOT_FOUND;
	}
}

void WatchesTable::OnDeleteAllUI(wxUpdateUIEvent &event)
{
	event.Enable(m_listTable->GetItemCount()>0);
}

void WatchesTable::OnDeleteWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		m_listTable->DeleteItem(m_selectedId);
		m_selectedId = wxNOT_FOUND;
	}
}

void WatchesTable::OnDeleteWatchUI(wxUpdateUIEvent &event)
{
	event.Enable(m_selectedId != wxNOT_FOUND && m_listTable->GetItemCount()>0);
}

void WatchesTable::AddExpression(const wxString &expr)
{
	//make sure that the expression does not exist
	int count = m_listTable->GetItemCount();
	for (int i=0; i<count; i++) {
		wxString exp = this->GetColumnText(i, 0);
		if (exp == expr) {
			wxMessageBox(_("A similar expression already exist in the watches table"), wxT("CodeLite"), wxOK | wxICON_INFORMATION);
			return;
		}
	}

	long item = AppendListCtrlRow(m_listTable);
	this->SetColumnText(item, 0, expr);

	//info.SetState(wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
	//item = m_listTable->InsertItem(info);

	m_listTable->SetColumnWidth(0, 200);
	m_listTable->SetColumnWidth(1, 600);
}

void WatchesTable::RefreshValues()
{
	//ask the debugger to update the table
	//to trigger the update for the table we issue a simple
	//file line update request from the debugger
	if (ManagerST::Get()->DbgCanInteract()) {
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		if (dbgr && dbgr->IsRunning()) {
			dbgr->QueryFileLine();
		}
	}
}

void WatchesTable::UpdateExpression(const wxString &expr, const wxString &value)
{
	//find the item of this expression
	long item = FindExpressionItem(expr);
	if (item == wxNOT_FOUND) {
		return;
	}
	//update the text
	SetColumnText(item, 1, value);
}

void WatchesTable::SetColumnText ( long indx, long column, const wxString &rText )
{
	m_listTable->Freeze();
	wxListItem list_item;
	list_item.SetId ( indx );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	list_item.SetText ( rText );
	m_listTable->SetItem ( list_item );
	m_listTable->Thaw();
}

wxString WatchesTable::GetColumnText(long index, long column)
{
	wxListItem list_item;
	list_item.SetId ( index );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	m_listTable->GetItem ( list_item );
	return list_item.GetText();
}

long WatchesTable::FindExpressionItem(const wxString &expression)
{
	long item = -1;
	while ( true ) {
		item = m_listTable->GetNextItem(item,
		                                wxLIST_NEXT_ALL,
		                                wxLIST_STATE_DONTCARE);

		if ( item == -1 )
			break;

		wxString text(GetColumnText(item, 0));
		if (text == expression)
			return item;
	}
	return wxNOT_FOUND;
}

wxArrayString WatchesTable::GetExpressions()
{
	long item = wxNOT_FOUND;
	wxArrayString expressions;
	while ( true ) {
		item = m_listTable->GetNextItem(item,
		                                wxLIST_NEXT_ALL,
		                                wxLIST_STATE_DONTCARE);

		if ( item == -1 )
			break;

		wxString text(GetColumnText(item, 0));
		expressions.Add(text);
	}
	return expressions;
}

void WatchesTable::OnMenuExpandExpr(wxCommandEvent &event)
{
	wxUnusedVar(event);
	DoShowMoreDetails(m_selectedId);
}

void WatchesTable::OnListEditLabelEnd(wxListEvent &event)
{
	wxUnusedVar(event);
	RefreshValues();
}

void WatchesTable::OnMenuDerefExpr(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString curvalue = GetColumnText(m_selectedId, 0);
		curvalue.Prepend(wxT("*"));
		SetColumnText(m_selectedId, 0, curvalue);
		RefreshValues();
	}
}

void WatchesTable::OnMenuEditExpr(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString curvalue = GetColumnText(m_selectedId, 0);
		wxString newValue = wxGetTextFromUser(wxT("Edit expression:"), wxT("Edit"), curvalue);
		if (newValue.IsEmpty() == false) {
			SetColumnText(m_selectedId, 0, newValue);
			RefreshValues();
		}
	}
}

void WatchesTable::OnMenuCopyExpr(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString expr = GetColumnText(m_selectedId, 0);
		// copy expr + value to clipboard
		CopyToClipboard(expr);
	}
}

void WatchesTable::OnMenuCopyBoth(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString expr = GetColumnText(m_selectedId, 0);
		wxString value = GetColumnText(m_selectedId, 1);
		// copy expr + value to clipboard
		CopyToClipboard(expr + wxT(" ") + value);
	}
}

void WatchesTable::OnMenuCopyValue(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString value = GetColumnText(m_selectedId, 1);
		// copy expr + value to clipboard
		CopyToClipboard(value);
	}
}

void WatchesTable::OnDisplayFormat(wxCommandEvent& event)
{
	wxUnusedVar(event);

	// refresh the table
	RefreshValues();
}

wxString WatchesTable::GetDisplayFormat()
{
	return m_choiceDisplayFormat->GetStringSelection();
}

void WatchesTable::OnNewWatch_Iternal(wxCommandEvent& event)
{
	wxString expr = event.GetString();
	if( expr.empty() == false ){
		AddExpression(expr);
		RefreshValues();
	}
}

void WatchesTable::DoShowMoreDetails(long item)
{
	if( item != wxNOT_FOUND ) {
		wxString value = GetColumnText(item, 0);
		IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
		if ( dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract() ) {
			dbgr->CreateVariableObject( value, DBG_USERR_WATCHTABLE );
		}
	}
}
