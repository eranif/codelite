//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : DebuggerDisassemblyTab.cpp
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

#include "DebuggerDisassemblyTab.h"
#include "breakpointdlg.h"
#include "breakpointsmgr.h"
#include "debuggermanager.h"
#include "debuggerobserver.h"
#include "debuggerpane.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "frame.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "manager.h"
#include "plugin.h"

#define CURLINE_MARKER 7
#define BREAKPOINT_MARKER 8
#define BREAKPOINT_MARKER_MASK 1 << BREAKPOINT_MARKER

extern const char* stop_xpm[]; // Breakpoint

DebuggerDisassemblyTab::DebuggerDisassemblyTab(wxWindow* parent, const wxString& label)
    : DebuggerDisassemblyTabBase(parent)
    , m_title(label)
{
    m_stc->MarkerDefine(CURLINE_MARKER, wxSTC_MARK_BACKGROUND, wxNullColour,
                        EditorConfigST::Get()->GetOptions()->GetDebuggerMarkerLine());
    m_stc->MarkerSetAlpha(CURLINE_MARKER, 50);

    m_stc->SetReadOnly(true);
    m_stc->SetMarginSensitive(2, true); // 2 is the symbol margin

    m_stc->MarkerDefineBitmap(BREAKPOINT_MARKER, wxBitmap(wxImage(stop_xpm)));

    m_stc->SetYCaretPolicy(wxSTC_CARET_SLOP, 30);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT,
                                  clCommandEventHandler(DebuggerDisassemblyTab::OnOutput), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_DISASSEBLE_CURLINE,
                                  clCommandEventHandler(DebuggerDisassemblyTab::OnCurLine), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_QUERY_FILELINE,
                                  clCommandEventHandler(DebuggerDisassemblyTab::OnQueryFileLineDone), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_LIST_REGISTERS,
                                  clCommandEventHandler(DebuggerDisassemblyTab::OnShowRegisters), NULL, this);
    EventNotifier::Get()->Bind(wxEVT_DEBUG_ENDED, &DebuggerDisassemblyTab::OnDebuggerStopped, this);
    EventNotifier::Get()->Connect(wxEVT_CODELITE_ALL_BREAKPOINTS_DELETED,
                                  wxCommandEventHandler(DebuggerDisassemblyTab::OnAllBreakpointsDeleted), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_DEBUGGER_UPDATE_VIEWS,
                                  clCommandEventHandler(DebuggerDisassemblyTab::OnRefreshView), NULL, this);
    LexerConf::Ptr_t lex = EditorConfigST::Get()->GetLexer("Assembly");
    if(lex) { lex->Apply(m_stc, true); }
}

DebuggerDisassemblyTab::~DebuggerDisassemblyTab()
{
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_DISASSEBLE_OUTPUT,
                                     clCommandEventHandler(DebuggerDisassemblyTab::OnOutput), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_DISASSEBLE_CURLINE,
                                     clCommandEventHandler(DebuggerDisassemblyTab::OnCurLine), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_QUERY_FILELINE,
                                     clCommandEventHandler(DebuggerDisassemblyTab::OnQueryFileLineDone), NULL, this);
    EventNotifier::Get()->Unbind(wxEVT_DEBUG_ENDED, &DebuggerDisassemblyTab::OnDebuggerStopped, this);
    EventNotifier::Get()->Disconnect(wxEVT_CODELITE_ALL_BREAKPOINTS_DELETED,
                                     wxCommandEventHandler(DebuggerDisassemblyTab::OnAllBreakpointsDeleted), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_LIST_REGISTERS,
                                     clCommandEventHandler(DebuggerDisassemblyTab::OnShowRegisters), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_DEBUGGER_UPDATE_VIEWS,
                                     clCommandEventHandler(DebuggerDisassemblyTab::OnRefreshView), NULL, this);
}

void DebuggerDisassemblyTab::OnOutput(clCommandEvent& e)
{
    e.Skip();
    DoClearDisassembleView();
    m_stc->SetReadOnly(false);

    BreakpointInfoVec_t memBps;
    wxStringSet_t addressSet;
    ManagerST::Get()->GetBreakpointsMgr()->GetAllMemoryBreakpoints(memBps);
    for(size_t i = 0; i < memBps.size(); ++i) {
        addressSet.insert(memBps.at(i).memory_address);
    }
    // Parse the output
    DebuggerEventData* ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
    if(ded) {

        m_lines.insert(m_lines.end(), ded->m_disassembleLines.begin(), ded->m_disassembleLines.end());

        for(size_t i = 0; i < ded->m_disassembleLines.size(); ++i) {
            m_stc->AppendText(ded->m_disassembleLines.at(i).m_address + "  " + ded->m_disassembleLines.at(i).m_inst +
                              "\n");
            // restore breakpoints
            if(addressSet.count(ded->m_disassembleLines.at(i).m_address)) { m_stc->MarkerAdd(i, BREAKPOINT_MARKER); }
        }
    }
    clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
    m_stc->SetReadOnly(true);
}

void DebuggerDisassemblyTab::OnCurLine(clCommandEvent& e)
{
    e.Skip();
    size_t curline = wxString::npos;
    DebuggerEventData* ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
    if(ded && ded->m_disassembleLines.size()) {
        DisassembleEntry entry = ded->m_disassembleLines.at(0);
        for(size_t i = 0; i < m_lines.size(); ++i) {
            if(m_lines.at(i).m_address == entry.m_address) {
                curline = i;
                break;
            }
        }

        if(curline != wxString::npos) {
            m_stc->MarkerAdd(curline, CURLINE_MARKER);

            int pos = m_stc->PositionFromLine(curline);
            m_stc->SetCurrentPos(pos);
            m_stc->SetSelection(pos, pos);
            DoCentrLine(curline);
            // m_stc->EnsureCaretVisible();
            m_textCtrlCurFunction->ChangeValue(entry.m_function);
        }
    }
}

void DebuggerDisassemblyTab::OnDebuggerStopped(clDebugEvent& e)
{
    e.Skip();
    DoClearDisassembleView();
    DoClearRegistersView();
    m_oldValues.clear();
}

void DebuggerDisassemblyTab::DoClearDisassembleView()
{
    m_stc->SetReadOnly(false);
    m_lines.clear();
    m_stc->ClearAll();
    m_textCtrlCurFunction->ChangeValue("");
    m_stc->SetReadOnly(true);
}

void DebuggerDisassemblyTab::OnQueryFileLineDone(clCommandEvent& e) { e.Skip(); }

void DebuggerDisassemblyTab::DoCentrLine(int line)
{
    // we want to make 'first' centered
    int numLinesOnScreen = m_stc->LinesOnScreen();
    int linesAboveIt = numLinesOnScreen / 2;

    line = line - linesAboveIt;
    if(line < 0) line = 0;

    m_stc->SetFirstVisibleLine(line);
    m_stc->ClearSelections();
}

void DebuggerDisassemblyTab::OnMarginClicked(wxStyledTextEvent& event)
{
    /// get the address of the line
    int nLine = m_stc->LineFromPosition(event.GetPosition());
    wxString line = m_stc->GetLine(nLine);
    wxString address = line.BeforeFirst(' ').Trim(true).Trim(false);

    if(m_stc->MarkerGet(nLine) & BREAKPOINT_MARKER_MASK) {

        // we already got a marker there
        m_stc->MarkerDelete(nLine, BREAKPOINT_MARKER);
        ManagerST::Get()->GetBreakpointsMgr()->DelBreakpointByAddress(address);

    } else {
        m_stc->MarkerAdd(nLine, BREAKPOINT_MARKER);
        ManagerST::Get()->GetBreakpointsMgr()->AddBreakpointByAddress(address);
    }
    clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

void DebuggerDisassemblyTab::OnAllBreakpointsDeleted(wxCommandEvent& e)
{
    e.Skip();
    /// all breakpoints were deleted, update the UI
    m_stc->MarkerDeleteAll(BREAKPOINT_MARKER);
}

void DebuggerDisassemblyTab::OnShowRegisters(clCommandEvent& e)
{
    e.Skip();
    DoClearRegistersView();
    DebuggerEventData* ded = dynamic_cast<DebuggerEventData*>(e.GetClientObject());
    wxStringMap_t curvalues;
    if(ded) {
        // Split the list into 2
        size_t i = 0;
        while(i < ded->m_registers.size()) {
            RegistersViewModelClientData* cd = new RegistersViewModelClientData(false, false);

            wxVector<wxVariant> cols;
            cols.push_back(ded->m_registers.at(i).reg_name);
            cols.push_back(ded->m_registers.at(i).reg_value);
            if(!m_oldValues.empty()) {
                wxStringMap_t::iterator iter = m_oldValues.find(ded->m_registers.at(i).reg_name);
                if(iter != m_oldValues.end() && iter->second != ded->m_registers.at(i).reg_value) {
                    cd->SetFirstColModified(true);
                }
            }
            curvalues.insert(std::make_pair(ded->m_registers.at(i).reg_name, ded->m_registers.at(i).reg_value));
            m_dvListCtrlRegisters->AppendItem(cols, (wxUIntPtr)cd);
            ++i;
        }
    }
    m_oldValues.swap(curvalues);
}

void DebuggerDisassemblyTab::OnRefreshView(clCommandEvent& e)
{
    e.Skip();
    IDebugger* debugger = DebuggerMgr::Get().GetActiveDebugger();
    if(debugger && debugger->IsRunning() && ManagerST::Get()->DbgCanInteract()) {
        // Only update disass view if the view is visible
        if(ManagerST::Get()->IsDebuggerViewVisible(DebuggerPane::DISASSEMBLY)) {
            debugger->ListRegisters();
            debugger->Disassemble("", -1);
        }
    }
}

void DebuggerDisassemblyTab::DoClearRegistersView()
{
    m_dvListCtrlRegisters->DeleteAllItems([&](wxUIntPtr pData) {
        RegistersViewModelClientData* cd = reinterpret_cast<RegistersViewModelClientData*>(pData);
        wxDELETE(cd);
    });
}

wxArrayString DebuggerDisassemblyTab::GetRegisterNames() const
{
    wxArrayString names;
    for(size_t i = 0; i < m_dvListCtrlRegisters->GetItemCount(); ++i) {
        names.Add(m_dvListCtrlRegisters->GetItemText(m_dvListCtrlRegisters->RowToItem(i)));
    }
    return names;
}
