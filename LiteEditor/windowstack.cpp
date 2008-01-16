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
