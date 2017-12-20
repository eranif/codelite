//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : wxcl_log_text_ctrl.cpp
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
#include "drawingutils.h"
#include "lexer_configuration.h"
#include "wxcl_log_text_ctrl.h"
#include <wx/settings.h>

wxclTextCtrl::wxclTextCtrl(wxStyledTextCtrl* stc)
    : m_stc(stc)
{
    DoInit();
}

wxclTextCtrl::~wxclTextCtrl() {}

void wxclTextCtrl::Reset() { DoInit(); }

void wxclTextCtrl::DoInit()
{
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) { lexer->Apply(m_stc); }
}

void wxclTextCtrl::DoLogText(const wxString& msg)
{
    m_stc->AppendText(msg + wxS("\n"));
    m_stc->SetSelectionEnd(m_stc->GetLength());
    m_stc->SetSelectionStart(m_stc->GetLength());
    m_stc->SetCurrentPos(m_stc->GetLength());
    m_stc->EnsureCaretVisible();
}
