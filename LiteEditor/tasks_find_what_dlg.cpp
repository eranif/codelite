//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : tasks_find_what_dlg.cpp
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

#include "tasks_find_what_dlg.h"
#include <wx/msgdlg.h>
#include <wx/regex.h>
#include "editor_config.h"
#include "taskspaneldata.h"
#include "windowattrmanager.h"

class TaskData : public wxClientData
{
public:
    wxString m_regex;

public:
    TaskData(const wxString& regex)
        : m_regex(regex)
    {
    }
    virtual ~TaskData() {}
};

TasksFindWhatDlg::TasksFindWhatDlg(wxWindow* parent)
    : TasksFindWhatDlgBase(parent)
{
    m_list->InsertColumn(0, _("Enabled"));
    m_list->InsertColumn(1, _("Task"));
    m_list->SetColumnWidth(1, 200);

    // Load all info from disk
    TasksPanelData data;
    EditorConfigST::Get()->ReadObject(wxT("TasksPanelData"), &data);

    wxStringMap_t::const_iterator iter = data.GetTasks().begin();
    for(; iter != data.GetTasks().end(); iter++) {
        DoAddLine(iter->first, iter->second, data.GetEnabledItems().Index(iter->first) != wxNOT_FOUND);
    }
    SetName("TasksFindWhatDlg");
    WindowAttrManager::Load(this);
}

TasksFindWhatDlg::~TasksFindWhatDlg() {}

void TasksFindWhatDlg::OnNewTask(wxCommandEvent& event)
{
    NewTaskDialog dlg(this);
    WindowAttrManager::Load(this);

    dlg.SetLabel(_("New Task"));
    if(dlg.ShowModal() == wxID_OK) {
        wxRegEx re(dlg.m_regex->GetValue());
        if(re.IsValid() == false) {
            wxMessageBox(wxString::Format(_("'%s' is not a valid regular expression"), dlg.m_regex->GetValue().c_str()),
                         _("CodeLite"),
                         wxICON_WARNING | wxOK);
            return;
        }
        DoAddLine(dlg.m_name->GetValue(), dlg.m_regex->GetValue(), true);
    }
}

void TasksFindWhatDlg::OnDeleteTask(wxCommandEvent& event)
{
    int selection = m_list->GetSelection();
    if(selection == wxNOT_FOUND) return;

    int answer =
        wxMessageBox(_("Are you sure you want to delete this entry?"), _("Confirm"), wxICON_QUESTION | wxYES_NO);
    if(answer == wxYES) {
        m_list->DeleteItem(selection);
    }
}

void TasksFindWhatDlg::OnDeleteTaskUI(wxUpdateUIEvent& event)
{
    event.Enable(m_list->GetItemCount() && m_list->GetSelection() != wxNOT_FOUND);
}

void TasksFindWhatDlg::OnEditTask(wxCommandEvent& event)
{
    int selection = m_list->GetSelection();
    if(selection != wxNOT_FOUND) {

        NewTaskDialog dlg(this);
        WindowAttrManager::Load(this);

        dlg.SetLabel(_("Edit Task"));
        dlg.m_name->SetValue(m_list->GetText(selection, 1));

        TaskData* data = (TaskData*)m_list->GetItemData(selection);
        dlg.m_regex->SetValue(data->m_regex);
        if(dlg.ShowModal() == wxID_OK) {
            m_list->SetTextColumn(selection, 1, dlg.m_name->GetValue());
            m_list->SetItemClientData(selection, new TaskData(dlg.m_regex->GetValue()));
        }
    }
}

void TasksFindWhatDlg::OnEditTaskUI(wxUpdateUIEvent& event)
{
    event.Enable(m_list->GetItemCount() && m_list->GetSelection() != wxNOT_FOUND);
}

void TasksFindWhatDlg::DoAddLine(const wxString& name, const wxString& regex, bool enabled)
{
    long item = m_list->AppendRow();
    m_list->SetCheckboxRow(item, enabled);
    m_list->SetTextColumn(item, 1, name);
    m_list->SetItemClientData(item, new TaskData(regex));
}

void TasksFindWhatDlg::DoSaveList()
{
    // Save all items
    TasksPanelData data;
    wxStringMap_t items;
    wxArrayString enabledItems;

    for(int i = 0; i < m_list->GetItemCount(); i++) {
        wxString name = m_list->GetText(i, 1);
        TaskData* clientData = (TaskData*)m_list->GetItemData(i);
        items[name] = clientData->m_regex;

        if(m_list->IsChecked(i)) enabledItems.Add(name);
    }

    data.SetEnabledItems(enabledItems);
    data.SetTasks(items);
    EditorConfigST::Get()->WriteObject(wxT("TasksPanelData"), &data);
}

void TasksFindWhatDlg::OnButtonOk(wxCommandEvent& event)
{
    DoSaveList();
    event.Skip();
}
