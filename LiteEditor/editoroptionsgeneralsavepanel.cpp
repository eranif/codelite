//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editoroptionsgeneralsavepanel.cpp
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

#include "editoroptionsgeneralsavepanel.h"

EditorOptionsGeneralSavePanel::EditorOptionsGeneralSavePanel(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddHeader(_("Trim lines"));
    bool trim_empty_lines = EditorConfigST::Get()->GetInteger(wxT("EditorTrimEmptyLines"), 0) ? true : false;
    AddProperty(_("Enable trim on save"), trim_empty_lines, [&](const wxString& label, const wxAny& value) {
        wxUnusedVar(label);
        bool value_bool;
        if(value.GetAs(&value_bool)) {
            EditorConfigST::Get()->SetInteger("EditorTrimEmptyLines", value_bool ? 1 : 0);
        }
    });

    bool do_not_trim_caret_line = EditorConfigST::Get()->GetInteger(wxT("DontTrimCaretLine"), 0) ? true : false;
    AddProperty(_("Do not trim the caret line"), do_not_trim_caret_line,
                [&](const wxString& label, const wxAny& value) {
                    wxUnusedVar(label);
                    bool value_bool;
                    if(value.GetAs(&value_bool)) {
                        EditorConfigST::Get()->SetInteger("DontTrimCaretLine", value_bool ? 1 : 0);
                    }
                });

    AddProperty(_("Trim modified lines only"), m_options->GetTrimOnlyModifiedLines(),
                UPDATE_BOOL_CB(SetTrimOnlyModifiedLines));
    AddHeader(_("Misc"));
    bool append_eol_if_missing = EditorConfigST::Get()->GetInteger(wxT("EditorAppendLf"), 0) ? true : false;
    AddProperty(_("If missing, append EOL at the end of file"), append_eol_if_missing,
                [&](const wxString& label, const wxAny& value) {
                    wxUnusedVar(label);
                    bool value_bool;
                    if(value.GetAs(&value_bool)) {
                        EditorConfigST::Get()->SetInteger("EditorAppendLf", value_bool ? 1 : 0);
                    }
                });
}
