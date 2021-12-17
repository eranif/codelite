//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_diff.cpp
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
#include "context_diff.h"
#include "cl_editor.h"
#include "editor_config.h"
#include "frame.h"
#include "manager.h"
#include <wx/regex.h>
#include <wx/xrc/xmlres.h>

ContextDiff::ContextDiff()
    : ContextBase(wxT("Diff"))
{
}

ContextDiff::ContextDiff(clEditor* container)
    : ContextBase(container)
{
    SetName(wxT("diff"));
    ApplySettings();
}

ContextDiff::~ContextDiff() {}

ContextBase* ContextDiff::NewInstance(clEditor* container) { return new ContextDiff(container); }

void ContextDiff::ApplySettings()
{
    LexerConf::Ptr_t lexPtr;
    if(EditorConfigST::Get()->IsOk()) {
        lexPtr = EditorConfigST::Get()->GetLexer(GetName());
    }
    GetCtrl().SetLexer(lexPtr ? lexPtr->GetLexerId() : wxSTC_LEX_NULL);
    DoApplySettings(lexPtr);
}
