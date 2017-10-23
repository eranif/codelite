//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : unittestspage.cpp
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

#include "workspace.h"
#include "globals.h"
#include "unittestspage.h"
#include "imanager.h"
#include <wx/msgdlg.h>
#include "event_notifier.h"
#include "plugin.h"

class UTLineInfo : public wxClientData
{
public:
    ErrorLineInfo m_info;

public:
    UTLineInfo(const ErrorLineInfo& info)
        : m_info(info)
    {
    }
    virtual ~UTLineInfo() {}
};

UnitTestsPage::UnitTestsPage(wxWindow* parent, IManager* mgr)
    : UnitTestsBasePage(parent, wxID_ANY, wxDefaultPosition, wxSize(1, 1), 0)
    , m_mgr(mgr)
{
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(UnitTestsPage::OnWorkspaceClosed), NULL,
                                  this);
}

UnitTestsPage::~UnitTestsPage()
{
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(UnitTestsPage::OnWorkspaceClosed),
                                     NULL, this);
}

void UnitTestsPage::UpdateFailedBar(size_t amount, const wxString& msg) { m_progressFailed->Update(amount, msg); }

void UnitTestsPage::UpdatePassedBar(size_t amount, const wxString& msg) { m_progressPassed->Update(amount, msg); }

void UnitTestsPage::OnItemActivated(wxDataViewEvent& e)
{
    CHECK_ITEM_RET(e.GetItem());
    UTLineInfo* info = (UTLineInfo*)m_dvListCtrlErrors->GetItemData(e.GetItem());

    long lineNumber = wxNOT_FOUND;
    info->m_info.line.ToCLong(&lineNumber);

    // convert the file to absolute path
    wxString err_msg, cwd;
    wxString proj = m_mgr->GetWorkspace()->GetActiveProjectName();
    ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(proj, err_msg);

    if(p) {
        cwd = p->GetFileName().GetPath();
    }

    wxFileName fn(info->m_info.file);
    fn.MakeAbsolute(cwd);
    IEditor* editor = m_mgr->OpenFile(fn.GetFullPath(), wxEmptyString, (lineNumber - 1));
    if(editor) {
        editor->SetActive();
    }
}

void UnitTestsPage::Initialize(TestSummary* summary)
{
    Clear();

    m_progressPassed->SetMaxRange((size_t)summary->totalTests);
    m_progressFailed->SetMaxRange((size_t)summary->totalTests);
    m_progressFailed->SetFillCol(wxT("RED"));
    m_progressPassed->SetFillCol(wxT("PALE GREEN"));

    wxString msg;
    msg << summary->totalTests;
    m_staticTextTotalTests->SetLabel(msg);

    msg.clear();
    msg << summary->errorCount;
    m_staticTextFailTestsNum->SetLabel(msg);

    msg.clear();
    msg << summary->totalTests - summary->errorCount;
    m_staticTextSuccessTestsNum->SetLabel(msg);

    for(size_t i = 0; i < summary->errorLines.GetCount(); ++i) {
        const ErrorLineInfo& info = summary->errorLines.Item(i);
        wxVector<wxVariant> cols;
        cols.push_back(info.file);
        cols.push_back(info.line);
        cols.push_back(info.description);
        m_dvListCtrlErrors->AppendItem(cols, (wxUIntPtr) new UTLineInfo(info));
    }
}

void UnitTestsPage::Clear()
{
    for(int i = 0; i < m_dvListCtrlErrors->GetItemCount(); ++i) {
        UTLineInfo* info = (UTLineInfo*)m_dvListCtrlErrors->GetItemData(m_dvListCtrlErrors->RowToItem(i));
        wxDELETE(info);
    }
    m_dvListCtrlErrors->DeleteAllItems();
    m_progressFailed->Clear();
    m_progressPassed->Clear();
    m_staticTextFailTestsNum->SetLabel("");
    m_staticTextSuccessTestsNum->SetLabel("");
    m_staticTextTotalTests->SetLabel("");
}

void UnitTestsPage::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    Clear();
}
void UnitTestsPage::OnClearReport(wxCommandEvent& event)
{
    wxUnusedVar(event);
    Clear();
}
