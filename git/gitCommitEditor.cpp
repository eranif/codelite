//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : gitCommitEditor.cpp
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

#include "gitCommitEditor.h"

#include "drawingutils.h"
#include "lexer_configuration.h"
#include "editor_config.h"

GitCommitEditor::GitCommitEditor(wxWindow* parent,
                                 wxWindowID id,
                                 const wxPoint& position,
                                 const wxSize& size,
                                 long style)
    : wxStyledTextCtrl(parent, id, position, size, style | wxBORDER_THEME)
{
    InitStyles();
}

void GitCommitEditor::InitStyles()
{
    LexerConf::Ptr_t diffLexer = EditorConfigST::Get()->GetLexer("Diff");
    if(diffLexer) {
        diffLexer->Apply(this);
        this->SetLexer(wxSTC_LEX_DIFF);
    }
}
