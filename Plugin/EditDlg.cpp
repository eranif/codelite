//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : EditDlg.cpp
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

#include "EditDlg.h"
#include "lexer_configuration.h"
#include "editor_config.h"
#include "windowattrmanager.h"

EditDlg::EditDlg(wxWindow* parent, const wxString &text)
    : EditDlgBase(parent)
{
    LexerConf::Ptr_t lex = EditorConfigST::Get()->GetLexer("text");
    lex->Apply( m_stc10 );
    m_stc10->SetText( text );
    WindowAttrManager::Load(this, "EditDlg");
}

EditDlg::~EditDlg()
{
    WindowAttrManager::Save(this, "EditDlg");
}


wxString clGetTextFromUser(const wxString &initialValue, wxWindow* parent)
{
    EditDlg dlg(parent, initialValue);
    if ( dlg.ShowModal() == wxID_OK ) {
        return dlg.GetText();
    }
    return wxEmptyString;
}
