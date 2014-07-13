//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : generic_context.cpp
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
#include "generic_context.h"
#include "editor_config.h"
#include "cl_editor.h"

ContextGeneric::ContextGeneric(LEditor *container, const wxString &name)
    : ContextBase(container)
{
    SetName(name);
    ApplySettings();
}

ContextGeneric::~ContextGeneric()
{
}

ContextBase *ContextGeneric::NewInstance(LEditor *container)
{
    return new ContextGeneric(container, GetName());
}

void ContextGeneric::ApplySettings()
{
    LexerConf::Ptr_t lexPtr;
    if (EditorConfigST::Get()->IsOk()) {
        lexPtr = EditorConfigST::Get()->GetLexer(GetName());
    }
    LEditor &rCtrl = GetCtrl();
    if (lexPtr) {
        rCtrl.SetLexer(lexPtr->GetLexerId());
        for (int i = 0; i <= 4; ++i) {
            wxString keyWords = lexPtr->GetKeyWords(i);
            keyWords.Replace(wxT("\n"), wxT(" "));
            keyWords.Replace(wxT("\r"), wxT(" "));
            rCtrl.SetKeyWords(i, keyWords);
        }
    } else {
        rCtrl.SetLexer(wxSTC_LEX_NULL);
    }
    DoApplySettings(lexPtr);
}
