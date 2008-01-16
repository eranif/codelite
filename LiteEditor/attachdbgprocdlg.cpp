#include "attachdbgprocdlg.h"
#include "debuggermanager.h"
#include "globals.h"
#include "procutils.h"
#include <algorithm>
 
/// Ascending sorting function
struct PIDSorter 
{
	bool operator()(const ProcessEntry &rStart, const ProcessEntry &rEnd)
	{
		return rEnd.pid < rStart.pid;
	}
};

struct NameSorter 
{
	bool operator()(const ProcessEntry &rStart, const ProcessEntry &rEnd)
	{
		return rEnd.name.CmpNoCase(rStart.name) < 0;
	}
};

AttachDbgProcDlg::AttachDbgProcDlg( wxWindow* parent )
:
AttachDbgProcBaseDlg( parent )
, m_selectedItem(wxNOT_FOUND)
{
	wxArrayString choices = DebuggerMgr::Get().GetAvailableDebuggers();
	m_choiceDebugger->Append(choices);

	if(choices.IsEmpty() == false){
		m_choiceDebugger->SetSelection(0);
	}

	m_listCtrlProcesses->InsertColumn(0, wxT("PID"));
	m_listCtrlProcesses->InsertColumn(1, wxT("Name"));

	RefreshProcessesList();
}

void AttachDbgProcDlg::RefreshProcessesList(int colToSort)
{
	m_listCtrlProcesses->Freeze();
	m_listCtrlProcesses->DeleteAllItems();

	//Populate the list with list of processes
	std::vector<ProcessEntry> proclist;
	ProcUtils::GetProcessList(proclist);
	
	if(colToSort == 0){//sort by PID
		std::sort(proclist.begin(), proclist.end(), PIDSorter());
	}else if(colToSort == 1){//sort by name
		std::sort(proclist.begin(), proclist.end(), NameSorter());
	}

	for(size_t i=0; i<proclist.size(); i++){
		long item = AppendListCtrlRow(m_listCtrlProcesses);
		ProcessEntry entry = proclist.at(i);
		wxString spid;
		spid << entry.pid;
		SetColumnText(m_listCtrlProcesses, item, 0, spid);
		SetColumnText(m_listCtrlProcesses, item, 1, entry.name);
	}
	m_listCtrlProcesses->SetColumnWidth(0, wxLIST_AUTOSIZE);
	m_listCtrlProcesses->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_listCtrlProcesses->Thaw();
}

wxString AttachDbgProcDlg::GetExeName() const
{
	if(m_selectedItem != wxNOT_FOUND){
		return GetColumnText(m_listCtrlProcesses, m_selectedItem, 1);
	}
	return wxEmptyString;
}

wxString AttachDbgProcDlg::GetProcessId() const
{
	if(m_selectedItem != wxNOT_FOUND){
		return GetColumnText(m_listCtrlProcesses, m_selectedItem, 0);
	}
	return wxEmptyString;
}


void AttachDbgProcDlg::OnSortColumn( wxListEvent& event )
{
	RefreshProcessesList( event.m_col );
}

void AttachDbgProcDlg::OnItemActivated( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	EndModal(wxID_OK);
}

void AttachDbgProcDlg::OnItemDeselected( wxListEvent& event )
{
	m_selectedItem = wxNOT_FOUND;
	wxUnusedVar(event);
}

void AttachDbgProcDlg::OnItemSelected( wxListEvent& event )
{
	m_selectedItem = event.m_itemIndex;
	wxUnusedVar(event);
}

void AttachDbgProcDlg::OnBtnAttachUI( wxUpdateUIEvent& event )
{
	event.Enable(m_selectedItem != wxNOT_FOUND);
}
