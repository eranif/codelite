//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBThreadsView.cpp
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

#include "LLDBThreadsView.h"

#include "LLDBPlugin.h"

namespace
{

const int lldbSuspendThreadIds = XRCID("lldb_suspend_threads");
const int lldbSuspendOtherThreadIds = XRCID("lldb_suspend_other_threads");
const int lldbResumeThreadIds = XRCID("lldb_resume_threads");
const int lldbResumeOtherThreadIds = XRCID("lldb_resume_other_threads");
const int lldbResumeAllThreadsId = XRCID("lldb_resume_all_threads");

} // namespace

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

LLDBThreadsView::LLDBThreadsView(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBThreadsViewBase(parent)
    , m_plugin(plugin)
    , m_selectedThread(wxNOT_FOUND)
{
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_RUNNING, &LLDBThreadsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STOPPED, &LLDBThreadsView::OnLLDBStopped, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_EXITED, &LLDBThreadsView::OnLLDBExited, this);
    m_plugin->GetLLDB()->Bind(wxEVT_LLDB_STARTED, &LLDBThreadsView::OnLLDBStarted, this);
    m_dvListCtrlThreads->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &LLDBThreadsView::OnContextMenu, this);
}

LLDBThreadsView::~LLDBThreadsView()
{
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_RUNNING, &LLDBThreadsView::OnLLDBRunning, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_STOPPED, &LLDBThreadsView::OnLLDBStopped, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_EXITED, &LLDBThreadsView::OnLLDBExited, this);
    m_plugin->GetLLDB()->Unbind(wxEVT_LLDB_STARTED, &LLDBThreadsView::OnLLDBStarted, this);
    m_dvListCtrlThreads->Unbind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &LLDBThreadsView::OnContextMenu, this);
}

void LLDBThreadsView::OnItemActivated(wxDataViewEvent& event)
{
    CHECK_ITEM_RET(event.GetItem());
    LLDBThreadViewClientData* cd =
        reinterpret_cast<LLDBThreadViewClientData*>(m_dvListCtrlThreads->GetItemData(event.GetItem()));
    if(cd && !cd->GetThread().IsActive()) {
        m_plugin->GetLLDB()->SelectThread(cd->GetThread().GetId());
    }
}

void LLDBThreadsView::OnLLDBExited(LLDBEvent& event)
{
    event.Skip();
    DoCleanup();
}

void LLDBThreadsView::OnLLDBRunning(LLDBEvent& event)
{
    event.Skip();
    DoCleanup();
}

void LLDBThreadsView::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    DoCleanup();
}

void LLDBThreadsView::OnLLDBStopped(LLDBEvent& event)
{
    event.Skip();
    DoCleanup();
    // Update the thread view
    const LLDBThread::Vect_t& threads = event.GetThreads();
    for(size_t i = 0; i < threads.size(); ++i) {
        const LLDBThread& thr = threads.at(i);
        if(thr.IsActive()) {
            m_selectedThread = i;
        }
        wxVector<wxVariant> cols;
        cols.push_back(thr.GetId() == wxNOT_FOUND ? wxString() : wxString() << thr.GetId());
        cols.push_back(thr.GetName());
        cols.push_back(thr.GetStopReasonString());
        cols.push_back(thr.GetFunc());
        cols.push_back(m_plugin->GetFilenameForDisplay(thr.GetFile()));
        cols.push_back(thr.GetLine() == wxNOT_FOUND ? wxString() : wxString() << thr.GetLine());
        m_dvListCtrlThreads->AppendItem(cols, (wxUIntPtr) new LLDBThreadViewClientData(thr));
    }

    if((wxNOT_FOUND != m_selectedThread) && ((int)m_dvListCtrlThreads->GetItemCount() > m_selectedThread)) {
        const auto item = m_dvListCtrlThreads->RowToItem(m_selectedThread);
        if(item.IsOk()) {
            m_dvListCtrlThreads->EnsureVisible(item);
        }
    }
}

void LLDBThreadsView::DoCleanup()
{
    for(size_t i = 0; i < m_dvListCtrlThreads->GetItemCount(); ++i) {
        LLDBThreadViewClientData* cd = reinterpret_cast<LLDBThreadViewClientData*>(
            m_dvListCtrlThreads->GetItemData(m_dvListCtrlThreads->RowToItem(i)));
        wxDELETE(cd);
    }
    m_dvListCtrlThreads->DeleteAllItems();
    m_selectedThread = wxNOT_FOUND;
}

void LLDBThreadsView::OnContextMenu(wxDataViewEvent& event)
{
    wxDataViewItemArray items;
    m_dvListCtrlThreads->GetSelections(items);

    std::vector<int> threadIds;
    for(size_t i = 0; i < items.GetCount(); ++i) {
        const auto& item = items.Item(i);
        const auto cd = reinterpret_cast<LLDBThreadViewClientData*>(m_dvListCtrlThreads->GetItemData(item));
        if(cd) {
            const auto threadId = cd->GetThread().GetId();
            if(threadId != wxNOT_FOUND) {
                threadIds.push_back(threadId);
            }
        }
    }

    wxMenu menu;
    if(!threadIds.empty()) {
        menu.Append(lldbSuspendThreadIds, wxPLURAL("Suspend thread", "Suspend threads", threadIds.size()));
        menu.Append(lldbSuspendOtherThreadIds, _("Suspend other threads"));
        menu.AppendSeparator();
        menu.Append(lldbResumeThreadIds, wxPLURAL("Resume thread", "Resume threads", threadIds.size()));
        menu.Append(lldbResumeOtherThreadIds, _("Resume other threads"));
    }

    menu.Append(lldbResumeAllThreadsId, _("Resume all threads"));

    const auto sel = GetPopupMenuSelectionFromUser(menu);

    if(lldbSuspendThreadIds == sel) {
        m_plugin->GetLLDB()->SuspendThreads(threadIds);
    } else if(lldbSuspendOtherThreadIds == sel) {
        m_plugin->GetLLDB()->SuspendOtherThreads(threadIds);
    } else if(lldbResumeThreadIds == sel) {
        m_plugin->GetLLDB()->ResumeThreads(threadIds);
    } else if(lldbResumeOtherThreadIds == sel) {
        m_plugin->GetLLDB()->ResumeOtherThreads(threadIds);
    } else if(lldbResumeAllThreadsId == sel) {
        m_plugin->GetLLDB()->ResumeAllThreads();
    }
}
