//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : externaltooldlg.cpp
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

#include "externaltooldlg.h"
#include "externaltoolsdata.h"
#include "globals.h"
#include "newtooldlg.h"
#include <wx/clntdata.h>
#include <wx/msgdlg.h>

ExternalToolDlg::ExternalToolDlg(wxWindow* parent, IManager* mgr)
    : ExternalToolBaseDlg(parent)
    , m_mgr(mgr)
{
    GetSizer()->Fit(this);
    CentreOnParent();
}

ExternalToolDlg::~ExternalToolDlg() {}

void ExternalToolDlg::OnItemActivated(wxDataViewEvent& event) { DoEditEntry(event.GetItem()); }

void ExternalToolDlg::OnButtonNew(wxCommandEvent& event)
{
    NewToolDlg dlg(this, m_mgr, NULL);
    if(dlg.ShowModal() == wxID_OK) {
        DoUpdateEntry(wxDataViewItem(nullptr), dlg.GetToolId(), dlg.GetToolName(), dlg.GetPath(),
                      dlg.GetWorkingDirectory(), dlg.GetIcon16(), dlg.GetIcon24(), dlg.GetCaptureOutput(),
                      dlg.GetSaveAllFiles(), dlg.IsCallOnFileSave());
    }
}

void ExternalToolDlg::OnButtonNewUI(wxUpdateUIEvent& event)
{
    // maximum of 10 items
    event.Enable(m_dvListCtrlTools->GetItemCount() < MAX_TOOLS);
}

void ExternalToolDlg::OnButtonEdit(wxCommandEvent& event) { DoEditEntry(m_dvListCtrlTools->GetSelection()); }

void ExternalToolDlg::OnButtonEditUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlTools->GetSelectedItemsCount());
}

void ExternalToolDlg::OnButtonDelete(wxCommandEvent& event)
{
    wxDataViewItem item = m_dvListCtrlTools->GetSelection();
    CHECK_ITEM_RET(item);
    if(wxMessageBox(_("Are you sure you want to delete this tool?"), _("CodeLite"), wxYES_NO | wxCANCEL) == wxYES) {
        m_dvListCtrlTools->DeleteItem(m_dvListCtrlTools->ItemToRow(item));
    }
}

void ExternalToolDlg::OnButtonDeleteUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrlTools->GetSelection().IsOk());
}

void ExternalToolDlg::DoUpdateEntry(const wxDataViewItem& item, const wxString& id, const wxString& name,
                                    const wxString& path, const wxString& workingDirectory, const wxString& icon16,
                                    const wxString& icon24, bool captureOutput, bool saveAllFiles, bool callOnFileSave)
{
    if(item.IsOk()) { DoDeleteItem(item); }

    ExternalToolData* data = new ExternalToolData(id, name, path, workingDirectory, icon16, icon24, captureOutput,
                                                  saveAllFiles, callOnFileSave);
    wxDataViewItem child = m_dvListCtrlTools->AppendItem(id, wxNOT_FOUND, wxNOT_FOUND, (wxUIntPtr)data);
    m_dvListCtrlTools->SetItemText(child, name, 1);
    m_dvListCtrlTools->SetItemText(child, path, 2);
}

void ExternalToolDlg::DoEditEntry(const wxDataViewItem& item)
{
    ExternalToolData* data = GetToolData(item);
    CHECK_PTR_RET(data);
    NewToolDlg dlg(this, m_mgr, data);
    if(dlg.ShowModal() == wxID_OK) {
        DoUpdateEntry(item, dlg.GetToolId(), dlg.GetToolName(), dlg.GetPath(), dlg.GetWorkingDirectory(),
                      dlg.GetIcon16(), dlg.GetIcon24(), dlg.GetCaptureOutput(), dlg.GetSaveAllFiles(),
                      dlg.IsCallOnFileSave());
    }
}

std::vector<ToolInfo> ExternalToolDlg::GetTools()
{
    std::vector<ToolInfo> tools;
    for(size_t i = 0; i < m_dvListCtrlTools->GetItemCount(); ++i) {

        ToolInfo ti;
        ExternalToolData* data = GetToolData(m_dvListCtrlTools->RowToItem(i));

        if(data) {
            ti.SetId(data->m_id);
            ti.SetName(data->m_name);
            ti.SetPath(data->m_path);
            ti.SetWd(data->m_workingDirectory);
            ti.SetIcon16(data->m_icon16);
            ti.SetIcon24(data->m_icon24);
            ti.SetCaptureOutput(data->m_captureOutput);
            ti.SetSaveAllFiles(data->m_saveAllFiles);
            ti.SetCallOnFileSave(data->m_callOnFileSave);
            tools.push_back(ti);
        }
    }
    return tools;
}

void ExternalToolDlg::SetTools(const std::vector<ToolInfo>& tools)
{
    DoClear();
    for(size_t i = 0; i < tools.size(); i++) {
        const ToolInfo& ti = tools[i];
        ExternalToolData* data = new ExternalToolData(ti);
        wxDataViewItem item = m_dvListCtrlTools->AppendItem(ti.GetId(), wxNOT_FOUND, wxNOT_FOUND, (wxUIntPtr)data);
        m_dvListCtrlTools->SetItemText(item, ti.GetName(), 1);
        m_dvListCtrlTools->SetItemText(item, ti.GetPath(), 2);
    }
}

ExternalToolData* ExternalToolDlg::GetToolData(const wxDataViewItem& item)
{
    CHECK_ITEM_RET_NULL(item);
    return reinterpret_cast<ExternalToolData*>(m_dvListCtrlTools->GetItemData(item));
}

void ExternalToolDlg::DoClear()
{
    m_dvListCtrlTools->DeleteAllItems([](wxUIntPtr data) { delete reinterpret_cast<ExternalToolData*>(data); });
}

void ExternalToolDlg::DoDeleteItem(const wxDataViewItem& item)
{
    if(!item.IsOk()) { return; }
    ExternalToolData* d = GetToolData(item);
    wxDELETE(d);
    m_dvListCtrlTools->DeleteItem(m_dvListCtrlTools->ItemToRow(item));
}
