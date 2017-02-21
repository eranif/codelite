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

#include "globals.h"
#include <wx/clntdata.h>
#include <wx/msgdlg.h>
#include "externaltoolsdata.h"
#include "newtooldlg.h"
#include "externaltooldlg.h"

ExternalToolDlg::ExternalToolDlg(wxWindow* parent, IManager* mgr)
    : ExternalToolBaseDlg(parent)
    , m_item(wxNOT_FOUND)
    , m_mgr(mgr)
{
    Initialize();
    m_listCtrlTools->SetFocus();
    GetSizer()->Fit(this);
    CentreOnParent();
}

ExternalToolDlg::~ExternalToolDlg() {}

void ExternalToolDlg::OnItemActivated(wxListEvent& event)
{
    m_item = event.m_itemIndex;
    DoEditEntry(event.m_itemIndex);
}

void ExternalToolDlg::OnItemDeSelected(wxListEvent& event)
{
    wxUnusedVar(event);
    m_item = wxNOT_FOUND;
}

void ExternalToolDlg::OnItemSelected(wxListEvent& event) { m_item = event.m_itemIndex; }

void ExternalToolDlg::OnButtonNew(wxCommandEvent& event)
{
    NewToolDlg dlg(this, m_mgr, NULL);
    if(dlg.ShowModal() == wxID_OK) {
        DoUpdateEntry(dlg.GetToolId(),
                      dlg.GetToolName(),
                      dlg.GetPath(),
                      dlg.GetWorkingDirectory(),
                      dlg.GetArguments(),
                      dlg.GetIcon16(),
                      dlg.GetIcon24(),
                      dlg.GetCaptureOutput(),
                      dlg.GetSaveAllFiles(),
                      dlg.IsCallOnFileSave());
    }
}

void ExternalToolDlg::OnButtonNewUI(wxUpdateUIEvent& event)
{
    // maximum of 10 items
    event.Enable(m_listCtrlTools->GetItemCount() < MAX_TOOLS);
}

void ExternalToolDlg::OnButtonEdit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoEditEntry(m_item);
}

void ExternalToolDlg::OnButtonEditUI(wxUpdateUIEvent& event) { event.Enable(m_item != wxNOT_FOUND); }

void ExternalToolDlg::OnButtonDelete(wxCommandEvent& event)
{
    if(wxMessageBox(_("Are you sure you want to delete this tool?"), _("CodeLite"), wxYES_NO | wxCANCEL) == wxYES) {
        m_listCtrlTools->DeleteItem(m_item);
    }
}

void ExternalToolDlg::OnButtonDeleteUI(wxUpdateUIEvent& event) { event.Enable(m_item != wxNOT_FOUND); }

void ExternalToolDlg::Initialize()
{
    m_listCtrlTools->InsertColumn(0, _("ID"));
    m_listCtrlTools->InsertColumn(1, _("Name"));
    m_listCtrlTools->InsertColumn(2, _("Path"));

    m_listCtrlTools->SetColumnWidth(0, 200);
    m_listCtrlTools->SetColumnWidth(1, 200);
    m_listCtrlTools->SetColumnWidth(2, 200);
}

void ExternalToolDlg::DoUpdateEntry(const wxString& id,
                                    const wxString& name,
                                    const wxString& path,
                                    const wxString& workingDirectory,
                                    const wxString& arguments,
                                    const wxString& icon16,
                                    const wxString& icon24,
                                    bool captureOutput,
                                    bool saveAllFiles,
                                    bool callOnFileSave)
{
    // try to see if 'id' already exist in the list control
    long item(wxNOT_FOUND);
    for(size_t i = 0; i < (size_t)m_listCtrlTools->GetItemCount(); i++) {
        if(GetColumnText(m_listCtrlTools, i, 0) == id) {
            item = i;

            // Delete old data
            ExternalToolData* data = (ExternalToolData*)m_listCtrlTools->GetItemData(item);
            if(data) delete data;

            break;
        }
    }

    // append new row
    if(item == wxNOT_FOUND) {
        item = AppendListCtrlRow(m_listCtrlTools);
    }

    SetColumnText(m_listCtrlTools, item, 0, id);
    SetColumnText(m_listCtrlTools, item, 1, name);
    SetColumnText(m_listCtrlTools, item, 2, path);

    ExternalToolData* data = new ExternalToolData(
        id, name, path, workingDirectory, arguments, icon16, icon24, captureOutput, saveAllFiles, callOnFileSave);
    m_listCtrlTools->SetItemPtrData(item, wxUIntPtr(data));
}

void ExternalToolDlg::DoEditEntry(long item)
{
    ExternalToolData* data = (ExternalToolData*)m_listCtrlTools->GetItemData(item);
    NewToolDlg dlg(this, m_mgr, data);
    if(dlg.ShowModal() == wxID_OK) {
        DoUpdateEntry(dlg.GetToolId(),
                      dlg.GetToolName(),
                      dlg.GetPath(),
                      dlg.GetWorkingDirectory(),
                      dlg.GetArguments(),
                      dlg.GetIcon16(),
                      dlg.GetIcon24(),
                      dlg.GetCaptureOutput(),
                      dlg.GetSaveAllFiles(),
                      dlg.IsCallOnFileSave());
    }
}

std::vector<ToolInfo> ExternalToolDlg::GetTools()
{
    std::vector<ToolInfo> tools;
    for(size_t i = 0; i < (size_t)m_listCtrlTools->GetItemCount(); i++) {

        ToolInfo ti;
        ExternalToolData* data = (ExternalToolData*)m_listCtrlTools->GetItemData(i);

        if(data) {
            ti.SetId(data->m_id);
            ti.SetName(data->m_name);
            ti.SetPath(data->m_path);
            ti.SetArguments(data->m_args);
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
    m_listCtrlTools->Freeze();
    for(size_t i = 0; i < (size_t)m_listCtrlTools->GetItemCount(); i++) {
        ExternalToolData* data = (ExternalToolData*)m_listCtrlTools->GetItemData(i);
        wxDELETE(data);
    }
    
    m_listCtrlTools->DeleteAllItems();

    for(size_t i = 0; i < tools.size(); i++) {
        ToolInfo ti = tools.at(i);
        long item = AppendListCtrlRow(m_listCtrlTools);

        ExternalToolData* data = new ExternalToolData(ti);
        m_listCtrlTools->SetItemPtrData(item, wxUIntPtr(data));

        SetColumnText(m_listCtrlTools, item, 0, ti.GetId());
        SetColumnText(m_listCtrlTools, item, 1, ti.GetName());
        SetColumnText(m_listCtrlTools, item, 2, ti.GetPath());
    }

    m_listCtrlTools->Thaw();
}
