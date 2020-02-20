//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : wxterminalbase.cpp
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

///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ColoursAndFontsManager.h"
#include "lexer_configuration.h"
#include "wxterminalbase.h"

///////////////////////////////////////////////////////////////////////////

#define MARKER_ID 1

wxTerminalBase::wxTerminalBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxPanel(parent, id, pos, size, style)
{
    wxBoxSizer* mainSizer;
    mainSizer = new wxBoxSizer(wxVERTICAL);

    m_textCtrl = new wxStyledTextCtrl(this);
    m_textCtrl->SetMarginType(2, wxSTC_MARGIN_SYMBOL);
    m_textCtrl->SetMarginMask(2, ~(wxSTC_MASK_FOLDERS));
    m_textCtrl->SetMarginWidth(2, 4);
    m_textCtrl->SetMarginSensitive(2, true);
    m_textCtrl->MarkerDefine(MARKER_ID, wxSTC_MARK_ARROWS);
    m_textCtrl->MarkerAdd(0, MARKER_ID);

    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) { lexer->Apply(m_textCtrl); }
    mainSizer->Add(m_textCtrl, 1, wxEXPAND, 0);
    this->SetSizer(mainSizer);
    this->Layout();
    mainSizer->Fit(this);

    // Connect Events
    m_textCtrl->Bind(wxEVT_KEY_DOWN, &wxTerminalBase::OnKey, this);
    m_textCtrl->Bind(wxEVT_STC_CHARADDED, &wxTerminalBase::OnCharAdded, this);
    m_textCtrl->Bind(wxEVT_LEFT_UP, [&](wxMouseEvent& event) {
        if(m_textCtrl->GetSelectedText().IsEmpty()) { this->CallAfter(&wxTerminalBase::CaretToEnd); }
        event.Skip();
    });
    m_textCtrl->SetReadOnly(true);
}

wxTerminalBase::~wxTerminalBase()
{
    m_textCtrl->Unbind(wxEVT_KEY_DOWN, &wxTerminalBase::OnKey, this);
    m_textCtrl->Unbind(wxEVT_STC_CHARADDED, &wxTerminalBase::OnCharAdded, this);
}

void wxTerminalBase::OnKey(wxKeyEvent& event)
{
    bool isLastLine = (m_textCtrl->LineFromPosition(m_textCtrl->GetCurrentPos()) == (m_textCtrl->GetLineCount() - 1));
    m_textCtrl->SetReadOnly(!isLastLine);
    switch(event.GetKeyCode()) {
    case WXK_BACK: {
        if(m_textCtrl->GetColumn(m_textCtrl->GetCurrentPos()) == 0) {
            return;
        } else {
            event.Skip();
        }
        break;
    }
    case WXK_DOWN:
        OnDown(event);
        break;
    case WXK_UP:
        OnUp(event);
        break;
    case WXK_RIGHT:
        OnRight(event);
        break;
    case WXK_LEFT:
        OnLeft(event);
        break;
    case 'C':
    case 'c':
        if(event.GetModifiers() == wxMOD_RAW_CONTROL) {
            OnCtrlC(event);
            event.Skip();
        } else {
            event.Skip();
        }
        break;
    default:
        event.Skip();
    }
}

void wxTerminalBase::OnCharAdded(wxStyledTextEvent& event)
{
    event.Skip();
    if(event.GetKey() == '\n') {
        AddMarker();
        OnEnter();
    }
}

void wxTerminalBase::AddMarker()
{
    int lastLine = m_textCtrl->LineFromPosition(m_textCtrl->GetLastPosition());
    // m_textCtrl->MarkerDeleteAll(MARKER_ID);
    m_textCtrl->MarkerAdd(lastLine, MARKER_ID);
}

void wxTerminalBase::CaretToEnd()
{
    m_textCtrl->ScrollToEnd();
    m_textCtrl->ClearSelections();
    m_textCtrl->GotoPos(m_textCtrl->GetLength());
    AddMarker();
}
