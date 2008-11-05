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

void WindowStack::DoSelect(wxWindow *win, const wxString &key)
{
    Freeze();
    //remove the old selection
    if(m_selection){
        m_mainSizer->Detach(m_selection);
        m_selection->Hide();
    }
    if (win) {
        m_mainSizer->Add(win, 1, wxEXPAND);
        win->Show();
        m_selection = win;
        m_selectionKey = key;
    } else {
        m_selection = NULL;
        m_selectionKey.Clear();
    }
    m_mainSizer->Layout();
    Thaw();
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
    wxWindow *win = Find(key);
    if (win) {
        DoSelect(win, key);
    }
}

void WindowStack::Select(wxWindow* win)
{
    wxString key = Find(win);
    if (!key.IsEmpty()) {
        DoSelect(win, key);
    }
}

void WindowStack::SelectNone()
{
    DoSelect(NULL, wxEmptyString);
}
  
void WindowStack::Clear()
{
    SelectNone();
    m_selection = NULL;
    m_selectionKey.Clear();
    m_windows.clear();
}

wxWindow* WindowStack::Remove(const wxString &key)
{
    std::map<wxString, wxWindow*>::iterator iter = m_windows.find(key);
    if(iter == m_windows.end()){
	return NULL;
    }

    wxWindow *win = iter->second;
    if(!win){
        return NULL;
    }
	
    if (m_selection == win) {
        SelectNone();
    }
  
    m_windows.erase(iter);
     
    return win;
}

wxString WindowStack::Remove(wxWindow* win)
{
    wxString key = Find(win);
    if (!key.IsEmpty()) {
        Remove(key);
    }
    return key;
}

void WindowStack::Delete(const wxString &key) 
{
    wxWindow *win = Remove(key);
    if (win) { 
        win->Destroy(); 
    }
}

void WindowStack::Delete(wxWindow *win) 
{
    wxString key = Remove(win);
    if (!key.IsEmpty()) { 
        win->Destroy(); 
    }
}
    
wxWindow *WindowStack::Find(const wxString &key)
{
	std::map<wxString, wxWindow*>::iterator iter = m_windows.find(key);
	if(iter == m_windows.end()){
		return NULL;
	}
	return iter->second;
}

wxString WindowStack::Find(wxWindow *win) 
{
    for (std::map<wxString,wxWindow*>::iterator iter = m_windows.begin();
            iter != m_windows.end(); iter++) {
        if (iter->second == win)
            return iter->first;
    }
    return wxEmptyString;
}

void WindowStack::GetKeys(std::vector<wxString> &keys) const
{
    for (std::map<wxString,wxWindow*>::const_iterator iter = m_windows.begin();
            iter != m_windows.end(); iter++) {
        keys.push_back(iter->first);
    }
}
