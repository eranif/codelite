//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : debuggerasciiviewer.cpp
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

#include "debuggerasciiviewer.h"

#include "debugger.h"
#include "debuggerpane.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "frame.h"
#include "lexer_configuration.h"
#include "manager.h"
#include "plugin.h"

static void sDefineMarker(wxStyledTextCtrl *s, int marker, int markerType, wxColor fore, wxColor back)
{
    s->MarkerDefine(marker, markerType);
    s->MarkerSetForeground(marker, fore);
    s->MarkerSetBackground(marker, back);
}

DebuggerAsciiViewer::DebuggerAsciiViewer( wxWindow* parent )
    : DebuggerAsciiViewerBase( parent )
{
    EventNotifier::Get()->Connect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(DebuggerAsciiViewer::OnThemeColourChanged), NULL, this);
    LexerConf::Ptr_t cpp_lexer = EditorConfigST::Get()->GetLexer("C++");
    if ( cpp_lexer ) {
        cpp_lexer->Apply( m_textView );
        m_textView->SetLexer(wxSTC_LEX_CPP);
        
    } else {
        // This code should not be called at all...
        // but still - just in case
        wxFont font(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        // hide all margins
        m_textView->SetMarginWidth(0, 0);
        m_textView->SetMarginWidth(1, 0);
        m_textView->SetMarginWidth(2, 0);
        m_textView->SetMarginWidth(3, 0);
        m_textView->SetMarginWidth(4, 0);

        m_textView->SetMarginSensitive(4, true);
        m_textView->SetProperty(wxT("fold"), wxT("1"));

        sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));
        sDefineMarker(m_textView, wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, wxColor(0xff, 0xff, 0xff), wxColor(0x80, 0x80, 0x80));

        // set wrapped line indicator
        m_textView->SetWrapVisualFlags(1);
        m_textView->SetWrapStartIndent(4);
        m_textView->SetScrollWidthTracking(true);
        m_textView->StyleSetForeground(wxSTC_STYLE_DEFAULT, wxT("GREY"));

        // Set wrap mode on
        m_textView->SetWrapMode(wxSTC_WRAP_WORD);
        // Use NULL lexer
        m_textView->SetLexer(wxSTC_LEX_CPP);
        m_textView->SetMarginMask(4, wxSTC_MASK_FOLDERS);

        // Set TELETYPE font (monospace)
        m_textView->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
        m_textView->StyleSetSize(wxSTC_STYLE_DEFAULT, 12  );
    }
    
    m_textView->SetReadOnly(true);

    wxTheApp->Connect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerAsciiViewer::OnEdit),   NULL, this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerAsciiViewer::OnEdit),   NULL, this);
    wxTheApp->Connect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerAsciiViewer::OnEditUI), NULL, this);
    wxTheApp->Connect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerAsciiViewer::OnEditUI), NULL, this);
}

bool DebuggerAsciiViewer::IsFocused()
{
    wxWindow *win = wxWindow::FindFocus();
    return (win && win == m_textView);
}

void DebuggerAsciiViewer::UpdateView(const wxString &expr, const wxString &value)
{
    m_textCtrlExpression->SetValue(expr);

    wxString evaluated (value);
    evaluated.Replace(wxT("\r\n"), wxT("\n"));
    evaluated.Replace(wxT("\n,"), wxT(",\n"));
    evaluated.Replace(wxT("\n\n"), wxT("\n"));

    m_textView->SetReadOnly(false);
    m_textView->ClearAll();
    m_textView->SetText(evaluated);
    m_textView->SetReadOnly(true);

}

void DebuggerAsciiViewer::OnClearView(wxCommandEvent& e)
{
    wxUnusedVar(e);
    UpdateView(wxT(""), wxT(""));
}


void DebuggerAsciiViewer::OnEditUI(wxUpdateUIEvent& e)
{
    if ( !IsFocused() ) {
        e.Skip();
        return;
    }
    switch ( e.GetId() ) {
    case wxID_SELECTALL:
        e.Enable(true);
        break;
    case wxID_COPY:
        e.Enable( m_textView->GetSelectedText().IsEmpty() == false );
        break;
    default:
        e.Enable(false);
        break;
    }
}

void DebuggerAsciiViewer::OnEdit(wxCommandEvent& e)
{
    if ( !IsFocused() ) {
        e.Skip();
        return;
    }

    switch ( e.GetId() ) {
    case wxID_SELECTALL:
        m_textView->SelectAll();
        break;
    case wxID_COPY:
        m_textView->Copy();
        break;
    default:
        break;
    }
}

DebuggerAsciiViewer::~DebuggerAsciiViewer()
{
    wxTheApp->Disconnect(wxID_COPY,      wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerAsciiViewer::OnEdit),   NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(DebuggerAsciiViewer::OnEdit),   NULL, this);
    wxTheApp->Disconnect(wxID_COPY,      wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerAsciiViewer::OnEditUI), NULL, this);
    wxTheApp->Disconnect(wxID_SELECTALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(DebuggerAsciiViewer::OnEditUI), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CL_THEME_CHANGED, wxCommandEventHandler(DebuggerAsciiViewer::OnThemeColourChanged), NULL, this);
}

void DebuggerAsciiViewer::OnThemeColourChanged(wxCommandEvent& e)
{
    e.Skip();
    
    // Re-apply C++ lexer
    LexerConf::Ptr_t cpp_lexer = EditorConfigST::Get()->GetLexer("C++");
    if ( cpp_lexer ) {
        cpp_lexer->Apply( m_textView );
    }
}
