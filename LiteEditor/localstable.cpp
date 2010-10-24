#include "localstable.h"
#include <wx/wupdlock.h>
#include "debuggerconfigtool.h"
#include "globals.h"
#include "debuggermanager.h"
#include "manager.h"
#include "new_quick_watch_dlg.h"
#include <set>

class LocalsData : public wxTreeItemData
{
public:
	wxString _gdbId;
	size_t   _kind;

public:
	LocalsData()
	: _kind(LocalsTable::Locals)
	{}

	LocalsData(const wxString &gdbId)
	: _gdbId(gdbId)
	{}

	virtual ~LocalsData()
	{}
};

LocalsTable::LocalsTable(wxWindow *parent)
	: LocalsTableBase(parent)
{
	m_listTable->AddColumn(wxT("Name"), 150);
	m_listTable->AddColumn(wxT("Value"), 1000);
	m_listTable->AddRoot(wxT("Locals"));
}

LocalsTable::~LocalsTable()
{
}

void LocalsTable::UpdateLocals(const LocalVariables& locals)
{
	DoUpdateLocals(locals, LocalsTable::Locals);
}

void LocalsTable::UpdateFuncArgs(const LocalVariables& args)
{
	DoUpdateLocals(args, LocalsTable::FuncArgs);
}

void LocalsTable::Clear()
{
	wxTreeItemId root = m_listTable->GetRootItem();
	if(root.IsOk()) {
		if(m_listTable->HasChildren(root)) {
			m_listTable->DeleteChildren(root);
		}
	}

	m_listChildItemId.clear();
	m_createVarItemId.clear();
	m_gdbIdToTreeId.clear();
}

void LocalsTable::Initialize()
{
	// Read the debugger defined commands
	DebuggerSettingsPreDefMap data;
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &data);

	m_preDefTypes = data.GetActiveSet();
}

void LocalsTable::OnCreateVariableObj(const DebuggerEvent& event)
{
	wxString expr = event.m_expression;
	std::map<wxString, wxTreeItemId>::iterator iter = m_createVarItemId.find(expr);
	if( iter != m_createVarItemId.end() ) {

		// set the variable object
		LocalsData* data = static_cast<LocalsData*>(m_listTable->GetItemData(iter->second));
		if(data) {

			data->_gdbId = event.m_variableObject.gdbId;
			data->_kind  = LocalsTable::VariableObject;

			// refresh this item only
			IDebugger *dbgr = DoGetDebugger();
			if(dbgr)
				DoRefreshItem(dbgr, iter->second);

			dbgr->UpdateVariableObject(data->_gdbId, DBG_USERR_LOCALS);
			dbgr->ListChildren(data->_gdbId, LIST_LOCALS_CHILDS);
			m_listChildItemId[data->_gdbId] = iter->second;

		}
		m_createVarItemId.erase(iter);
	}
}

void LocalsTable::OnEvaluateVariableObj(const DebuggerEvent& event)
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
		//m_listTable->SetItemTextColour(iter->second, itemColor);

		// keep the red items IDs in the array
		m_gdbIdToTreeId.erase(iter);
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

	switch(event.m_userReason) {
	case LIST_LOCALS_CHILDS:
		if(event.m_varObjChildren.empty() == false) {
			for(size_t i=0; i<event.m_varObjChildren.size(); i++) {

				IDebugger *dbgr = DoGetDebugger();
				if(!dbgr)
					return;

				VariableObjChild ch = event.m_varObjChildren.at(i);
				if(ch.varName == wxT("public") || ch.varName == wxT("private") || ch.varName == wxT("protected")) {
					// not really a node...
					// ask for information about this node children
					dbgr->ListChildren(ch.gdbId, LIST_LOCALS_CHILDS);
					m_listChildItemId[ch.gdbId] = item;

				} else {

					LocalsData *data = new LocalsData();
					data->_gdbId = ch.gdbId;
					wxTreeItemId child = m_listTable->AppendItem(item, ch.varName, -1, -1, data);

					// Add a dummy node
					if(child.IsOk() && ch.numChilds > 0) {
						m_listTable->AppendItem(child, wxT("<dummy>"));
					}

					// refresh this item only
					dbgr->EvaluateVariableObject(data->_gdbId, DBG_USERR_LOCALS);
					// ask the value for this node
					m_gdbIdToTreeId[data->_gdbId] = child;

				}
			}
		}
		break;
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

IDebugger* LocalsTable::DoGetDebugger()
{
	if(!ManagerST::Get()->DbgCanInteract())
		return NULL;

	IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
	return dbgr;
}

wxString LocalsTable::DoGetGdbId(const wxTreeItemId& item)
{
	wxString gdbId;
	if(!item.IsOk())
		return gdbId;

	LocalsData *data = (LocalsData*) m_listTable->GetItemData(item);
	if(data) {
		return data->_gdbId;
	}
	return gdbId;
}

void LocalsTable::DoDeleteWatch(const wxTreeItemId& item)
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

	if(child.IsOk() && m_listTable->GetItemText(child) == wxT("<dummy>")) {

		// a dummy node, replace it with the real node content
		m_listTable->Delete(child);

		wxString gdbId = DoGetGdbId(event.GetItem());
		if(gdbId.IsEmpty() == false) {
			dbgr->UpdateVariableObject(gdbId, DBG_USERR_LOCALS);
			dbgr->ListChildren(gdbId, LIST_LOCALS_CHILDS);
			m_listChildItemId[gdbId] = event.GetItem();

		} else {
			// first time
			// create a variable object
			dbgr->CreateVariableObject(m_listTable->GetItemText(event.GetItem()), DBG_USERR_LOCALS);
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
			LocalsData* data = static_cast<LocalsData*>( m_listTable->GetItemData(item) );
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


void LocalsTable::DoRefreshItem(IDebugger* dbgr, const wxTreeItemId& item)
{
	if(!dbgr || !item.IsOk())
		return;

	LocalsData* data = static_cast<LocalsData*>(m_listTable->GetItemData(item));
	if(data && data->_gdbId.IsEmpty() == false) {

		dbgr->EvaluateVariableObject(data->_gdbId, DBG_USERR_LOCALS);
		m_gdbIdToTreeId[data->_gdbId] = item;

	}
}

void LocalsTable::DoRefreshItemRecursively(IDebugger *dbgr, const wxTreeItemId &item, wxArrayString &itemsToRefresh)
{
	if(itemsToRefresh.IsEmpty())
		return;

	wxTreeItemIdValue cookieOne;
	wxTreeItemId exprItem = m_listTable->GetFirstChild(item, cookieOne);
	while( exprItem.IsOk() ) {

		LocalsData* data = static_cast<LocalsData*>(m_listTable->GetItemData(exprItem));
		if(data) {
			int where = itemsToRefresh.Index(data->_gdbId);
			if(where != wxNOT_FOUND) {
				dbgr->EvaluateVariableObject(data->_gdbId, DBG_USERR_LOCALS);
				m_gdbIdToTreeId[data->_gdbId] = exprItem;
				itemsToRefresh.RemoveAt((size_t)where);
			}
		}

		if(m_listTable->HasChildren(exprItem)) {
			DoRefreshItemRecursively(dbgr, exprItem, itemsToRefresh);
		}
		exprItem = m_listTable->GetNextChild(item, cookieOne);
	}
}

wxTreeItemId LocalsTable::DoFindItemByGdbId(const wxString& gdbId)
{
	wxTreeItemId root = m_listTable->GetRootItem();
	wxTreeItemIdValue cookieOne;
	wxTreeItemId item = m_listTable->GetFirstChild(root, cookieOne);
	while( item.IsOk() ) {

		wxString id = DoGetGdbId(item);
		if(id.IsEmpty() == false && id == gdbId)
			return item;

		item = m_listTable->GetNextChild(root, cookieOne);
	}
	return wxTreeItemId();
}


void LocalsTable::DoUpdateLocals(const LocalVariables& locals, size_t kind)
{
	wxTreeItemId root = m_listTable->GetRootItem();
	if(!root.IsOk())
		return;

	wxArrayString itemsNotRemoved;
	// remove the non-variable objects and return a list
	// of all the variable objects (at the top level)
	DoClearNonVariableObjectEntries(itemsNotRemoved, kind);
	for(size_t i=0; i<locals.size(); i++) {

		if(itemsNotRemoved.Index(locals[i].name) == wxNOT_FOUND) {
			// New entry
			wxTreeItemId item = m_listTable->AppendItem(root, locals[i].name, -1, -1, new LocalsData());
			m_listTable->SetItemText(item, 1, locals[i].value);

			m_listTable->AppendItem(item, wxT("<dummy>"));
			m_listTable->Collapse(item);

		}
	}

	IDebugger* dbgr = DoGetDebugger();
	if(dbgr && itemsNotRemoved.IsEmpty() == false) {
		dbgr->UpdateVariableObject(wxT("*"), DBG_USERR_LOCALS);
	}

}
