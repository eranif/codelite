//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : depend_dlg_page.cpp
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

#include "manager.h"
#include "project.h"
#include "depend_dlg_page.h"

DependenciesPage::DependenciesPage(wxWindow* parent, const wxString& projectName)
    : DependenciesPageBase(parent)
    , m_projectName(projectName)
    , m_dirty(false)
{
    Init();
}

void DependenciesPage::OnConfigChanged(wxCommandEvent& event)
{
    if(m_dirty) {
        // save old configuration
        if(wxMessageBox(
               wxString::Format(_("Build order for configuration '%s' has been modified, would you like to save it?"),
                                m_currentSelection.GetData()),
               _("CodeLite"), wxYES_NO | wxICON_QUESTION) == wxYES) {
            Save();
        }
        m_dirty = false;
    }

    m_currentSelection = event.GetString();
    // switch to new configuration
    DoPopulateControl(m_currentSelection);
}

void DependenciesPage::OnMoveUp(wxCommandEvent& event)
{
    wxUnusedVar(event);
    OnUpCommand(m_listBoxBuildOrder);
}

void DependenciesPage::OnMoveDown(wxCommandEvent& event)
{
    wxUnusedVar(event);
    OnDownCommand(m_listBoxBuildOrder);
}

void DependenciesPage::OnUpCommand(wxListBox* list)
{
    wxString selectedString = list->GetStringSelection();

    int sel = list->GetSelection();
    if(sel == wxNOT_FOUND) {
        return;
    }

    sel--;
    if(sel < 0) {
        return;
    }

    // sel contains the new position we want to place the selection string
    list->Delete(sel + 1);
    list->Insert(selectedString, sel);
    list->Select(sel);
    m_dirty = true;
}

void DependenciesPage::OnDownCommand(wxListBox* list)
{
    int sel = list->GetSelection();
    if(sel == wxNOT_FOUND) {
        return;
    }

    sel++;
    if(sel >= (int)list->GetCount()) {
        return;
    }

    // sel contains the new position we want to place the selection string
    wxString oldStr = list->GetString(sel);

    list->Delete(sel);
    list->Insert(oldStr, sel - 1);
    list->Select(sel);
    m_dirty = true;
}

void DependenciesPage::Save()
{
    // Save only if its dirty...
    if(m_dirty) {
        ProjectPtr proj = ManagerST::Get()->GetProject(m_projectName);

        wxArrayString depsArr;
        for(size_t i = 0; i < m_listBoxBuildOrder->GetCount(); i++) {
            depsArr.Add(m_listBoxBuildOrder->GetString((unsigned int)i));
        }

        if(m_currentSelection.IsEmpty()) {
            return;
        }

        proj->SetDependencies(depsArr, m_currentSelection);
    }
    m_dirty = false;
}

void DependenciesPage::OnCheckListItemToggled(wxCommandEvent& event)
{
    int item = event.GetSelection();
    wxString name = m_checkListProjectList->GetString((unsigned int)item);
    if(!m_checkListProjectList->IsChecked((unsigned int)item)) {
        unsigned int buildOrderId = m_listBoxBuildOrder->FindString(name);
        if(buildOrderId != (unsigned int)wxNOT_FOUND) {
            m_listBoxBuildOrder->Delete(buildOrderId);
        }
    } else {
        m_listBoxBuildOrder->Append(name);
    }
    m_dirty = true;
}

void DependenciesPage::Init()
{
    wxString errMsg;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(m_projectName, errMsg);
    if(proj) {

        // populate the choice control with the list of available configurations for this project
        ProjectSettingsPtr settings = proj->GetSettings();
        if(settings) {
            ProjectSettingsCookie cookie;
            BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
            while(bldConf) {
                m_choiceProjectConfig->Append(bldConf->GetName());
                bldConf = settings->GetNextBuildConfiguration(cookie);
            }
        }

        // by default select the first configuration
        if(m_choiceProjectConfig->GetCount() > 0) {
            m_choiceProjectConfig->SetSelection(0);
        }

        // select the active configuration
        BuildConfigPtr selBuildConf = clCxxWorkspaceST::Get()->GetProjBuildConf(m_projectName, wxEmptyString);
        if(selBuildConf) {
            int where = m_choiceProjectConfig->FindString(selBuildConf->GetName());
            if(where != wxNOT_FOUND) {
                m_choiceProjectConfig->SetSelection(where);
            }
        }

        m_currentSelection = m_choiceProjectConfig->GetStringSelection();
        DoPopulateControl(m_choiceProjectConfig->GetStringSelection());

    } else {
        wxMessageBox(errMsg, _("CodeLite"));
        return;
    }
}

void DependenciesPage::DoPopulateControl(const wxString& configuration)
{
    wxString errMsg;
    ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(m_projectName, errMsg);
    if(!proj) {
        return;
    }

    m_listBoxBuildOrder->Clear();
    m_checkListProjectList->Clear();

    // initialize the build order listbox
    wxArrayString depArr = proj->GetDependencies(configuration);
    size_t i = 0;
    for(i = 0; i < depArr.GetCount(); i++) {
        wxString item = depArr.Item(i);
        m_listBoxBuildOrder->Append(item);
    }

    // initialize the project dependencies check list
    wxArrayString projArr;
    ManagerST::Get()->GetProjectList(projArr);

    for(i = 0; i < projArr.GetCount(); i++) {

        if(projArr.Item(i) != m_projectName) {
            int idx = m_checkListProjectList->Append(projArr.Item(i));
            m_checkListProjectList->Check(idx, depArr.Index(projArr.Item(i)) != wxNOT_FOUND);
        }
    }
}

void DependenciesPage::OnApplyButton(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Save();
}

void DependenciesPage::OnApplyButtonUI(wxUpdateUIEvent& event) { event.Enable(m_dirty); }
