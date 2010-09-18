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
	if ( m_choiceExpand->GetSelection() == 1 ) {
		DoShowDetails( event.m_itemIndex );
	}
}

void LocalsTable::OnItemSelected(wxListEvent& event)
{
	if ( m_choiceExpand->GetSelection() == 0 ) {
		DoShowDetails( event.m_itemIndex );
	}
	event.Skip();
}

void LocalsTable::UpdateLocals(const LocalVariables& locals)
{
	bool evaluatingLocals = true;
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if( dbgr && dbgr->GetDebuggerInformation().resolveLocals == false) {
		evaluatingLocals = false;
	}

	LocalVariables vars = locals;
	// locate all items that were modified
	// this feature is disabled when 'Locals' resolving is enabled
	// due to the async nature of the debugger
	if( !evaluatingLocals ) {
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

	}

	wxWindowUpdateLocker locker ( this );
	Clear();

	for(size_t i=0; i<vars.size(); i++) {
		LocalVariable &var = vars.at(i);
		long idx = AppendListCtrlRow(m_listTable);
		SetColumnText(m_listTable, idx, LOCAL_NAME_COL,  var.name  );
		SetColumnText(m_listTable, idx, LOCAL_TYPE_COL,  var.type  );
		SetColumnText(m_listTable, idx, LOCAL_KIND_COL,  sKindLocalVariable );
		// If this variable has an "inline" value, dont display the row data
		if ( !DoShowInline(var, idx) ) {
			SetColumnText(m_listTable, idx, LOCAL_VALUE_COL, var.value );
			if ( var.updated && evaluatingLocals == false ) {
				m_listTable->SetItemTextColour(idx, wxT("RED"));
			}
		}
	}
}


void LocalsTable::UpdateFuncArgs(const LocalVariables& args)
{
	bool evaluatingLocals = true;
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if( dbgr && dbgr->GetDebuggerInformation().resolveLocals == false) {
		evaluatingLocals = false;
	}
	LocalVariables vars = args;

	// locate all items that were modified
	// this feature is disabled when 'Locals' resolving is enabled
	// due to the async nature of the debugger
	if( !evaluatingLocals ) {
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

		if ( !DoShowInline(var, idx) ) {
			SetColumnText(m_listTable, idx, LOCAL_VALUE_COL, var.value );
			if ( var.updated && evaluatingLocals == false ) {
				m_listTable->SetItemTextColour(idx, wxT("RED"));
			}
		}
	}
}

void LocalsTable::DoShowDetails(long item)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	long sel = m_listTable->GetFirstSelected();
	if( sel != wxNOT_FOUND && dbgr ){
		wxString name = GetColumnText(m_listTable, sel, LOCAL_NAME_COL);
		wxString type = GetColumnText(m_listTable, sel, LOCAL_TYPE_COL);
		wxString expression(name);
		
		// Look to see if the 'type' matches any of the PreDefined types, if it does 
		// and the flag 'Evaluate Locals...' is also set to ON, replace the variable
		// object with the PreDefined expression using 'name' as $(Variable)
		if(dbgr->GetDebuggerInformation().resolveLocals) {
			wxString preDefinedType = m_preDefTypes.GetPreDefinedTypeForTypename(type, name);
			if(preDefinedType.IsEmpty() == false)
				expression = preDefinedType;
		}
		
		if( dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract() ) {

			if ( ManagerST::Get()->GetDebuggerTip()->IsShown() ) {
				ManagerST::Get()->GetDebuggerTip()->HideDialog();
			}

			dbgr->CreateVariableObject(expression, DBG_USERR_LOCALS);
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
	m_expression2Idx.clear();
}

void LocalsTable::Initialize()
{
	// Read the debugger defined commands
	DebuggerSettingsPreDefMap data;
	DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &data);
	
	m_preDefTypes = data.GetActiveSet();
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

bool LocalsTable::DoShowInline(const LocalVariable& var, long item)
{
	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if( dbgr && dbgr->GetDebuggerInformation().resolveLocals == false) {
		return false;
	}
	
	wxString preDefinedType = m_preDefTypes.GetPreDefinedTypeForTypename(var.type, var.name);
	if(preDefinedType.IsEmpty())
		return false;
	
	if( dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract() ) {
		dbgr->CreateVariableObject(preDefinedType, DBG_USERR_LOCALS_INLINE);
		m_expression2Idx[preDefinedType] = item;
		return true;
	}
	
	return false;
}

void LocalsTable::UpdateInline(const DebuggerEvent& event)
{
	wxString key = event.m_expression;
	std::map<wxString, long>::iterator iter = m_expression2Idx.find(key);
	if(iter != m_expression2Idx.end()){
		long idx = iter->second;
		SetColumnText(m_listTable, idx, LOCAL_VALUE_COL, event.m_evaluated);
	}

	IDebugger *dbgr = DebuggerMgr::Get().GetActiveDebugger();
	if(dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
		dbgr->DeleteVariableObject(event.m_variableObject.gdbId);
	}
}

