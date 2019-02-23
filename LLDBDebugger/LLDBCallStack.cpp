//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBCallStack.cpp
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

#include "LLDBCallStack.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBConnector.h"
#include "LLDBPlugin.h"
#include <wx/wupdlock.h>
#include "macros.h"
#include "globals.h"
#include "file_logger.h"

LLDBCallStackPane::LLDBCallStackPane(wxWindow* parent, LLDBPlugin& plugin)
    : LLDBCallStackBase(parent)
    , m_plugin(plugin)
    , m_connector(plugin.GetLLDB())
    , m_selectedFrame(0)
{
    m_connector->Bind(wxEVT_LLDB_STOPPED, &LLDBCallStackPane::OnBacktrace, this);
    m_connector->Bind(wxEVT_LLDB_RUNNING, &LLDBCallStackPane::OnRunning, this);
}

LLDBCallStackPane::~LLDBCallStackPane()
{
    m_connector->Unbind(wxEVT_LLDB_STOPPED, &LLDBCallStackPane::OnBacktrace, this);
    m_connector->Unbind(wxEVT_LLDB_RUNNING, &LLDBCallStackPane::OnRunning, this);
}

void LLDBCallStackPane::OnBacktrace(LLDBEvent& event)
{
    event.Skip();

    SetSelectedFrame(0); // Clear the selected frame
    wxWindowUpdateLocker locker(m_dvListCtrlBacktrace);
    m_dvListCtrlBacktrace->DeleteAllItems();
    const LLDBBacktrace& bt = event.GetBacktrace();
    SetSelectedFrame(bt.GetSelectedFrameId());

    const LLDBBacktrace::EntryVec_t& entries = bt.GetCallstack();
    wxDataViewItem selectedItem;
    for(size_t i = 0; i < entries.size(); ++i) {
        const LLDBBacktrace::Entry& entry = entries.at(i);
        wxVector<wxVariant> cols;
        cols.push_back(wxString() << entry.id);
        cols.push_back(entry.functionName);
        cols.push_back(m_plugin.GetFilenameForDisplay(entry.filename));
        cols.push_back(wxString() << (int)(entry.line + 1));
        m_dvListCtrlBacktrace->AppendItem(cols);
        wxDataViewItem item = m_dvListCtrlBacktrace->RowToItem(i);
        if(bt.GetSelectedFrameId() == entry.id) { selectedItem = item; }
    }

    if(selectedItem.IsOk()) {
        m_dvListCtrlBacktrace->Select(selectedItem);
        m_dvListCtrlBacktrace->EnsureVisible(selectedItem);
    }
}

void LLDBCallStackPane::OnRunning(LLDBEvent& event)
{
    event.Skip();
    m_dvListCtrlBacktrace->DeleteAllItems();
    SetSelectedFrame(0);
}

void LLDBCallStackPane::OnItemActivated(wxDataViewEvent& event)
{
    // Activate the selected frame
    CHECK_ITEM_RET(event.GetItem());
    int rowNum = m_dvListCtrlBacktrace->ItemToRow(event.GetItem());
    m_connector->SelectFrame(rowNum);
}

bool CallstackModel::GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const
{
    int row = m_view->ItemToRow(item);
    if(row == m_ctrl->GetSelectedFrame()) {
        attr.SetBold(true);
        return true;
    }
    return false;
}

void LLDBCallStackPane::OnContextMenu(wxDataViewEvent& event)
{
    wxMenu menu;

    menu.Append(11981, _("Copy backtrace"), _("Copy backtrace"));
    int selection = GetPopupMenuSelectionFromUser(menu);
    switch(selection) {
    case 11981:
        DoCopyBacktraceToClipboard();
        break;
    default:
        break;
    }
}

void LLDBCallStackPane::DoCopyBacktraceToClipboard()
{
    wxString callstack;
    for(size_t i = 0; i < m_dvListCtrlBacktrace->GetItemCount(); ++i) {
        wxString line;
        for(size_t col = 0; col < m_dvListCtrlBacktrace->GetHeader()->size(); ++col) {
            line << m_dvListCtrlBacktrace->GetItemText(m_dvListCtrlBacktrace->RowToItem(i), col) << " ";
        }
        callstack << line << "\n";
    }
    clDEBUG() << "LLDB: Copying stack to clipboard";
    ::CopyToClipboard(callstack);
}
