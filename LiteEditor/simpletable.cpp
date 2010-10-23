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
#include "debugger.h"

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

class WatchData : public wxTreeItemData
{
	VariableObjChild* m_voc;
	wxString          m_expression;
	bool              m_isFirst;
	wxString          m_gdbId;

public:

	void SetExpression(const wxString& expression) {
		this->m_expression = expression;
	}

	void SetVoc(const VariableObjChild& voc) {
		this->m_voc = new VariableObjChild(voc);
	}

	const wxString& GetExpression() const {
		return m_expression;
	}

	VariableObjChild* GetVoc() {
		return m_voc;
	}

	void SetIsFirst(bool isFirst) {
		this->m_isFirst = isFirst;
	}
	bool GetIsFirst() const {
		return m_isFirst;
	}
	void SetGdbId(const wxString& gdbId) {
		this->m_gdbId = gdbId;
	}
	const wxString& GetGdbId() const {
		return m_gdbId;
	}
public:
	WatchData(const wxString& expr)
		: m_voc(NULL)
		, m_expression(expr)
		, m_isFirst(false) {
	}

	virtual ~WatchData() {
		if(m_voc) {
			delete m_voc;
		}
		m_expression.clear();
		m_voc = NULL;
	}
};

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
WatchesTable::WatchesTable( wxWindow* parent )
	: SimpleTableBase( parent )
	, m_selectedId(wxNOT_FOUND)
	, m_displayFormat(DBG_DF_NATURAL)
{
	InitTable();

	//Load the right click menu
	m_rclickMenu = wxXmlResource::Get()->LoadMenu(wxT("dbg_watch_rmenu"));
	Connect(XRCID("del_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnDeleteWatch), NULL, this);
	Connect(XRCID("del_expr_all"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnDeleteAll), NULL, this);
	Connect(XRCID("edit_expr"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuEditExpr), NULL, this);
	Connect(XRCID("copy_value"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuCopyValue), NULL, this);
	Connect(XRCID("add_watch"), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnNewWatch_Internal), NULL, this);
	Connect(XRCID("copy_both"),wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WatchesTable::OnMenuCopyBoth), NULL, this);

	// UI events
	Connect(XRCID("edit_expr"),wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WatchesTable::OnMenuEditExprUI), NULL, this);
	Connect(XRCID("del_expr"),wxEVT_UPDATE_UI, wxUpdateUIEventHandler( WatchesTable::OnDeleteWatchUI), NULL, this);
}

WatchesTable::~WatchesTable()
{
	if (m_rclickMenu) {
		delete m_rclickMenu;
		m_rclickMenu = NULL;
	}

	Clear();
	m_listTable->DeleteRoot();
}

void WatchesTable::Clear()
{
	wxTreeItemId root = m_listTable->GetRootItem();
	if(root.IsOk()) {
		if(m_listTable->HasChildren(root)) {
			m_listTable->DeleteChildren(root);
		}
	}
}

void WatchesTable::InitTable()
{
	m_listTable->AddColumn(wxT("Expression"), 150);
	m_listTable->AddColumn(wxT("Value"),      1000);
	m_listTable->AddRoot(wxT("Watches"));
	
	m_listTable->SetColumnEditable(0);
}

void WatchesTable::OnListKeyDown( wxTreeEvent& event )
{
	if (event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_NUMPAD_DELETE) {

		wxTreeItemId item = m_listTable->GetSelection();
		if(item.IsOk() == false)
			return;

		DoDeleteWatch(item);
		m_listTable->Delete(item);

	} else {
		event.Skip();

	}
}

void WatchesTable::OnItemRightClick(wxTreeEvent &event)
{
	wxUnusedVar(event);
	if (m_rclickMenu) {
		PopupMenu(m_rclickMenu);
	}
}

void WatchesTable::OnNewWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxString expr = wxGetTextFromUser(wxT("Expression to watch:"), wxT("New watch"));
	if (expr.IsEmpty() == false) {
		AddExpression(expr);
		RefreshValues(false);
	}
}

void WatchesTable::OnNewWatchUI(wxUpdateUIEvent &event)
{
	event.Enable(true);
}


void WatchesTable::OnDeleteAll(wxCommandEvent &event)
{
	wxUnusedVar(event);
	Clear();
}

void WatchesTable::OnDeleteAllUI(wxUpdateUIEvent &event)
{
	wxTreeItemId root = m_listTable->GetRootItem();

	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_listTable->GetFirstChild(root, cookie);

	event.Enable(root.IsOk() && child.IsOk());
}

void WatchesTable::OnDeleteWatch(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTreeItemId item = m_listTable->GetSelection();
	if(item.IsOk() == false)
		return;

	DoDeleteWatch(item);
	m_listTable->Delete(item);
}

void WatchesTable::OnDeleteWatchUI(wxUpdateUIEvent &event)
{
	wxTreeItemId item = m_listTable->GetSelection();
	event.Enable(item.IsOk() && m_listTable->GetItemParent(item) == m_listTable->GetRootItem());
}

void WatchesTable::AddExpression(const wxString &expr)
{
	//make sure that the expression does not exist
	wxTreeItemId root = m_listTable->GetRootItem();
	if(!root.IsOk()) {
		return;
	}

	// Obtain the debugger and make sure that we can interact with it
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if(!dbgr || !ManagerST::Get()->DbgCanInteract())
		return;

	if(m_listTable->HasChildren(root)) {
		wxTreeItemIdValue cookie;
		wxTreeItemId item = m_listTable->GetFirstChild(root, cookie);
		while(item.IsOk()) {
			if(m_listTable->GetItemText(item) == expr) {
				wxMessageBox(_("A similar expression already exist in the watches table"), wxT("CodeLite"), wxOK | wxICON_INFORMATION);
				return;
			}
			item = m_listTable->GetNextChild(root, cookie);
		}
	}

	// Append the new item and call the debugger to create a new variable object for this
	// expression
	wxTreeItemId item = m_listTable->AppendItem(root, expr, -1, -1, new WatchData(expr));
	dbgr->CreateVariableObject(expr, DBG_USERR_WATCHTABLE);
	m_exprToItemId[expr] = item;
}

void WatchesTable::RefreshValues(bool repositionEditor)
{
	//indicate in the global manager if we want to actually reposition the editor's position after the dbgr->QueryFileLine() refresh
	ManagerST::Get()->SetRepositionEditor(repositionEditor);

	// loop over the childrens, if we got a valid variable object, re-evaluate it
	wxTreeItemId root = m_listTable->GetRootItem();
	if(root.IsOk() == false)
		return;

	// Obtain the debugger and make sure that we can interact with it
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if(!dbgr || !ManagerST::Get()->DbgCanInteract())
		return;

	DoRefreshItemRecursively(dbgr, root);
}

void WatchesTable::DoRefreshItemRecursively(IDebugger *dbgr, const wxTreeItemId &item)
{
	wxTreeItemIdValue cookieOne;
	wxTreeItemId exprItem = m_listTable->GetFirstChild(item, cookieOne);
	while( exprItem.IsOk() ) {

		DoRefreshItem(dbgr, exprItem);

		if(m_listTable->HasChildren(exprItem)) {
			DoRefreshItemRecursively(dbgr, exprItem);
		}
		exprItem = m_listTable->GetNextChild(item, cookieOne);
	}
}

void WatchesTable::DoRefreshItem(IDebugger* dbgr, const wxTreeItemId& item)
{
	if(!dbgr || !item.IsOk())
		return;

	WatchData* data = static_cast<WatchData*>(m_listTable->GetItemData(item));
	if(data && data->GetIsFirst()) {
		dbgr->EvaluateVariableObject(data->GetGdbId(), m_displayFormat, DBG_USERR_WATCHTABLE);
		m_gdbIdToTreeId[data->GetGdbId()] = item;

	} else if(data && data->GetVoc()) {
		// re-evaluate the item value
		dbgr->EvaluateVariableObject(data->GetVoc()->gdbId, m_displayFormat, DBG_USERR_WATCHTABLE);
		m_gdbIdToTreeId[data->GetVoc()->gdbId] = item;
	}
}

void WatchesTable::UpdateExpression(const wxString &expr, const wxString &value)
{
}

wxArrayString WatchesTable::GetExpressions()
{
	wxArrayString expressions;

	wxTreeItemId root = m_listTable->GetRootItem();
	if(root.IsOk() && m_listTable->HasChildren(root)) {
		wxTreeItemIdValue cookie;
		wxTreeItemId item = m_listTable->GetFirstChild(root, cookie);
		while(item.IsOk()) {
			expressions.Add( m_listTable->GetItemText(item) ) ;
			item = m_listTable->GetNextChild(root, cookie);
		}
	}

	return expressions;
}

void WatchesTable::OnMenuEditExpr(wxCommandEvent &event)
{
	wxUnusedVar(event);
	wxTreeItemId item = m_listTable->GetSelection();
	if(item.IsOk() == false)
		return;

	IDebugger *dbgr = DoGetDebugger();
	if(!dbgr) {
		return;
	}

	wxString newExpr = wxGetTextFromUser(wxT("Update expression:"), wxT("Update Watch"), m_listTable->GetItemText(item));
	if(newExpr.IsEmpty())
		return;

	DoUpdateExpression(item, newExpr);
}

void WatchesTable::DoUpdateExpression(const wxTreeItemId& item, const wxString &newExpr)
{
	IDebugger *dbgr = DoGetDebugger();
	if(!dbgr) {
		return;
	}

	DoDeleteWatch(item);
	if(m_listTable->HasChildren(item)) {
		m_listTable->Collapse(item);
		m_listTable->DeleteChildren(item);
	}

	// update the expression
	m_listTable->SetItemText(item, newExpr);

	// remove any data associate with the item
	WatchData *data = (WatchData*)m_listTable->GetItemData(item);
	if(data) {
		delete data;
	}
	m_listTable->SetItemData(item, new WatchData(newExpr));

	// reset the previous value
	m_listTable->SetItemText(item, 1, wxT(""));

	dbgr->CreateVariableObject(newExpr, DBG_USERR_WATCHTABLE);
	m_exprToItemId[newExpr] = item;
}

void WatchesTable::OnMenuCopyBoth(wxCommandEvent& event)
{
	wxTreeItemId item = m_listTable->GetSelection();
	if(item.IsOk()) {
		CopyToClipboard( m_listTable->GetItemText(item) + wxT(" ") + m_listTable->GetItemText(item, 1) );
	}
}

void WatchesTable::OnMenuCopyValue(wxCommandEvent& event)
{
	wxTreeItemId item = m_listTable->GetSelection();
	if(item.IsOk()) {
		CopyToClipboard( m_listTable->GetItemText(item, 1) );
	}
}

void WatchesTable::OnDisplayFormat(wxCommandEvent& event)
{
	wxString selection = m_choiceDisplayFormat->GetStringSelection();
	if(selection == wxT("natural")) {
		m_displayFormat = DBG_DF_NATURAL;

	} else if(selection == wxT("hexadecimal")) {
		m_displayFormat = DBG_DF_HEXADECIMAL;

	} else if(selection == wxT("binary")) {
		m_displayFormat = DBG_DF_BINARY;

	} else if(selection == wxT("octal")) {
		m_displayFormat = DBG_DF_OCTAL;

	} else if(selection == wxT("decimal")) {
		m_displayFormat = DBG_DF_DECIMAL;

	} else {
		m_displayFormat = DBG_DF_NATURAL;
	}
}

void WatchesTable::OnNewWatch_Internal(wxCommandEvent& event)
{
	wxString expr = event.GetString();
	if( expr.empty() == false ) {
		AddExpression(expr);
		RefreshValues(false);
	}
}

void WatchesTable::OnEvaluateVariableObject(const DebuggerEvent& event)
{
	wxString gdbId = event.m_expression;
	wxString value = event.m_evaluated;

	std::map<wxString, wxTreeItemId>::iterator iter = m_gdbIdToTreeId.find(gdbId);
	if( iter != m_gdbIdToTreeId.end() ) {

		wxColour defColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
		wxColour redColour = *wxRED;
		wxColour itemColor;

		wxString newValue = value;
		wxString curValue = m_listTable->GetItemText(iter->second, 1);

		if(newValue == curValue || curValue.IsEmpty())
			itemColor = defColour;
		else
			itemColor = redColour;

		m_listTable->SetItemText(iter->second, 1, value);
		m_listTable->SetItemTextColour(iter->second, itemColor);

		m_gdbIdToTreeId.erase(iter);
	}
}

void WatchesTable::OnCreateVariableObject(const DebuggerEvent& event)
{
	wxString expr = event.m_expression;

	std::map<wxString, wxTreeItemId>::iterator iter = m_exprToItemId.find(expr);
	if( iter != m_exprToItemId.end() ) {
		// set the variable object
		WatchData* data = static_cast<WatchData*>(m_listTable->GetItemData(iter->second));
		if(data) {
			data->SetExpression( expr );
			data->SetIsFirst(true);
			data->SetGdbId(event.m_variableObject.gdbId);

			// refresh this item only
			IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
			if(dbgr && ManagerST::Get()->DbgCanInteract())
				DoRefreshItem(dbgr, iter->second);

			// Query the debugger to see if this node has a children
			// In case it does, we add a dummy node so we will get the [+] sign
			dbgr->ListChildren(data->GetGdbId(), QUERY_NUM_CHILDS);
			m_listChildItemId[data->GetGdbId()] = iter->second;
		}
		m_exprToItemId.erase(iter);
	}
}

void WatchesTable::OnListChildren(const DebuggerEvent& event)
{
	wxString gdbId = event.m_expression;
	std::map<wxString, wxTreeItemId>::iterator iter = m_listChildItemId.find(gdbId);
	if(iter == m_listChildItemId.end())
		return;

	wxTreeItemId item = iter->second;
	m_listChildItemId.erase(iter);

	switch(event.m_userReason) {
	case QUERY_NUM_CHILDS:
		if(event.m_varObjChildren.empty() == false)
			m_listTable->AppendItem(item, wxT("<dummy>"));
		break;
	case LIST_WATCH_CHILDS:
		if(event.m_varObjChildren.empty() == false) {
			for(size_t i=0; i<event.m_varObjChildren.size(); i++) {
				IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
				if(!dbgr || !ManagerST::Get()->DbgCanInteract())
					return;

				VariableObjChild ch = event.m_varObjChildren.at(i);
				if(ch.varName == wxT("public") || ch.varName == wxT("private") || ch.varName == wxT("protected")) {
					// not really a node...
					// ask for information about this node children
					dbgr->ListChildren(ch.gdbId, LIST_WATCH_CHILDS);
					m_listChildItemId[ch.gdbId] = item;

				} else {

					WatchData *data = new WatchData(wxT(""));
					data->SetGdbId(ch.gdbId);
					data->SetVoc(ch);
					wxTreeItemId child = m_listTable->AppendItem(item, ch.varName, -1, -1, data);

					// Add a dummy node
					if(child.IsOk() && ch.numChilds > 0) {
						m_listTable->AppendItem(child, wxT("<dummy>"));
					}

					// refresh this item only
					dbgr->EvaluateVariableObject(data->GetGdbId(), m_displayFormat, DBG_USERR_WATCHTABLE);
					// ask the value for this node
					m_gdbIdToTreeId[data->GetGdbId()] = child;

				}
			}
		}
		break;
	}
}

void WatchesTable::OnItemExpanding(wxTreeEvent& event)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_listTable->GetFirstChild(event.GetItem(), cookie);

	IDebugger *dbgr = DoGetDebugger();
	if(!dbgr) {
		// dont allow the expansion of this item
		event.Veto();
		return;
	}

	if(child.IsOk() && m_listTable->GetItemText(child) == wxT("<dummy>")) {
		// a dummy node, replace it with the real node content
		m_listTable->Delete(child);

		WatchData *data = (WatchData*) m_listTable->GetItemData(event.GetItem());
		if(data) {
			wxString gdbId;
			if(data->GetIsFirst()) {
				gdbId = data->GetGdbId();

			} else if (data->GetVoc()) {
				gdbId = data->GetVoc()->gdbId;

			}

			if(gdbId.IsEmpty() == false) {
				dbgr->ListChildren(gdbId, LIST_WATCH_CHILDS);
				m_listChildItemId[gdbId] = event.GetItem();
			}
		}
	}
}

void WatchesTable::DoDeleteWatch(const wxTreeItemId& item)
{
	IDebugger *dbgr = DoGetDebugger();
	if(!dbgr || !item.IsOk()) {
		return;
	}

	wxString gdbId = DoGetGdbId(item);
	if(gdbId.IsEmpty() == false) {
		dbgr->DeleteVariableObject(gdbId);
	}

#ifdef __WXMAC__

	// Mac's GDB does not delete all the children of the variable object
	// instead we will do it manually

	if(m_listTable->HasChildren(item)) {
		// Delete this item children
		wxTreeItemIdValue cookie;
		wxTreeItemId child = m_listTable->GetFirstChild(item, cookie);
		while(child.IsOk()) {
			gdbId = DoGetGdbId(child);
			if(gdbId.IsEmpty() == false) {
				dbgr->DeleteVariableObject(gdbId);
			}

			if(m_listTable->HasChildren(child)) {
				DoDeleteWatch(child);
			}

			child = m_listTable->GetNextChild(item, cookie);
		}
	}
#endif

}

IDebugger* WatchesTable::DoGetDebugger()
{
	if(!ManagerST::Get()->DbgCanInteract())
		return NULL;

	IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
	return dbgr;
}

wxString WatchesTable::DoGetGdbId(const wxTreeItemId& item)
{
	wxString gdbId;
	if(!item.IsOk())
		return gdbId;

	WatchData *data = (WatchData*) m_listTable->GetItemData(item);
	if(data) {
		if(data->GetIsFirst()) {
			gdbId = data->GetGdbId();

		} else if (data->GetVoc()) {
			gdbId = data->GetVoc()->gdbId;
		}
	}
	return gdbId;
}

void WatchesTable::OnMenuEditExprUI(wxUpdateUIEvent& event)
{
	// we enable it for root elements only
	wxTreeItemId item = m_listTable->GetSelection();
	event.Enable(item.IsOk() && m_listTable->GetItemParent(item) == m_listTable->GetRootItem());
}

void WatchesTable::OnListEditLabelBegin(wxTreeEvent& event)
{
	event.Skip();
}

void WatchesTable::OnListEditLabelEnd(wxTreeEvent& event)
{
	if(event.GetItem().IsOk()) {
		DoUpdateExpression(event.GetItem(), event.GetLabel());
	}
	event.Skip();
}
