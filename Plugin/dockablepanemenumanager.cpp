//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dockablepanemenumanager.cpp
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

#include "dockablepanemenumanager.h"

#include <wx/app.h>
#include <wx/aui/aui.h>
#include <wx/control.h>
#include <wx/log.h>
#include <wx/xrc/xmlres.h>

void DockablePaneMenuManager::HackShowPane(wxAuiPaneInfo& pane_info, wxAuiManager* pAui)
{
    if(pane_info.IsOk() && pAui) {
        pane_info.MinSize(pane_info.best_size); // saved while hiding
        pane_info.Show();
        pAui->Update();
        pane_info.MinSize(10, 5); // so it can't disappear if undocked
        pAui->Update();
    }
}

void DockablePaneMenuManager::HackHidePane(bool commit, wxAuiPaneInfo& pane_info, wxAuiManager* pAui)
{
    if(pane_info.IsOk() && pAui) {
        int width = 0;
        int height = 0;
        pane_info.window->GetClientSize(&width, &height);
        pane_info.BestSize(width, height); // save for later subsequent show
        pane_info.Hide();

        if(commit) {
            pAui->Update();
        }
    }
}

DockablePaneMenuManager::DockablePaneMenuManager(wxAuiManager* aui)
    : m_aui(aui)
{
}

DockablePaneMenuManager::~DockablePaneMenuManager() {}

void DockablePaneMenuManager::RemoveMenu(const wxString& name)
{
    int itemId = wxXmlResource::GetXRCID(name.c_str());
    std::map<int, wxString>::iterator iter = m_id2nameMap.find(itemId);
    if(iter != m_id2nameMap.end()) {
        m_id2nameMap.erase(iter);
    }
}

void DockablePaneMenuManager::AddMenu(const wxString& name)
{
    int itemId = wxXmlResource::GetXRCID(name.c_str());
    m_id2nameMap[itemId] = name;
}

wxString DockablePaneMenuManager::NameById(int id)
{
    std::map<int, wxString>::iterator iter = m_id2nameMap.find(id);
    if(iter != m_id2nameMap.end()) {
        return iter->second;
    }
    return wxEmptyString;
}

void DockablePaneMenuManager::OnDockpaneMenuItem(wxCommandEvent& e)
{
    wxString name = NameById(e.GetId());
    wxAuiPaneInfo& info = m_aui->GetPane(name);
    if(info.IsOk()) {
        if(e.IsChecked()) {
            HackShowPane(info, m_aui);
        } else {
            HackHidePane(true, info, m_aui);
        }
    }
}

void DockablePaneMenuManager::OnDockpaneMenuItemUI(wxUpdateUIEvent& e)
{
    wxString name = NameById(e.GetId());
    wxAuiPaneInfo& info = m_aui->GetPane(name);
    if(info.IsOk()) {
        if(info.IsShown()) {
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
    for(; iter != m_id2nameMap.end(); iter++) {
        arr.Add(iter->second);
    }
    return arr;
}

bool DockablePaneMenuManager::IsPaneDetached(const wxString& name)
{
    std::map<int, wxString>::iterator iter = m_id2nameMap.begin();
    for(; iter != m_id2nameMap.end(); iter++) {
        if(iter->second == name) {
            return true;
        }
    }
    return false;
}
