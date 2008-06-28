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

SimpleTable::SimpleTable( wxWindow* parent )
		:
		SimpleTableBase( parent )
		, m_selectedId(wxNOT_FOUND)
{
	InitTable();

	//Load the right click menu
	m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("dbg_watch_rmenu"));
	Connect(XRCID("del_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SimpleTable::OnDeleteWatch), NULL, this);
	Connect(XRCID("del_expr_all"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SimpleTable::OnDeleteAll), NULL, this);
	Connect(XRCID("expand_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SimpleTable::OnMenuExpandExpr), NULL, this);
	Connect(XRCID("edit_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SimpleTable::OnMenuEditExpr), NULL, this);
	Connect(XRCID("copy_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SimpleTable::OnMenuCopyExpr), NULL, this);
	Connect(XRCID("copy_both"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SimpleTable::OnMenuCopyBoth), NULL, this);
	Connect(XRCID("copy_value"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SimpleTable::OnMenuCopyValue), NULL, this);
	Connect(XRCID("dereference_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SimpleTable::OnMenuDerefExpr), NULL, this);
}

SimpleTable::~SimpleTable()
{
	if (m_rclickMenu) {
		delete m_rclickMenu;
		m_rclickMenu = NULL;
	}
}

void SimpleTable::Clear()
{
	m_listTable->DeleteAllItems();
}

void SimpleTable::InitTable()
{
	//add two columns to the list ctrl
	m_listTable->InsertColumn(0, wxT("Expression"));
	m_listTable->InsertColumn(1, wxT("Value"));
}

void SimpleTable::OnItemActivated( wxListEvent& event )
{
	m_selectedId = event.m_itemIndex;
	DoShowMoreDetails(m_selectedId);
}

void SimpleTable::OnItemSelected( wxListEvent& event )
{
	// TODO: Implement OnItemSelected
	m_selectedId = event.m_itemIndex;
}

void SimpleTable::OnListKeyDown( wxListEvent& event )
{
	if (event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {
		//remove the entry
		if (m_selectedId != wxNOT_FOUND) {
			m_listTable->DeleteItem(m_selectedId);
			m_selectedId = wxNOT_FOUND;
		}
	}
}

void SimpleTable::OnItemRightClick(wxListEvent &event)
{
	wxUnusedVar(event);
	if (m_rclickMenu) {
		PopupMenu(m_rclickMenu);
	}
}

void SimpleTable::OnItemDeSelected(wxListEvent &event)
{
	m_selectedId = wxNOT_FOUND;
	wxUnusedVar(event);
}

void SimpleTable::OnNewWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString expr = wxGetTextFromUser(wxT("Expression to watch:"), wxT("New watch"));
	if (expr.IsEmpty() == false) {
		AddExpression(expr);
		RefreshValues();
	}
}

void SimpleTable::OnNewWatchUI(wxUpdateUIEvent &event)
{
	event.Enable(true);
}


void SimpleTable::OnDeleteAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_listTable->GetItemCount()>0) {
		m_listTable->DeleteAllItems();
		m_selectedId = wxNOT_FOUND;
	}
}

void SimpleTable::OnDeleteAllUI(wxUpdateUIEvent &event)
{
	event.Enable(m_listTable->GetItemCount()>0);
}

void SimpleTable::OnDeleteWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		m_listTable->DeleteItem(m_selectedId);
		m_selectedId = wxNOT_FOUND;
	}
}

void SimpleTable::OnDeleteWatchUI(wxUpdateUIEvent &event)
{
	event.Enable(m_selectedId != wxNOT_FOUND && m_listTable->GetItemCount()>0);
}

void SimpleTable::AddExpression(const wxString &expr)
{
	//make sure that the expression does not exist
	int count = m_listTable->GetItemCount();
	for (int i=0; i<count; i++) {
		wxString exp = this->GetColumnText(i, 0);
		if (exp == expr) {
			wxMessageBox(wxT("A similar expression already exist in the watches table"), wxT("CodeLite"), wxOK | wxICON_INFORMATION);
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

void SimpleTable::RefreshValues()
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

void SimpleTable::UpdateExpression(const wxString &expr, const wxString &value)
{
	//find the item of this expression
	long item = FindExpressionItem(expr);
	if (item == wxNOT_FOUND) {
		return;
	}
	//update the text
	SetColumnText(item, 1, value);
}

void SimpleTable::SetColumnText ( long indx, long column, const wxString &rText )
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

wxString SimpleTable::GetColumnText(long index, long column)
{
	wxListItem list_item;
	list_item.SetId ( index );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	m_listTable->GetItem ( list_item );
	return list_item.GetText();
}

long SimpleTable::FindExpressionItem(const wxString &expression)
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

wxArrayString SimpleTable::GetExpressions()
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

void SimpleTable::DoShowMoreDetails(long item)
{
	ManagerST::Get()->DbgQuickWatch(GetColumnText(item, 0));
}

void SimpleTable::OnMenuExpandExpr(wxCommandEvent &event)
{
	wxUnusedVar(event);
	DoShowMoreDetails(m_selectedId);
}

void SimpleTable::OnListEditLabelEnd(wxListEvent &event)
{
	wxUnusedVar(event);
	RefreshValues();
}

void SimpleTable::OnMenuDerefExpr(wxCommandEvent &event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString curvalue = GetColumnText(m_selectedId, 0);
		curvalue.Prepend(wxT("*"));
		SetColumnText(m_selectedId, 0, curvalue);
		RefreshValues();
	}
}

void SimpleTable::OnMenuEditExpr(wxCommandEvent &event)
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

void SimpleTable::OnMenuCopyExpr(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString expr = GetColumnText(m_selectedId, 0);
		// copy expr + value to clipboard
		CopyToClipboard(expr);
	}	
}

void SimpleTable::OnMenuCopyBoth(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString expr = GetColumnText(m_selectedId, 0);
		wxString value = GetColumnText(m_selectedId, 1);
		// copy expr + value to clipboard
		CopyToClipboard(expr + wxT(" ") + value);
	}
}

void SimpleTable::OnMenuCopyValue(wxCommandEvent& event)
{
	wxUnusedVar(event);
	if (m_selectedId != wxNOT_FOUND) {
		wxString value = GetColumnText(m_selectedId, 1);
		// copy expr + value to clipboard
		CopyToClipboard(value);
	}	
}

void SimpleTable::OnDisplayFormat(wxCommandEvent& event)
{
	wxUnusedVar(event);
	
	// refresh the table
	RefreshValues();
}

wxString SimpleTable::GetDisplayFormat()
{
	return m_choiceDisplayFormat->GetStringSelection();
}

