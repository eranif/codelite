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

#include "ColoursAndFontsManager.h"
#include "LLDBNewBreakpointDlg.h"
#include "LLDBOutputView.h"
#include "LLDBPlugin.h"
#include "LLDBProtocol/LLDBBreakpoint.h"
#include "LLDBProtocol/LLDBEnums.h"
#include "bitmap_loader.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "lexer_configuration.h"

class LLDBBreakpointClientData : public wxTreeItemData
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
    auto images = m_tbBreakpoints->GetBitmapsCreateIfNeeded();
    m_tbBreakpoints->AddTool(wxID_DELETE, _("Delete Breakpoint"), images->Add("minus"));
    m_tbBreakpoints->AddTool(wxID_CLEAR, _("Delete All Breakpoints"), images->Add("clean"));
    m_tbBreakpoints->AddTool(wxID_NEW, _("New Breakpoint"), images->Add("plus"));
    m_tbBreakpoints->Realize();
    m_tbBreakpoints->Bind(wxEVT_TOOL, &LLDBOutputView::OnDeleteBreakpoint, this, wxID_DELETE);
    m_tbBreakpoints->Bind(wxEVT_UPDATE_UI, &LLDBOutputView::OnDeleteBreakpointUI, this, wxID_DELETE);
    m_tbBreakpoints->Bind(wxEVT_TOOL, &LLDBOutputView::OnDeleteAll, this, wxID_CLEAR);
    m_tbBreakpoints->Bind(wxEVT_UPDATE_UI, &LLDBOutputView::OnDeleteAllUI, this, wxID_CLEAR);
    m_tbBreakpoints->Bind(wxEVT_TOOL, &LLDBOutputView::OnNewBreakpoint, this, wxID_NEW);
    m_tbBreakpoints->Bind(wxEVT_UPDATE_UI, &LLDBOutputView::OnNewBreakpointUI, this, wxID_NEW);

    Initialize();
    m_connector->Bind(wxEVT_LLDB_INTERPERTER_REPLY, &LLDBOutputView::OnConsoleOutput, this);
    m_connector->Bind(wxEVT_LLDB_STARTED, &LLDBOutputView::OnLLDBStarted, this);
    m_connector->Bind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBOutputView::OnBreakpointsUpdated, this);
    m_connector->Bind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBOutputView::OnBreakpointsUpdated, this);
    EventNotifier::Get()->TopFrame()->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBOutputView::OnSelectAll, this,
                                           wxID_SELECTALL);
    EventNotifier::Get()->TopFrame()->Bind(wxEVT_COMMAND_MENU_SELECTED, &LLDBOutputView::OnCopy, this, wxID_COPY);
    m_treeCtrlBreakpoints->AddHeader("#");
    m_treeCtrlBreakpoints->AddHeader("Function");
    m_treeCtrlBreakpoints->AddHeader("File");
    m_treeCtrlBreakpoints->AddHeader("Line");
    m_treeCtrlBreakpoints->AddRoot("Breakpoints");
}

LLDBOutputView::~LLDBOutputView()
{
    m_connector->Unbind(wxEVT_LLDB_INTERPERTER_REPLY, &LLDBOutputView::OnConsoleOutput, this);
    m_connector->Unbind(wxEVT_LLDB_STARTED, &LLDBOutputView::OnLLDBStarted, this);
    m_connector->Unbind(wxEVT_LLDB_BREAKPOINTS_UPDATED, &LLDBOutputView::OnBreakpointsUpdated, this);
    m_connector->Unbind(wxEVT_LLDB_BREAKPOINTS_DELETED_ALL, &LLDBOutputView::OnBreakpointsUpdated, this);
    EventNotifier::Get()->TopFrame()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBOutputView::OnSelectAll, this,
                                             wxID_SELECTALL);
    EventNotifier::Get()->TopFrame()->Unbind(wxEVT_COMMAND_MENU_SELECTED, &LLDBOutputView::OnCopy, this, wxID_COPY);
}

void LLDBOutputView::Clear()
{
    // free all user data from the list
    m_treeCtrlBreakpoints->DeleteAllItems();
    m_treeCtrlBreakpoints->AddRoot("Breakpoints");
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

        LLDBBreakpoint::Ptr_t bp = breakpoints.at(i);
        wxTreeItemId parent =
            m_treeCtrlBreakpoints->AppendItem(m_treeCtrlBreakpoints->GetRootItem(), wxString() << bp->GetId(),
                                              wxNOT_FOUND, wxNOT_FOUND, new LLDBBreakpointClientData(bp));
        m_treeCtrlBreakpoints->SetItemText(parent, bp->GetName(), 1);
        m_treeCtrlBreakpoints->SetItemText(parent, m_plugin->GetFilenameForDisplay(bp->GetFilename()), 2);
        m_treeCtrlBreakpoints->SetItemText(parent, wxString() << bp->GetLineNumber(), 3);

        // add the children breakpoints (sites)
        if(!bp->GetChildren().empty()) {
            const LLDBBreakpoint::Vec_t& children = bp->GetChildren();
            for(size_t i = 0; i < children.size(); ++i) {
                LLDBBreakpoint::Ptr_t breakpoint = children.at(i);
                wxTreeItemId child =
                    m_treeCtrlBreakpoints->AppendItem(parent, wxString() << breakpoint->GetId(), wxNOT_FOUND,
                                                      wxNOT_FOUND, new LLDBBreakpointClientData(breakpoint));
                m_treeCtrlBreakpoints->SetItemText(child, breakpoint->GetName(), 1);
                m_treeCtrlBreakpoints->SetItemText(child, m_plugin->GetFilenameForDisplay(breakpoint->GetFilename()),
                                                   2);
                m_treeCtrlBreakpoints->SetItemText(child, wxString() << breakpoint->GetLineNumber(), 3);
            }
        }
    }
}

void LLDBOutputView::OnBreakpointsUpdated(LLDBEvent& event)
{
    event.Skip();
    clDEBUG() << "Setting LLDB breakpoints to:";
    for(size_t i = 0; i < event.GetBreakpoints().size(); ++i) {
        clDEBUG() << event.GetBreakpoints().at(i)->ToString();
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

void LLDBOutputView::OnDeleteAllUI(wxUpdateUIEvent& event)
{
    event.Enable(m_treeCtrlBreakpoints->GetChildrenCount(m_treeCtrlBreakpoints->GetRootItem()));
}

void LLDBOutputView::OnDeleteBreakpoint(wxCommandEvent& event)
{
    // get the breakpoint we want to delete
    wxArrayTreeItemIds selections;
    m_treeCtrlBreakpoints->GetSelections(selections);
    for(size_t i = 0; i < selections.GetCount(); ++i) {
        m_connector->MarkBreakpointForDeletion(GetBreakpoint(selections.Item(i)));
    }
    m_connector->DeleteBreakpoints();
}

void LLDBOutputView::OnDeleteBreakpointUI(wxUpdateUIEvent& event)
{
    wxTreeItemId item = m_treeCtrlBreakpoints->GetSelection();
    LLDBBreakpoint::Ptr_t bp = GetBreakpoint(item);
    event.Enable(bp && !bp->IsLocation());
}

LLDBBreakpoint::Ptr_t LLDBOutputView::GetBreakpoint(const wxTreeItemId& item)
{
    if(!item.IsOk()) {
        return LLDBBreakpoint::Ptr_t(NULL);
    }
    LLDBBreakpointClientData* cd = dynamic_cast<LLDBBreakpointClientData*>(m_treeCtrlBreakpoints->GetItemData(item));
    if(cd) {
        return cd->GetBreakpoint();
    } else {
        return LLDBBreakpoint::Ptr_t(NULL);
    }
}

void LLDBOutputView::GotoBreakpoint(LLDBBreakpoint::Ptr_t bp)
{
    if(!bp)
        return;
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
        if(m_stcConsole->CanCopy())
            m_stcConsole->Copy();
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
    if(text.IsEmpty())
        return;

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
void LLDBOutputView::OnBpActivated(wxTreeEvent& event)
{
    event.Skip();
    CallAfter(&LLDBOutputView::GotoBreakpoint, GetBreakpoint(event.GetItem()));
}
