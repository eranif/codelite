#include "localstable.h"
#include <wx/wupdlock.h>
#include "debuggerconfigtool.h"
#include "globals.h"
#include "debuggermanager.h"
#include "manager.h"
#include "new_quick_watch_dlg.h"
#include <set>

LocalsTable::LocalsTable(wxWindow *parent)
	: DebuggerTreeListCtrlBase(parent, wxID_ANY, false)
{
	m_listTable->AddColumn(wxT("Name"), 150);
	m_listTable->AddColumn(wxT("Value"), 500);
	m_listTable->AddColumn(wxT("Type"), 200);
	m_listTable->AddRoot(wxT("Locals"));

	m_DBG_USERR        = DBG_USERR_LOCALS;
	m_QUERY_NUM_CHILDS = QUERY_LOCALS_CHILDS;
	m_LIST_CHILDS      = LIST_LOCALS_CHILDS;
}

LocalsTable::~LocalsTable()
{
}

void LocalsTable::UpdateLocals(const LocalVariables& locals)
{
	DoResetItemColour(m_listTable->GetRootItem());
	DoUpdateLocals(locals, DbgTreeItemData::Locals);
}

void LocalsTable::UpdateFuncArgs(const LocalVariables& args)
{
	DoUpdateLocals(args, DbgTreeItemData::FuncArgs);
}

void LocalsTable::Initialize()
{
	// Read the debugger defined commands
	DebuggerSettingsPreDefMap data;
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &data);

	DebuggerInformation info;
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if(dbgr) {
		DebuggerMgr::Get().GetDebuggerInformation(dbgr->GetName(), info);
		m_resolveLocals = info.resolveLocals;
	}

	m_preDefTypes = data.GetActiveSet();
}

void LocalsTable::OnCreateVariableObj(const DebuggerEvent& event)
{
	wxString expr = event.m_expression;
	std::map<wxString, wxTreeItemId>::iterator iter = m_createVarItemId.find(expr);
	if( iter != m_createVarItemId.end() ) {

		// set the variable object
		DbgTreeItemData* data = static_cast<DbgTreeItemData*>(m_listTable->GetItemData(iter->second));
		if(data) {

			data->_gdbId = event.m_variableObject.gdbId;
			data->_kind  = DbgTreeItemData::VariableObject;
			
			// variable object's type name is extracted from the event.m_variableObject.typeName
			m_listTable->SetItemText(iter->second, 2, event.m_variableObject.typeName);
			
			// refresh this item only
			IDebugger *dbgr = DoGetDebugger();
			if(dbgr)
				DoRefreshItem(dbgr, iter->second, false);

			dbgr->UpdateVariableObject(data->_gdbId, m_DBG_USERR);
			dbgr->ListChildren(data->_gdbId, m_LIST_CHILDS);
			m_listChildItemId[data->_gdbId] = iter->second;

		}
		m_createVarItemId.erase(iter);
	}
}

void LocalsTable::OnListChildren(const DebuggerEvent& event)
{
	wxString gdbId = event.m_expression;
	std::map<wxString, wxTreeItemId>::iterator iter = m_listChildItemId.find(gdbId);
	if(iter == m_listChildItemId.end())
		return;

	wxTreeItemId item = iter->second;
	m_listChildItemId.erase(iter);

	if(event.m_userReason == m_LIST_CHILDS) {
		if(event.m_varObjChildren.empty() == false) {
			for(size_t i=0; i<event.m_varObjChildren.size(); i++) {

				IDebugger *dbgr = DoGetDebugger();
				if(!dbgr)
					return;

				VariableObjChild ch = event.m_varObjChildren.at(i);
				if(ch.varName == wxT("public") || ch.varName == wxT("private") || ch.varName == wxT("protected")) {
					// not really a node...
					// ask for information about this node children
					dbgr->ListChildren(ch.gdbId, m_LIST_CHILDS);
					m_listChildItemId[ch.gdbId] = item;

				} else {

					DbgTreeItemData *data = new DbgTreeItemData();
					data->_gdbId = ch.gdbId;
					wxTreeItemId child = m_listTable->AppendItem(item, ch.varName, -1, -1, data);

					// Add a dummy node
					if(child.IsOk() && ch.numChilds > 0) {
						m_listTable->AppendItem(child, wxT("<dummy>"));
					}

					// refresh this item only
					dbgr->EvaluateVariableObject(data->_gdbId, m_DBG_USERR);
					// ask the value for this node
					m_gdbIdToTreeId[data->_gdbId] = child;

				}
			}
		}
	}
}

void LocalsTable::OnVariableObjUpdate(const DebuggerEvent& event)
{
	VariableObjectUpdateInfo updateInfo = event.m_varObjUpdateInfo;

	// remove all obsolete items
	for(size_t i=0; i<updateInfo.removeIds.GetCount(); i++) {
		wxString    gdbId = updateInfo.removeIds.Item(i);
		wxTreeItemId item = DoFindItemByGdbId(gdbId);
		if(item.IsOk()) {
			DoDeleteWatch(item);
			m_listTable->Delete(item);
		}
	}

	// refresh the values of the items that requires that
	IDebugger* dbgr = DoGetDebugger();
	if(dbgr) {
		wxArrayString itemsToRefresh = event.m_varObjUpdateInfo.refreshIds;
		DoRefreshItemRecursively(dbgr, m_listTable->GetRootItem(), itemsToRefresh);
	}
}

void LocalsTable::OnItemExpanding(wxTreeEvent& event)
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = m_listTable->GetFirstChild(event.GetItem(), cookie);

	IDebugger *dbgr = DoGetDebugger();
	if(!dbgr || !event.GetItem()) {
		// dont allow the expansion of this item
		event.Veto();
		return;
	}

	size_t childCount = m_listTable->GetChildrenCount(event.GetItem());
	if(childCount > 1) {
		// make sure there is no <dummy> node and continue
		wxTreeItemIdValue cookieOne;
		wxTreeItemId dummyItem = m_listTable->GetFirstChild(event.GetItem(), cookieOne);
		while( dummyItem.IsOk() )
		{
			if(m_listTable->GetItemText(dummyItem) == wxT("<dummy>")) {
				m_listTable->Delete(dummyItem);
				event.Skip();
				return;
			}
			dummyItem = m_listTable->GetNextChild(event.GetItem(), cookieOne);
		}

	} else if (child.IsOk() && m_listTable->GetItemText(child) == wxT("<dummy>")) {

		// a dummy node, replace it with the real node content
		m_listTable->Delete(child);

		wxString gdbId = DoGetGdbId(event.GetItem());
		if(gdbId.IsEmpty() == false) {
			dbgr->UpdateVariableObject(gdbId, m_DBG_USERR);
			dbgr->ListChildren(gdbId, m_LIST_CHILDS);
			m_listChildItemId[gdbId] = event.GetItem();

		} else {
			// first time
			// create a variable object
			dbgr->CreateVariableObject(m_listTable->GetItemText(event.GetItem()), m_DBG_USERR);
			m_createVarItemId[m_listTable->GetItemText(event.GetItem())] = event.GetItem();
		}
	}
}

void LocalsTable::DoClearNonVariableObjectEntries(wxArrayString& itemsNotRemoved, size_t flags)
{
	wxTreeItemIdValue cookie;
	std::vector<wxTreeItemId> itemsToRemove;

	wxTreeItemId item = m_listTable->GetFirstChild(m_listTable->GetRootItem(), cookie);
	while( item.IsOk() ) {
		wxString gdbId = DoGetGdbId(item);
		if(gdbId.IsEmpty()) {
			DbgTreeItemData* data = static_cast<DbgTreeItemData*>( m_listTable->GetItemData(item) );
			if(data && (data->_kind & flags)) {
				// not a variable object entry, remove it
				itemsToRemove.push_back(item);
			}

		} else {
			itemsNotRemoved.Add( m_listTable->GetItemText(item) );
		}
		item = m_listTable->GetNextChild(m_listTable->GetRootItem(), cookie);
	}

	for(size_t i=0; i<itemsToRemove.size(); i++) {
		m_listTable->Delete( itemsToRemove[i] );
	}
}

void LocalsTable::DoUpdateLocals(const LocalVariables& locals, size_t kind)
{
	wxTreeItemId root = m_listTable->GetRootItem();
	if(!root.IsOk())
		return;

	IDebugger* dbgr = DoGetDebugger();
	wxArrayString itemsNotRemoved;
	// remove the non-variable objects and return a list
	// of all the variable objects (at the top level)
	DoClearNonVariableObjectEntries(itemsNotRemoved, kind);
	for(size_t i=0; i<locals.size(); i++) {

		// try to replace the
		wxString newVarName;
		if(m_resolveLocals) {
			newVarName = m_preDefTypes.GetPreDefinedTypeForTypename(locals[i].type, locals[i].name);
		}

		if(newVarName.IsEmpty() == false) {
			if(newVarName.Contains(wxT("@"))) {

				// using GDB special array print,
				// we need to delete this variable object and re-create it
				// otherwise its content wont be updated
				int where = itemsNotRemoved.Index(newVarName);
				if(where != wxNOT_FOUND)
					itemsNotRemoved.RemoveAt(where);

				wxTreeItemId treeItem = DoFindItemByExpression(newVarName);
				if(treeItem.IsOk()) {
					DoDeleteWatch(treeItem);
					m_listTable->Delete(treeItem);
				}
			}

			// replace the local with a variable object
			// but make sure we dont enter a duplicate item
			if(itemsNotRemoved.Index(newVarName) == wxNOT_FOUND) {
				// this type has a pre-defined type, use it instead
				wxTreeItemId item = m_listTable->AppendItem(root, newVarName, -1, -1, new DbgTreeItemData());
				m_listTable->AppendItem(item, wxT("<dummy>"));
				m_listTable->Collapse(item);

				if(dbgr) {
					dbgr->CreateVariableObject(newVarName, m_DBG_USERR);
					m_createVarItemId[newVarName] = item;
				}
			}

		} else {

			if(itemsNotRemoved.Index(locals[i].name) == wxNOT_FOUND) {
				// New entry
				wxTreeItemId item = m_listTable->AppendItem(root, locals[i].name, -1, -1, new DbgTreeItemData());
				m_listTable->SetItemText(item, 1, locals[i].value);
				m_listTable->SetItemText(item, 2, locals[i].type);
				
				m_listTable->AppendItem(item, wxT("<dummy>"));
				m_listTable->Collapse(item);

			}

		}
	}

	if(dbgr && itemsNotRemoved.IsEmpty() == false) {
		dbgr->UpdateVariableObject(wxT("*"), m_DBG_USERR);
	}

}
