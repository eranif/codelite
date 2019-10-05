//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : attachdbgprocdlg.cpp
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
#include "attachdbgprocdlg.h"
#include "cl_config.h"
#include "debuggermanager.h"
#include "fileutils.h"
#include "globals.h"
#include "procutils.h"
#include "windowattrmanager.h"
#include <algorithm>
#include <wx/wupdlock.h>

/// Ascending sorting function
struct PIDSorter {
    bool operator()(const ProcessEntry& rStart, const ProcessEntry& rEnd) { return rEnd.pid < rStart.pid; }
};

struct NameSorter {
    bool operator()(const ProcessEntry& rStart, const ProcessEntry& rEnd)
    {
        return rEnd.name.CmpNoCase(rStart.name) < 0;
    }
};

AttachDbgProcDlg::AttachDbgProcDlg(wxWindow* parent)
    : AttachDbgProcBaseDlg(parent)
{
    wxArrayString choices = DebuggerMgr::Get().GetAvailableDebuggers();
    m_choiceDebugger->Append(choices);

    if(choices.IsEmpty() == false) { m_choiceDebugger->SetSelection(0); }

    wxString filter = clConfig::Get().Read("AttachDebuggerDialog/Filter", wxString());
    m_textCtrlFilter->ChangeValue(filter);
    CallAfter(&AttachDbgProcDlg::RefreshProcessesList, filter);
    m_textCtrlFilter->SetFocus();
    SetName("AttachDbgProcDlg");
    WindowAttrManager::Load(this);
}

void AttachDbgProcDlg::RefreshProcessesList(wxString filter)
{
    wxWindowUpdateLocker locker(m_dvListCtrl);
    m_dvListCtrl->DeleteAllItems();

    filter.Trim().Trim(false);

    // Populate the list with list of processes
    std::vector<ProcessEntry> proclist;
    ProcUtils::GetProcessList(proclist);

    filter.MakeLower();
    for(size_t i = 0; i < proclist.size(); ++i) {

        // Use case in-sensitive match for the filter
        wxString processName;
        wxString processID;

        const ProcessEntry& processEntry = proclist.at(i);

        processName << processEntry.name;
        processID << processEntry.pid;

        // Append only processes that matches the filter string
        if(filter.IsEmpty() || FileUtils::FuzzyMatch(filter, processName) || FileUtils::FuzzyMatch(filter, processID)) {
            if(processEntry.pid == (long)wxGetProcessId()) continue;
            wxVector<wxVariant> cols;
            cols.push_back(wxString() << processEntry.pid);
            cols.push_back(processEntry.name);
            m_dvListCtrl->AppendItem(cols);
        }
    }
}

wxString AttachDbgProcDlg::GetExeName() const
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    CHECK_ITEM_RET_EMPTY_STRING(item);
    return m_dvListCtrl->GetItemText(item, 1);
}

wxString AttachDbgProcDlg::GetProcessId() const
{
    wxDataViewItem item = m_dvListCtrl->GetSelection();
    CHECK_ITEM_RET_EMPTY_STRING(item);
    return m_dvListCtrl->GetItemText(item, 0);
}

void AttachDbgProcDlg::OnBtnAttachUI(wxUpdateUIEvent& event)
{
    event.Enable(m_dvListCtrl->GetSelectedItemsCount() == 1);
}

AttachDbgProcDlg::~AttachDbgProcDlg()
{
    clConfig::Get().Write("AttachDebuggerDialog/Filter", m_textCtrlFilter->GetValue());
}

void AttachDbgProcDlg::OnFilter(wxCommandEvent& event)
{
    wxUnusedVar(event);
    RefreshProcessesList(m_textCtrlFilter->GetValue());
}

void AttachDbgProcDlg::OnRefresh(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // Clear all filters and refresh the processes list
    m_textCtrlFilter->Clear();
    RefreshProcessesList(wxEmptyString);
    m_textCtrlFilter->SetFocus();
}
void AttachDbgProcDlg::OnEnter(wxCommandEvent& event)
{
    if(!GetProcessId().IsEmpty()) { EndModal(wxID_OK); }
}
void AttachDbgProcDlg::OnItemActivated(wxDataViewEvent& event) { EndModal(wxID_OK); }
