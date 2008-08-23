#include "taskpanel.h"
#include "globals.h"
#include "macros.h"
#include "manager.h"
#include "search_thread.h"
#include "wx/log.h"

TaskPanel::TaskPanel( wxWindow* parent )
		:
		TaskBasePanel( parent )
{
	// Initialize the list headers
	m_listCtrlTasks->InsertColumn(0, wxT("Type"));
	m_listCtrlTasks->InsertColumn(1, wxT("Comment"));
	m_listCtrlTasks->InsertColumn(2, wxT("Line"));
	m_listCtrlTasks->InsertColumn(3, wxT("File"));
	
	// set initial size
	m_listCtrlTasks->SetColumnWidth(0, 100);
	m_listCtrlTasks->SetColumnWidth(1, 100);
	m_listCtrlTasks->SetColumnWidth(2, 100);
	m_listCtrlTasks->SetColumnWidth(3, 100);
	
	m_buttonStop->Disable();
	Connect(wxEVT_SEARCH_THREAD_MATCHFOUND, wxCommandEventHandler(TaskPanel::OnSearchThread));
	Connect(wxEVT_SEARCH_THREAD_SEARCHCANCELED, wxCommandEventHandler(TaskPanel::OnSearchThread));
	Connect(wxEVT_SEARCH_THREAD_SEARCHEND, wxCommandEventHandler(TaskPanel::OnSearchThread));
	Connect(wxEVT_SEARCH_THREAD_SEARCHSTARTED, wxCommandEventHandler(TaskPanel::OnSearchThread));
}

void TaskPanel::OnSearch( wxCommandEvent& event )
{
	wxUnusedVar(event);

	/* Use the following regex:
	//( *)((TODO)|(ATTN)|(FIXME)|(BUG)):
	*/
	SearchData data;
	data.SetExtensions(wxT("*.*")); // use all files
	data.SetRootDir(m_choiceScope->GetStringSelection());
	data.SetRegularExpression(true);
	data.SetMatchCase(false);
	data.SetMatchWholeWord(false);

	// prepare a list of files to search
	if (m_choiceScope->GetStringSelection() == SEARCH_IN_WORKSPACE) {

		wxArrayString files;
		ManagerST::Get()->GetWorkspaceFiles(files);
		data.SetFiles(files);

	} else if (m_choiceScope->GetStringSelection() == SEARCH_IN_PROJECT) {

		wxArrayString files;
		ManagerST::Get()->GetProjectFiles(ManagerST::Get()->GetActiveProjectName(), files);
		data.SetFiles(files);
	}

	// always scan for all, the filter be done in the display part
	data.SetFindString(wxT("//( *)((TODO)|(ATTN)|(FIXME)|(BUG)):"));
	data.SetOwner(this);
	SearchThreadST::Get()->PerformSearch(data);
	
	// disable the search button
	m_buttonSearch->Disable();
	m_buttonStop->Enable();
}

void TaskPanel::OnItemActivated( wxListEvent& event )
{
	// open the selected item in the editor
	// type, comment, line, file
	long line(wxNOT_FOUND);
	wxString fileName = GetColumnText(m_listCtrlTasks, event.m_itemIndex, 3); 
	wxString strLine = GetColumnText(m_listCtrlTasks, event.m_itemIndex, 2); 
	
	strLine.ToLong(&line);
	ManagerST::Get()->OpenFile(fileName, wxEmptyString, line - 1);
}

void TaskPanel::OnSearchThread(wxCommandEvent& e)
{
	if (e.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHCANCELED || e.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHEND) {
		
		// re-enable the search button
		m_buttonSearch->Enable();
		m_buttonStop->Disable();
		
		DoDisplayResults();
		
	} else if (e.GetEventType() == wxEVT_SEARCH_THREAD_MATCHFOUND) {
		
		//add an entry to the replace panel
		SearchResultList *res = (SearchResultList*)e.GetClientData();
		SearchResultList::iterator iter = res->begin();
		for(; iter != res->end(); iter++){
			m_results.push_back(*iter);
		}
		delete res;
		
	} else if (e.GetEventType() == wxEVT_SEARCH_THREAD_SEARCHSTARTED) {
		DoClearResults();
	}
}

void TaskPanel::DoClearResults()
{
	m_results.clear();
	m_listCtrlTasks->DeleteAllItems();
	
	// set initial size
	m_listCtrlTasks->SetColumnWidth(0, 100);
	m_listCtrlTasks->SetColumnWidth(1, 100);
	m_listCtrlTasks->SetColumnWidth(2, 100);
	m_listCtrlTasks->SetColumnWidth(3, 100);
}

void TaskPanel::DoDisplayResults()
{
	m_listCtrlTasks->Freeze();
	m_listCtrlTasks->DeleteAllItems();
	SearchResultList::iterator iter = m_results.begin();
	for(; iter != m_results.end(); iter++){
		SearchResult res = *iter;
		
		// extract the 'Kind'
		int where(wxNOT_FOUND);
		wxString type;
		where = res.GetPattern().Find(wxT("//"));
		if(where != wxNOT_FOUND){
			type = res.GetPattern().Mid((size_t)where+2);
			type = type.BeforeFirst(wxT(':'));
			type.Trim().Trim(false);
		}
		
		// Filter out non matching entries
		if(m_choiceFilter->GetStringSelection() != wxT("All") && type.CmpNoCase(m_choiceFilter->GetStringSelection()) != 0){
			continue;
		}
		
		// append new line to the list control
		long index = AppendListCtrlRow(m_listCtrlTasks);		
		
		// set the line number
		wxString strLine;
		strLine << res.GetLineNumber();
		
		wxFileName fn(res.GetFileName());
		
		wxString pattern(res.GetPattern().AfterFirst(wxT(':')));
		pattern.Trim().Trim(false);
		
		// type, comment, line, file
		SetColumnText(m_listCtrlTasks, index, 0, type);
		SetColumnText(m_listCtrlTasks, index, 1, pattern);
		SetColumnText(m_listCtrlTasks, index, 2, strLine);
		SetColumnText(m_listCtrlTasks, index, 3, fn.GetFullPath());
	}
	
	// adjust the columns width
	m_listCtrlTasks->SetColumnWidth(0, 100);
	m_listCtrlTasks->SetColumnWidth(1, wxLIST_AUTOSIZE);
	m_listCtrlTasks->SetColumnWidth(2, 50);
	m_listCtrlTasks->SetColumnWidth(3, wxLIST_AUTOSIZE);
	
	m_listCtrlTasks->Thaw();
}

void TaskPanel::OnStop(wxCommandEvent& e)
{
	wxUnusedVar(e);
	SearchThreadST::Get()->StopSearch();
}

void TaskPanel::OnFilter(wxCommandEvent& e)
{
	// refresh the view
	wxUnusedVar(e);
	DoDisplayResults();
}

void TaskPanel::OnClear(wxCommandEvent& e)
{
	wxUnusedVar(e);
	DoClearResults();
}

void TaskPanel::OnClearUI(wxUpdateUIEvent& e)
{
	e.Enable(m_listCtrlTasks->GetItemCount() > 0);
}
