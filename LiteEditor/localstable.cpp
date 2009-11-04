#include "localstable.h"
#include "globals.h"
#include "debuggermanager.h"
#include "manager.h"
#include "new_quick_watch_dlg.h"

#define LOCAL_NAME_COL      0
#define LOCAL_TYPE_COL      1
#define LOCAL_VALUE_COL     2

LocalsTable::LocalsTable(wxWindow *parent)
		: LocalsTableBase(parent)
{
	m_listTable->InsertColumn(LOCAL_NAME_COL, wxT("Name"));
	m_listTable->InsertColumn(LOCAL_TYPE_COL, wxT("Type"));
	m_listTable->InsertColumn(LOCAL_VALUE_COL, wxT("Value"));

	m_listTable->SetColumnWidth(LOCAL_NAME_COL, 200);
	m_listTable->SetColumnWidth(LOCAL_TYPE_COL, 200);
	m_listTable->SetColumnWidth(LOCAL_VALUE_COL, 200);
}

LocalsTable::~LocalsTable()
{
}

void LocalsTable::OnItemActivated(wxListEvent& event)
{
	DoShowDetails( event.m_itemIndex );
}

void LocalsTable::OnItemSelected(wxListEvent& event)
{
	//DoShowDetails( event.m_itemIndex );
	event.Skip();
}

void LocalsTable::UpdateLocals(const LocalVariables& locals)
{
	m_listTable->Freeze();
	m_listTable->DeleteAllItems();
	for(size_t i=0; i<locals.size(); i++){
		LocalVariable var = locals.at(i);
		long idx = AppendListCtrlRow(m_listTable);
		SetColumnText(m_listTable, idx, LOCAL_NAME_COL, var.name  );
		SetColumnText(m_listTable, idx, LOCAL_TYPE_COL, var.type  );
		SetColumnText(m_listTable, idx, LOCAL_VALUE_COL, var.value );
	}
	m_listTable->Thaw();
}

void LocalsTable::DoShowDetails(long item)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	long sel = m_listTable->GetFirstSelected();
	if( sel != wxNOT_FOUND ){
		wxString name = GetColumnText(m_listTable, sel, LOCAL_NAME_COL);
		if( dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract() ) {

			if ( ManagerST::Get()->GetQuickWatchDialog()->IsShown() ) {
				ManagerST::Get()->GetQuickWatchDialog()->HideDialog();
			}

			dbgr->CreateVariableObject(name, DBG_USERR_LOCALS);
		}
	}
}

long LocalsTable::DoGetIdxByName(const wxString& name)
{
	for( int i=0; i<m_listTable->GetItemCount(); i++){
		if(GetColumnText(m_listTable, i, LOCAL_NAME_COL) == name) {
			return i;
		}
	}
	return wxNOT_FOUND;
}

void LocalsTable::Clear()
{
	m_listTable->DeleteAllItems();
}
