//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : LLDBBreakpointsPane.cpp
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

#include "LLDBOutputView.h"
#include "LLDBProtocol/LLDBBreakpoint.h"
#include "event_notifier.h"
#include "LLDBNewBreakpointDlg.h"
#include "LLDBPlugin.h"
#include "ieditor.h"
#include "LLDBProtocol/LLDBEnums.h"
#include "file_logger.h"
#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"

class LLDBBreakpointClientData : public wxClientData
{
    LLDBBreakpoint::Ptr_t m_breakpoint;

public:
    LLDBBreakpointClientData(LLDBBreakpoint::Ptr_t bp)
        : m_breakpoint(bp)
    {
    }

    LLDBBreakpoint::Ptr_t GetBreakpoint() { return m_breakpoint; }
};

LLDBOutputView::LLDBOutputView(wxWindow* parent, LLDBPlugin* plugin)
    : LLDBOutputViewBase(parent)
    , m_plugin(plugin)
    , m_connector(plugin->GetLLDB())
{
    Initialize();
    m_connector->Bind(wxEVT_LLDB_INTERPERTER_REPLY, &LLDBOutputView::OnConsoleOutput, this);
    m_connector->Bind(wxEVT_LLDB_STARTED, &LLDBOutputView::OnLLDBStarted, this);
    m_connector->Bind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBOutputView::OnBreakpointsUpdated, this);
    m_connector->Bind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBOutputView::OnBreakpointsUpdated, this);
    EventNotifier::Get()->TopFrame()->Bind(
        wxEVT_COMMAND_MENU_SELECTED, &LLDBOutputView::OnSelectAll, this, wxID_SELECTALL);
    EventNotifier::Get()->TopFrame()->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBOutputView::OnCopy, this, wxID_COPY);
}

LLDBOutputView::~LLDBOutputView()
{
    m_connector->Unbind(wxEVT_LLDB_INTERPERTER_REPLY, &LLDBOutputView::OnConsoleOutput, this);
    m_connector->Unbind(wxEVT_LLDB_STARTED, &LLDBOutputView::OnLLDBStarted, this);
    m_connector->Unbind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBOutputView::OnBreakpointsUpdated, this);
    m_connector->Unbind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBOutputView::OnBreakpointsUpdated, this);
    EventNotifier::Get()->TopFrame()->Unbind(
        wxEVT_COMMAND_MENU_SELECTED, &LLDBOutputView::OnSelectAll, this, wxID_SELECTALL);
    EventNotifier::Get()->TopFrame()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBOutputView::OnCopy, this, wxID_COPY);
}

void LLDBOutputView::OnBreakpointActivated(wxDataViewEvent& event)
{
    event.Skip();
    CallAfter(&LLDBOutputView::GotoBreakpoint, GetBreakpoint(event.GetItem()));
}

void LLDBOutputView::Clear()
{
    // free all user data from the list
    m_dataviewModel->Clear();
}

void LLDBOutputView::Initialize()
{
    Clear();
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        lexer->Apply(m_stcConsole);
    }

    // initialize the console view
    m_stcConsole->SetReadOnly(true);

    LLDBBreakpoint::Vec_t breakpoints = m_connector->GetAllBreakpoints();
    for(size_t i = 0; i < breakpoints.size(); ++i) {

        wxVector<wxVariant> cols;
        LLDBBreakpoint::Ptr_t bp = breakpoints.at(i);
        cols.push_back(wxString() << bp->GetId());
        cols.push_back(bp->GetName());
        cols.push_back(m_plugin->GetFilenameForDisplay(bp->GetFilename()));
        cols.push_back(wxString() << bp->GetLineNumber());
        wxDataViewItem parent =
            m_dataviewModel->AppendItem(wxDataViewItem(NULL), cols, new LLDBBreakpointClientData(bp));

        // add the children breakpoints (sites)
        if(!bp->GetChildren().empty()) {
            const LLDBBreakpoint::Vec_t& children = bp->GetChildren();
            for(size_t i = 0; i < children.size(); ++i) {
                cols.clear();
                LLDBBreakpoint::Ptr_t breakpoint = children.at(i);
                cols.push_back("");
                cols.push_back(breakpoint->GetName());
                cols.push_back(m_plugin->GetFilenameForDisplay(breakpoint->GetFilename()));
                cols.push_back(wxString() << breakpoint->GetLineNumber());
                m_dataviewModel->AppendItem(parent, cols, new LLDBBreakpointClientData(breakpoint));
            }
        }
    }
}

void LLDBOutputView::OnBreakpointsUpdated(LLDBEvent& event)
{
    event.Skip();
    CL_DEBUG("Setting LLDB breakpoints to:");
    for(size_t i = 0; i < event.GetBreakpoints().size(); ++i) {
        CL_DEBUG("%s", event.GetBreakpoints().at(i)->ToString());
    }
    m_connector->UpdateAppliedBreakpoints(event.GetBreakpoints());
    Initialize();
}

void LLDBOutputView::OnNewBreakpoint(wxCommandEvent& event)
{
    LLDBNewBreakpointDlg dlg(EventNotifier::Get()->TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        LLDBBreakpoint::Ptr_t bp = dlg.GetBreakpoint();
        if(bp->IsValid()) {
            m_connector->AddBreakpoint(bp);
            m_connector->ApplyBreakpoints();
        }
    }
}

void LLDBOutputView::OnNewBreakpointUI(wxUpdateUIEvent& event) { event.Enable(true); }

void LLDBOutputView::OnDeleteAll(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_connector->DeleteAllBreakpoints();
}

void LLDBOutputView::OnDeleteAllUI(wxUpdateUIEvent& event) { event.Enable(!m_dataviewModel->IsEmpty()); }

void LLDBOutputView::OnDeleteBreakpoint(wxCommandEvent& event)
{
    // get the breakpoint we want to delete
    wxDataViewItemArray items;
    m_dataview->GetSelections(items);
    for(size_t i = 0; i < items.GetCount(); ++i) {
        m_connector->MarkBreakpointForDeletion(GetBreakpoint(items.Item(i)));
    }
    m_connector->DeleteBreakpoints();
}

void LLDBOutputView::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
    wxDataViewItem item = m_dataview->GetSelection();
    LLDBBreakpoint::Ptr_t bp = GetBreakpoint(item);
    event.Enable(bp && !bp->IsLocation());
}

LLDBBreakpoint::Ptr_t LLDBOutputView::GetBreakpoint(const wxDataViewItem& item)
{
    if(!item.IsOk()) {
        return LLDBBreakpoint::Ptr_t(NULL);
    }

    LLDBBreakpointClientData* cd = dynamic_cast<LLDBBreakpointClientData*>(m_dataviewModel->GetClientObject(item));
    return cd->GetBreakpoint();
}

void LLDBOutputView::GotoBreakpoint(LLDBBreakpoint::Ptr_t bp)
{
    if(!bp) return;
    wxFileName fileLoc(bp->GetFilename());
    if(fileLoc.Exists()) {
        if(m_plugin->GetManager()->OpenFile(fileLoc.GetFullPath(), "", bp->GetLineNumber() - 1)) {
            IEditor* editor = m_plugin->GetManager()->GetActiveEditor();
            if(editor) {
                editor->SetActive();
            }
        }
    }
}

void LLDBOutputView::OnSelectAll(wxCommandEvent& event)
{
    event.Skip();
    if(m_stcConsole->HasFocus()) {
        event.Skip(false);
        m_stcConsole->SelectAll();
    }
}

void LLDBOutputView::OnCopy(wxCommandEvent& event)
{
    event.Skip();
    if(m_stcConsole->HasFocus()) {
        event.Skip(false);
        if(m_stcConsole->CanCopy()) m_stcConsole->Copy();
    }
}

void LLDBOutputView::OnConsoleOutput(LLDBEvent& event)
{
    event.Skip();
    m_stcConsole->SetReadOnly(false);
    wxString text;
    if(!m_stcConsole->IsEmpty() && !m_stcConsole->GetText().EndsWith("\n")) {
        text << "\n";
    }
    text << event.GetString();
    text.Trim();
    if(text.IsEmpty()) return;

    text << "\n";
    m_stcConsole->AppendText(text);
    m_stcConsole->SetReadOnly(true);
    int lastPos = m_stcConsole->GetLastPosition();
    m_stcConsole->SetCurrentPos(lastPos);
    m_stcConsole->SetSelectionStart(lastPos);
    m_stcConsole->SetSelectionEnd(lastPos);
    m_stcConsole->ScrollToEnd();

    // give the focus back to the console text control
    m_textCtrlConsoleSend->CallAfter(&wxTextCtrl::SetFocus);
}

void LLDBOutputView::OnSendCommandToLLDB(wxCommandEvent& event)
{
    wxString commandText = m_textCtrlConsoleSend->GetValue();
    commandText.Trim().Trim(false);

    if(commandText.Lower() == "quit" || commandText.Lower() == "exit") {
        m_textCtrlConsoleSend->ChangeValue("");
        return;
    }
    m_connector->SendInterperterCommand(commandText);
    m_textCtrlConsoleSend->ChangeValue("");
}

void LLDBOutputView::OnLLDBStarted(LLDBEvent& event)
{
    event.Skip();
    m_stcConsole->SetReadOnly(false);
    m_stcConsole->ClearAll();
    m_stcConsole->SetReadOnly(true);
}
