//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : ps_debugger_page.cpp
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

#include "ps_debugger_page.h"

#include "globals.h"

#include <project.h>
#include <workspace.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

PSDebuggerPage::PSDebuggerPage(wxWindow* parent, ProjectSettingsDlg* dlg)
    : PSDebuggerPageBase(parent)
    , m_dlg(dlg)
{
}

void PSDebuggerPage::OnCmdEvtVModified(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_dlg->SetIsDirty(true);
}

void PSDebuggerPage::OnStcEvtVModified(wxStyledTextEvent& event)
{
    event.Skip();
    m_dlg->SetIsDirty(true);
}

void PSDebuggerPage::OnRemoteDebugUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxDbgRemote->IsChecked()); }

void PSDebuggerPage::Load(BuildConfigPtr buildConf)
{
    Clear();
    m_textCtrlDbgCmds->ChangeValue(buildConf->GetDebuggerStartupCmds());
    m_textCtrlDbgPostConnectCmds->ChangeValue(buildConf->GetDebuggerPostRemoteConnectCmds());
    m_checkBoxDbgRemote->SetValue(buildConf->GetIsDbgRemoteTarget());
    m_textCtrl1DbgHost->ChangeValue(buildConf->GetDbgHostName());
    m_textCtrlDbgPort->ChangeValue(buildConf->GetDbgHostPort());
    m_textCtrlDebuggerPath->ChangeValue(buildConf->GetDebuggerPath());
    m_checkBoxDbgRemoteExt->SetValue(buildConf->GetIsDbgRemoteExtended());

    const wxArrayString& searchPaths = buildConf->GetDebuggerSearchPaths();
    for(size_t i = 0; i < searchPaths.GetCount(); ++i) {
        wxVector<wxVariant> cols;
        cols.push_back(searchPaths.Item(i));
        m_dvListCtrlDebuggerSearchPaths->AppendItem(cols, (wxUIntPtr)NULL);
    }
}

void PSDebuggerPage::Save(BuildConfigPtr buildConf, ProjectSettingsPtr projSettingsPtr)
{
    buildConf->SetDebuggerStartupCmds(m_textCtrlDbgCmds->GetValue());
    buildConf->SetDebuggerPostRemoteConnectCmds(m_textCtrlDbgPostConnectCmds->GetValue());
    buildConf->SetIsDbgRemoteTarget(m_checkBoxDbgRemote->IsChecked());
    buildConf->SetDbgHostName(m_textCtrl1DbgHost->GetValue());
    buildConf->SetDbgHostPort(m_textCtrlDbgPort->GetValue());
    buildConf->SetDebuggerPath(m_textCtrlDebuggerPath->GetValue());
    buildConf->SetIsDbgRemoteExtended(m_checkBoxDbgRemoteExt->IsChecked());

    wxArrayString searchPaths;
    int nCount = m_dvListCtrlDebuggerSearchPaths->GetItemCount();
    for(int i = 0; i < nCount; ++i) {
        wxVariant colValue;
        m_dvListCtrlDebuggerSearchPaths->GetValue(colValue, i, 0);
        if(!colValue.IsNull()) {
            searchPaths.Add(colValue.GetString());
        }
    }
    buildConf->SetDebuggerSearchPaths(searchPaths);
}

void PSDebuggerPage::Clear()
{
    m_textCtrlDebuggerPath->Clear();
    m_textCtrl1DbgHost->Clear();
    m_textCtrlDbgCmds->Clear();
    m_textCtrlDbgPort->Clear();
    m_textCtrlDbgPostConnectCmds->Clear();
    m_dvListCtrlDebuggerSearchPaths->DeleteAllItems();
    m_checkBoxDbgRemote->SetValue(false);
}

void PSDebuggerPage::OnBrowseForDebuggerPath(wxCommandEvent& event)
{
    wxString debugger_path = ::wxFileSelector(_("Select debugger:"));

    if(!debugger_path.IsEmpty()) {
        wxString errMsg;
        ProjectPtr proj = clCxxWorkspaceST::Get()->FindProjectByName(m_dlg->GetProjectName(), errMsg);
        if(proj) {
            wxFileName fnDebuggerPath(debugger_path);
            wxString project_path = proj->GetFileName().GetPath();
            if(fnDebuggerPath.MakeRelativeTo(project_path)) {
                debugger_path = fnDebuggerPath.GetFullPath();
            }
        }
        m_textCtrlDebuggerPath->ChangeValue(debugger_path);
        m_dlg->SetIsDirty(true);
    }
}
void PSDebuggerPage::OnAddDebuggerSearchPath(wxCommandEvent& event)
{
    wxString path = ::wxDirSelector();
    if(!path.IsEmpty()) {
        wxVector<wxVariant> cols;
        cols.push_back(path);
        m_dvListCtrlDebuggerSearchPaths->AppendItem(cols, (wxUIntPtr)NULL);
        m_dlg->SetIsDirty(true);
    }
}

void PSDebuggerPage::OnDeleteDebuggerSearchPath(wxCommandEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrlDebuggerSearchPaths->GetSelections(items);
    if(items.IsEmpty())
        return;

    for(size_t i = 0; i < items.GetCount(); ++i) {
        m_dvListCtrlDebuggerSearchPaths->DeleteItem(m_dvListCtrlDebuggerSearchPaths->ItemToRow(items.Item(i)));
    }
    m_dlg->SetIsDirty(true);
}

void PSDebuggerPage::OnDeleteDebuggerSearchPathUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlDebuggerSearchPaths->GetSelectedItemsCount());
}

void PSDebuggerPage::OnItemActivated(wxDataViewEvent& event)
{
    wxVariant value;
    m_dvListCtrlDebuggerSearchPaths->GetValue(value, m_dvListCtrlDebuggerSearchPaths->ItemToRow(event.GetItem()), 0);

    if(!value.IsNull()) {

        wxString path = value.GetString();
        path = ::wxDirSelector(_("Select a folder"), path);

        if(!path.IsEmpty()) {
            m_dvListCtrlDebuggerSearchPaths->DeleteItem(m_dvListCtrlDebuggerSearchPaths->ItemToRow(event.GetItem()));
            CallAfter(&PSDebuggerPage::DoAddPath, path);
        }
    }
}

void PSDebuggerPage::DoAddPath(const wxString& path)
{
    wxVector<wxVariant> cols;
    cols.push_back(path);
    m_dvListCtrlDebuggerSearchPaths->AppendItem(cols, (wxUIntPtr)NULL);
    m_dlg->SetIsDirty(true);
}

void PSDebuggerPage::OnProjectEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_dlg->IsProjectEnabled()); }
