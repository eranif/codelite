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
#include "windowattrmanager.h"
#include "debuggermanager.h"
#include "globals.h"
#include "procutils.h"
#include <algorithm>
#include "fileutils.h"
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

    if(choices.IsEmpty() == false) {
        m_choiceDebugger->SetSelection(0);
    }

    RefreshProcessesList(wxEmptyString);
    m_textCtrlFilter->SetFocus();
    CentreOnParent();

    SetName("AttachDbgProcDlg");
    WindowAttrManager::Load(this);
}

void AttachDbgProcDlg::RefreshProcessesList(wxString filter, int colToSort)
{
    wxWindowUpdateLocker locker(m_dvListCtrl);
    m_dvListCtrl->DeleteAllItems();

    filter.Trim().Trim(false);

    // Populate the list with list of processes
    std::vector<ProcessEntry> proclist;
    ProcUtils::GetProcessList(proclist);

    //    if(colToSort == 0) { // sort by PID
    //        std::sort(proclist.begin(), proclist.end(), PIDSorter());
    //
    //    } else if(colToSort == 1) { // sort by name
    //        std::sort(proclist.begin(), proclist.end(), NameSorter());
    //    }

    filter.MakeLower();
    for(size_t i = 0; i < proclist.size(); ++i) {

        // Use case in-sensitive match for the filter
        wxString entryName(proclist.at(i).name);

        // Append only processes that matches the filter string
        if(filter.IsEmpty() || FileUtils::FuzzyMatch(filter, entryName)) {
            const ProcessEntry& entry = proclist.at(i);
            if(entry.pid == (long)wxGetProcessId()) continue;
            wxVector<wxVariant> cols;
            cols.push_back(wxString() << entry.pid);
            cols.push_back(entry.name);
            m_dvListCtrl->AppendItem(cols);
        }
    }
}

wxString AttachDbgProcDlg::GetExeName() const
{
    wxVariant v;
    int sel = m_dvListCtrl->GetSelectedRow();
    if(sel != wxNOT_FOUND) {
        m_dvListCtrl->GetValue(v, sel, 1);
        return v.GetString();
    }
    return wxEmptyString;
}

wxString AttachDbgProcDlg::GetProcessId() const
{
    wxVariant v;
    int sel = m_dvListCtrl->GetSelectedRow();
    if(sel != wxNOT_FOUND) {
        m_dvListCtrl->GetValue(v, sel, 0);
        return v.GetString();
    }
    return wxEmptyString;
}


void AttachDbgProcDlg::OnBtnAttachUI(wxUpdateUIEvent& event) { event.Enable(m_dvListCtrl->GetSelectedRow() != wxNOT_FOUND); }

AttachDbgProcDlg::~AttachDbgProcDlg() {}

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
