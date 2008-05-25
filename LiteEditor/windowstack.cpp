//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : windowstack.cpp              
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
 #include "windowstack.h"

WindowStack::WindowStack(wxWindow *parent, wxWindowID id)
: wxPanel(parent, id)
, m_selection(NULL)
{
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(m_mainSizer);
	m_windows.clear();
}

WindowStack::~WindowStack()
{
	Clear();
}

void WindowStack::Add(wxWindow *win, const wxString &key)
{
	if(!win || key.IsEmpty()){
		return;
	}

	//add the new item to the map
	m_windows[key] = win;
	win->Hide();
}

void WindowStack::Select(const wxString &key)
{
	//find the window
	std::map<wxString, wxWindow*>::iterator iter = m_windows.find(key);
	if(iter == m_windows.end()){
		return;
	}

	wxWindow *win = iter->second;
	if(!win){
		m_windows.erase(iter);
		return;
	}
	
	Freeze();
	//remove the old selection
	if(m_selection){
		m_mainSizer->Detach(m_selection);
		m_selection->Hide();
	}

	m_mainSizer->Add(win, 1, wxEXPAND);
	win->Show();
	m_selection = win;
	m_mainSizer->Layout();
	Thaw();
}

void WindowStack::Clear()
{
	if(m_selection){
		m_mainSizer->Detach(m_selection);
		m_selection->Hide();
	}
	
	m_mainSizer->Layout();
	m_selection = NULL;
	m_windows.clear();
}

void WindowStack::Delete(const wxString &key)
{
	std::map<wxString, wxWindow*>::iterator iter = m_windows.find(key);
	if(iter == m_windows.end()){
		return;
	}

	wxWindow *win = iter->second;
	if(!win){
		return;
	}
	
	Freeze();
	m_mainSizer->Detach(win);
	m_mainSizer->Layout();
	win->Hide();
	//if the removed page was also the selection, unselect it
	if(win == m_selection){
		m_selection = NULL;
	}
	m_windows.erase(iter);
	win->Destroy();
	Thaw();
}

wxWindow *WindowStack::Find(const wxString &key)
{
	std::map<wxString, wxWindow*>::iterator iter = m_windows.find(key);
	if(iter == m_windows.end()){
		return NULL;
	}
	return iter->second;
}
