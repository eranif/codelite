#include <wx/control.h>
#include <wx/aui/aui.h>
#include <wx/app.h>
#include <wx/xrc/xmlres.h>
#include "dockablepanemenumanager.h"
#include <wx/log.h>

//const wxEventType wxEVT_CMD_HIDE_PANE = wxNewEventType();
//const wxEventType wxEVT_CMD_SHOW_PANE = wxNewEventType();

DockablePaneMenuManager::DockablePaneMenuManager(wxMenuBar* mb, wxAuiManager *aui)
		: m_mb(mb)
		, m_aui(aui)
{
}

DockablePaneMenuManager::~DockablePaneMenuManager()
{
}

void DockablePaneMenuManager::RemoveMenu(const wxString& name)
{
	int idx = m_mb->FindMenu(wxT("View"));
	if (idx != wxNOT_FOUND) {
		wxMenu *menu = m_mb->GetMenu(idx);
		int itemId = wxXmlResource::GetXRCID(name.c_str());
		menu->Destroy(itemId);

		std::map<int, wxString>::iterator iter = m_id2nameMap.find(itemId);
		if (iter != m_id2nameMap.end()) {
			m_id2nameMap.erase(iter);
		}
		wxTheApp->Disconnect(itemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DockablePaneMenuManager::OnDockpaneMenuItem), NULL, this);
		wxTheApp->Disconnect(itemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DockablePaneMenuManager::OnDockpaneMenuItemUI), NULL, this);
	}
}

void DockablePaneMenuManager::AddMenu(const wxString& name)
{
	int idx = m_mb->FindMenu(wxT("View"));
	if (idx != wxNOT_FOUND) {
		wxMenu *menu = m_mb->GetMenu(idx);

		int itemId = wxXmlResource::GetXRCID(name.c_str());
		wxMenuItem *item = new wxMenuItem(menu, itemId, name, wxEmptyString, wxITEM_CHECK);
		menu->Append(item);
		item->Check(true);
		m_id2nameMap[itemId] = name;

		wxTheApp->Connect(itemId, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DockablePaneMenuManager::OnDockpaneMenuItem), NULL, this);
		wxTheApp->Connect(itemId, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DockablePaneMenuManager::OnDockpaneMenuItemUI), NULL, this);
	}
}

wxString DockablePaneMenuManager::NameById(int id)
{
	std::map<int, wxString>::iterator iter = m_id2nameMap.find(id);
	if (iter != m_id2nameMap.end()) {
		return iter->second;
	}
	return wxEmptyString;
}

void DockablePaneMenuManager::OnDockpaneMenuItem(wxCommandEvent& e)
{
	wxString name = NameById(e.GetId());
	wxAuiPaneInfo &info = m_aui->GetPane(name);
	if ( info.IsOk() ) {
		if (e.IsChecked()) {
			// need to show the item
			info.Show();
		} else {
			info.Hide();
		}
		m_aui->Update();
	}
}

void DockablePaneMenuManager::OnDockpaneMenuItemUI(wxUpdateUIEvent& e)
{
	wxString name = NameById(e.GetId());
	wxAuiPaneInfo &info = m_aui->GetPane(name);
	if (info.IsOk()) {
		if ( info.IsShown() ) {
			e.Check(true);
		} else {
			e.Check(false);
		}
	}
}

wxArrayString DockablePaneMenuManager::GetDeatchedPanesList()
{
	wxArrayString arr;
	std::map<int, wxString>::iterator iter = m_id2nameMap.begin();
	for(; iter != m_id2nameMap.end(); iter++){
		arr.Add(iter->second);
	}
	return arr;
}

bool DockablePaneMenuManager::IsPaneDetached(const wxString& name)
{
	std::map<int, wxString>::iterator iter = m_id2nameMap.begin();
	for(; iter != m_id2nameMap.end(); iter++){
		if(iter->second == name) {
			return true;
		}
	}
	return false;
}
