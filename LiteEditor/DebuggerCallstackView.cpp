//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : listctrlpanel.cpp
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
#include "DebuggerCallstackView.h"
#include "globals.h"
#include "manager.h"
#include "pluginmanager.h"

#include "debugger.h"
#include "debuggermanager.h"
#include "event_notifier.h"
#include "globals.h"
#include <memory>
#include <wx/imaglist.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

DebuggerCallstackView::DebuggerCallstackView(wxWindow* parent)
    : ListCtrlPanelBase(parent)
    , m_currLevel(0)
{
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_LIST_FRAMES,
                                  clCommandEventHandler(DebuggerCallstackView::OnUpdateBacktrace), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_FRAME_SELECTED,
                                  clCommandEventHandler(DebuggerCallstackView::OnFrameSelected), NULL, this);
}
DebuggerCallstackView::~DebuggerCallstackView()
{
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_LIST_FRAMES,
                                     clCommandEventHandler(DebuggerCallstackView::OnUpdateBacktrace), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_FRAME_SELECTED,
                                     clCommandEventHandler(DebuggerCallstackView::OnFrameSelected), NULL, this);
}

void DebuggerCallstackView::OnItemActivated(wxDataViewEvent& event)
{
    int row = m_dvListCtrl->ItemToRow(event.GetItem());

    StackEntry* entry = reinterpret_cast<StackEntry*>(m_dvListCtrl->GetItemData(event.GetItem()));
    if(entry) {
        long frame, frameLine;
        if(!entry->level.ToLong(&frame)) { frame = 0; }

        // Remove the currently selected item
        wxDataViewItem curitem = m_dvListCtrl->RowToItem(m_currLevel);
        if(curitem.IsOk()) {
            wxVariant v;
            v = ::MakeIconText(wxString() << m_currLevel, m_images.Bitmap("arrowInactive"));
            m_dvListCtrl->SetValue(v, m_currLevel, 0);
        }

        entry->line.ToLong(&frameLine);
        SetCurrentLevel(row);
        ManagerST::Get()->DbgSetFrame(frame, frameLine);

        // At this point m_currLevel is pointing to the new stack level
        // set it as the active one
        curitem = m_dvListCtrl->RowToItem(m_currLevel);
        if(curitem.IsOk()) {
            wxVariant v;
            v = ::MakeIconText(wxString() << m_currLevel, m_images.Bitmap("arrowActive"));
            m_dvListCtrl->SetValue(v, m_currLevel, 0);
        }
    }
}

void DebuggerCallstackView::Update(const StackEntryArray& stackArr)
{
    Clear();
    m_stack.insert(m_stack.end(), stackArr.begin(), stackArr.end());
    int activeFrame(-1);
    if(!m_stack.empty()) {
        for(int i = 0; i < (int)m_stack.size(); i++) {

            bool isactive = (i == m_currLevel);
            StackEntry entry = m_stack.at(i);
            wxVector<wxVariant> cols;
            cols.push_back(::MakeIconText(entry.level, isactive ? m_images.Bitmap("arrowActive")
                                                                : m_images.Bitmap("arrowInactive")));
            cols.push_back(entry.function);
            cols.push_back(entry.file);
            cols.push_back(entry.line);
            cols.push_back(entry.address);
            StackEntry* d = new StackEntry(entry);
            m_dvListCtrl->AppendItem(cols, (wxUIntPtr)d);

            if(isactive) { activeFrame = i; }
        }
        if(activeFrame != wxNOT_FOUND) {
            wxDataViewItem item = m_dvListCtrl->RowToItem(activeFrame);
            if(item.IsOk()) { m_dvListCtrl->EnsureVisible(item); }
        }
    }
}

void DebuggerCallstackView::SetCurrentLevel(const int level)
{
    // Set m_currLevel to level, or 0 if level is out of bounds
    m_currLevel = (level >= 0 && level < m_dvListCtrl->GetItemCount()) ? level : 0;
}

void DebuggerCallstackView::Clear()
{
    m_stack.clear();
    for(int i = 0; i < m_dvListCtrl->GetItemCount(); ++i) {
        wxDataViewItem item = m_dvListCtrl->GetStore()->GetItem(i);
        if(item.IsOk()) {
            StackEntry* entry = reinterpret_cast<StackEntry*>(m_dvListCtrl->GetItemData(item));
            if(entry) { delete entry; }
        }
    }
    m_dvListCtrl->DeleteAllItems();
}
void DebuggerCallstackView::OnMenu(wxDataViewEvent& event)
{
    // Popup the menu
    wxMenu menu;

    menu.Append(XRCID("stack_copy_backtrace"), _("Copy Backtrace to Clipboard"));
    menu.Connect(XRCID("stack_copy_backtrace"), wxEVT_COMMAND_MENU_SELECTED,
                 wxCommandEventHandler(DebuggerCallstackView::OnCopyBacktrace), NULL, this);
    m_dvListCtrl->PopupMenu(&menu);
}

void DebuggerCallstackView::OnCopyBacktrace(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString trace;
    for(size_t i = 0; i < m_stack.size(); ++i) {
        trace << m_stack.at(i).level << wxT("  ") << m_stack.at(i).address << wxT("  ") << m_stack.at(i).function
              << wxT("  ") << m_stack.at(i).file << wxT("  ") << m_stack.at(i).line << wxT("\n");
    }
    trace.RemoveLast();
    ::CopyToClipboard(trace);
}

void DebuggerCallstackView::OnUpdateBacktrace(clCommandEvent& e)
{
    e.Skip();
    Update(static_cast<DebuggerEventData*>(e.GetClientObject())->m_stack);
}

void DebuggerCallstackView::OnFrameSelected(clCommandEvent& e)
{
    e.Skip();
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if(dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
        // set the frame
        dbgr->QueryFileLine();
    }
}
