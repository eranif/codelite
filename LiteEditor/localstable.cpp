#include "localstable.h"
#include <wx/wupdlock.h>
#include "debuggerconfigtool.h"
#include "globals.h"
#include "debuggermanager.h"
#include "manager.h"
#include "new_quick_watch_dlg.h"
#include <set>

#define LOCAL_NAME_COL      0
#define LOCAL_TYPE_COL      1
#define LOCAL_VALUE_COL     2
#define LOCAL_KIND_COL      3

static const wxString sKindLocalVariable   (wxT("Local Variable"));
static const wxString sKindFunctionArgument(wxT("Function Argument"));

LocalsTable::LocalsTable(wxWindow *parent)
		: LocalsTableBase(parent)
{
	m_listTable->InsertColumn(LOCAL_NAME_COL, wxT("Name"));
	m_listTable->InsertColumn(LOCAL_TYPE_COL, wxT("Type"));
	m_listTable->InsertColumn(LOCAL_VALUE_COL, wxT("Value"));
	m_listTable->InsertColumn(LOCAL_KIND_COL, wxT("Kind"));

	m_listTable->SetColumnWidth(LOCAL_NAME_COL, 200);
	m_listTable->SetColumnWidth(LOCAL_TYPE_COL, 200);
	m_listTable->SetColumnWidth(LOCAL_VALUE_COL, 200);
	m_listTable->SetColumnWidth(LOCAL_KIND_COL, 200);
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
	LocalVariables vars = locals;
	// locate all items that were modified
	for(size_t i=0; i<vars.size(); i++){
		LocalVariable &var = vars.at(i);
		wxString      oldValue;

		// try to locate this variable in the table
		long idx = DoGetIdxByVar(var, sKindLocalVariable);
		if ( idx != wxNOT_FOUND ) {
			oldValue = GetColumnText(m_listTable, idx, LOCAL_VALUE_COL);
			var.updated = (oldValue != var.value);
		}
	}

	wxWindowUpdateLocker locker ( this );
	m_listTable->DeleteAllItems();

	for(size_t i=0; i<vars.size(); i++) {
		LocalVariable &var = vars.at(i);
		long idx = AppendListCtrlRow(m_listTable);
		SetColumnText(m_listTable, idx, LOCAL_NAME_COL,  var.name  );
		SetColumnText(m_listTable, idx, LOCAL_TYPE_COL,  var.type  );
		SetColumnText(m_listTable, idx, LOCAL_KIND_COL,  sKindLocalVariable );
		SetColumnText(m_listTable, idx, LOCAL_VALUE_COL, var.value );
		if ( var.updated ) {
			m_listTable->SetItemTextColour(idx, wxT("RED"));
		}
	}
}


void LocalsTable::UpdateFuncArgs(const LocalVariables& args)
{
	// Delete all 'function arguments' first
	LocalVariables vars = args;

	// locate all items that were modified
	for(size_t i=0; i<vars.size(); i++){
		LocalVariable &var = vars.at(i);
		wxString      oldValue;

		// try to locate this variable in the table
		long idx = DoGetIdxByVar(var, sKindFunctionArgument);
		if ( idx != wxNOT_FOUND ) {
			oldValue = GetColumnText(m_listTable, idx, LOCAL_VALUE_COL);
			var.updated = (oldValue != var.value);
		}
	}

	wxWindowUpdateLocker locker ( this );

	// Delete all function arguments from the table
	for(int i=0; i<m_listTable->GetItemCount(); i++){
		if(GetColumnText(m_listTable, i, LOCAL_KIND_COL) == sKindFunctionArgument) {
			m_listTable->DeleteItem(i);
		}
	}

	for(size_t i=0; i<vars.size(); i++) {
		LocalVariable &var = vars.at(i);
		long idx = AppendListCtrlRow(m_listTable);
		SetColumnText(m_listTable, idx, LOCAL_NAME_COL,  var.name  );
		SetColumnText(m_listTable, idx, LOCAL_TYPE_COL,  var.type  );
		SetColumnText(m_listTable, idx, LOCAL_KIND_COL,  sKindFunctionArgument );
		SetColumnText(m_listTable, idx, LOCAL_VALUE_COL, var.value );
		if ( var.updated ) {
			m_listTable->SetItemTextColour(idx, wxT("RED"));
		}
	}
}

void LocalsTable::DoShowDetails(long item)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	long sel = m_listTable->GetFirstSelected();
	if( sel != wxNOT_FOUND ){
		wxString name = GetColumnText(m_listTable, sel, LOCAL_NAME_COL);
		if( dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract() ) {

			if ( ManagerST::Get()->GetDisplayVariableDialog()->IsShown() ) {
				ManagerST::Get()->GetDisplayVariableDialog()->HideDialog();
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

void LocalsTable::Initialize()
{
	// Read the debugger defined commands
	DebuggerSettingsData data;
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &data);
	m_dbgCmds = data.GetCmds();
}

wxString LocalsTable::GetRealType(const wxString& gdbType)
{
	wxString realType ( gdbType );
	realType.Replace(wxT("*"), wxT(""));
	realType.Replace(wxT("const"), wxT(""));
	realType.Replace(wxT("&"), wxT(""));

	realType.Trim().Trim(false);
	return realType;
}

long LocalsTable::DoGetIdxByVar(const LocalVariable& var, const wxString& kind)
{
	for( int i=0; i<m_listTable->GetItemCount(); i++){
		if( GetColumnText(m_listTable, i, LOCAL_NAME_COL) == var.name &&
			GetColumnText(m_listTable, i, LOCAL_KIND_COL) == kind)
		{
			return i;
		}
	}
	return wxNOT_FOUND;
}
