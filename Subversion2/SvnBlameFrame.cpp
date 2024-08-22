//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : SvnBlameFrame.cpp
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

#include "SvnBlameFrame.h"

#include "editor_config.h"
#include "lexer_configuration.h"
#include "windowattrmanager.h"

SvnBlameFrame::SvnBlameFrame(wxWindow* parent, const wxFileName& filename, const wxString& content)
    : SvnBlameFrameBase(parent)
    , m_filename(filename)
{
    // Now that the base class is done with its initialization
    // do our custom initialization to the blame editor

    m_stc->SetText(content);
    m_stc->SetReadOnly(true);
    LexerConf::Ptr_t lexer = EditorConfigST::Get()->GetLexerForFile(filename.GetFullName());
    if(lexer) {
        lexer->Apply(m_stc, true);
    }
    m_stc->Initialize();
    SetName("SvnBlameFrame");
    WindowAttrManager::Load(this);
}

SvnBlameFrame::~SvnBlameFrame() {}
