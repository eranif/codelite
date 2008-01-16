#include "threadlistpanel.h"
#include "globals.h"
#include "manager.h"

ThreadListPanel::ThreadListPanel( wxWindow* parent )
:
ThreadListBasePanel( parent )
{
	InitList();
}

void ThreadListPanel::OnItemActivated( wxListEvent& event )
{
	long threadId(wxNOT_FOUND);
	int index = event.m_itemIndex;
	if(index != wxNOT_FOUND){
		wxString str_id = GetColumnText(m_list, index, 0);
		str_id.ToLong(&threadId);
		Manager *mgr = ManagerST::Get();
		mgr->DbgSetThread(threadId);
	}
}

void ThreadListPanel::InitList()
{
		//add two columns to the list ctrl
	m_list->InsertColumn(0, wxT("Thread ID"));
	m_list->InsertColumn(1, wxT("Active"));
	m_list->InsertColumn(2, wxT("Information"));
}

void ThreadListPanel::PopulateList(const ThreadEntryArray &threads)
{
	m_list->Freeze();
	m_list->DeleteAllItems();
	for(ThreadEntryArray::size_type i=0; i< threads.size(); i++){
		ThreadEntry entry = threads.at(i);

		long item;
		wxListItem info;

		//insert new item (row)
		info.SetColumn(0);
		item = m_list->InsertItem(info);

		wxString str_id;
		wxString str_active;
		
		str_id << entry.dbgid;
		str_active =  entry.active ? wxT("Yes") : wxT("No");
		
		SetColumnText(m_list, item, 0, str_id);
		SetColumnText(m_list, item, 1, str_active);
		SetColumnText(m_list, item, 2, entry.more);
	}
	m_list->SetColumnWidth(2, wxLIST_AUTOSIZE);
	m_list->Thaw();
}

void ThreadListPanel::Clear()
{
	m_list->DeleteAllItems();
}
