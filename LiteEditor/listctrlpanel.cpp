//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : listctrlpanel.cpp              
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
 #include "listctrlpanel.h"
#include "manager.h"
#include "globals.h"

ListCtrlPanel::ListCtrlPanel ( wxWindow* parent )
		: ListCtrlPanelBase ( parent )
{
	m_listCtrl->InsertColumn ( 0, wxT("Level")		);
	m_listCtrl->InsertColumn ( 1, wxT("Address")	);
	m_listCtrl->InsertColumn ( 2, wxT("Function")	);
	m_listCtrl->InsertColumn ( 3, wxT("File")		);
	m_listCtrl->InsertColumn ( 4, wxT("Line")		);
}

void ListCtrlPanel::OnItemActivated ( wxListEvent& event )
{
	long frame, frameLine;
	wxString frameNumber = GetColumnText ( event.m_itemIndex, 0 );
	wxString frameLineStr   = GetColumnText ( event.m_itemIndex, 4 );
	frameNumber.ToLong(&frame);
	frameLineStr.ToLong(&frameLine);
	ManagerST::Get()->DbgSetFrame(frame, frameLine);
}

void ListCtrlPanel::Update ( const StackEntryArray &stackArr )
{
	m_listCtrl->Freeze();
	Clear();
	if (!stackArr.empty()) {
		for (int i=(int)stackArr.size()-1; i>=0; i--) {
			long item = AppendListCtrlRow(m_listCtrl);

			StackEntry entry = stackArr.at(i);
			SetColumnText(item, 0, entry.level);
			SetColumnText(item, 1, entry.address);
			SetColumnText(item, 2, entry.function);
			SetColumnText(item, 3, entry.file);
			SetColumnText(item, 4, entry.line);
		}
		m_listCtrl->SetColumnWidth(1, wxLIST_AUTOSIZE);
		m_listCtrl->SetColumnWidth(2, wxLIST_AUTOSIZE);
		m_listCtrl->SetColumnWidth(3, wxLIST_AUTOSIZE);
	}
	m_listCtrl->Thaw();
}

void ListCtrlPanel::Clear()
{
	m_listCtrl->DeleteAllItems();
}

void ListCtrlPanel::SetColumnText ( long indx, long column, const wxString &rText )
{
	m_listCtrl->Freeze();
	wxListItem list_item;
	list_item.SetId ( indx );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	list_item.SetText ( rText );
	m_listCtrl->SetItem ( list_item );
	m_listCtrl->Thaw();
}

wxString ListCtrlPanel::GetColumnText(long index, long column)
{
	wxListItem list_item;
	list_item.SetId ( index );
	list_item.SetColumn ( column );
	list_item.SetMask ( wxLIST_MASK_TEXT );
	m_listCtrl->GetItem ( list_item );
	return list_item.GetText();
}
