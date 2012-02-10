//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : threadlistpanel.cpp              
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
	m_list->InsertColumn(0, _("Thread ID"));
	m_list->InsertColumn(1, _("Active"));
	m_list->InsertColumn(2, _("Information"));
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
		info.SetId(0);
		item = m_list->InsertItem(info);

		wxString str_id;
		wxString str_active;
		
		str_id << entry.dbgid;
		str_active =  entry.active ? _("Yes") : _("No");
		
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
