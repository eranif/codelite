//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsfolding.cpp
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

#include "editorsettingsfolding.h"

#include "globals.h"

EditorSettingsFolding::EditorSettingsFolding(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    const std::vector<wxString> fold_styles = { wxT("Simple"), wxT("Arrows"), wxT("Flatten Tree Square Headers"),
                                                wxT("Flatten Tree Circular Headers") };

    AddHeader(_("Folding"));
    AddProperty(_("Show folding margin"), m_options->GetDisplayFoldMargin(), UPDATE_BOOL_CB(SetDisplayFoldMargin));
    AddProperty(_("Appearance"), fold_styles, m_options->GetFoldStyle(), [&](const wxString&, const wxAny& value) {
        wxString value_str;
        if(value.GetAs(&value_str)) {
            m_options->SetFoldStyle(value_str);
        }
    });
    AddHeader(_("Folding options"));
    AddProperty(_("Underline folded line"), m_options->GetUnderlineFoldLine(), UPDATE_BOOL_CB(SetUnderlineFoldLine));
    AddProperty(_("Fold `else` blocks"), m_options->GetFoldAtElse(), UPDATE_BOOL_CB(SetFoldAtElse));
    AddProperty(_("Fold pre-processor blocks"), m_options->GetFoldPreprocessor(), UPDATE_BOOL_CB(SetFoldPreprocessor));
    AddProperty(_("Fold compact"), m_options->GetFoldCompact(), UPDATE_BOOL_CB(SetFoldCompact));
    AddProperty(_("Highlight active fold block"), m_options->IsHighlightFoldWhenActive(),
                UPDATE_BOOL_CB(SetHighlightFoldWhenActive));
}
