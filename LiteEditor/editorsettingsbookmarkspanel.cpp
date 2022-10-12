//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsbookmarkspanel.cpp
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

#include "editorsettingsbookmarkspanel.h"

#include "globals.h"

EditorSettingsBookmarksPanel::EditorSettingsBookmarksPanel(wxWindow* parent, OptionsConfigPtr options)
    : OptionsConfigPage(parent, options)
{
    AddHeader(_("Symbol margin"));
    AddProperty(_("Display the symbol margin"), m_options->GetDisplayBookmarkMargin(),
                UPDATE_BOOL_CB(SetDisplayBookmarkMargin));

    AddHeader(_("Bookmarks"));
    const std::vector<wxString> UnlocalisedShapes = { wxT("Small Rectangle"), wxT("Rounded Rectangle"), wxT("Circle"),
                                                      wxT("Small Arrow"), wxT("Bookmark") };

    AddProperty(_("Shape"), UnlocalisedShapes, m_options->GetBookmarkShape(), UPDATE_TEXT_CB(SetBookmarkShape));

    for(size_t n = 0; n < CL_N0_OF_BOOKMARK_TYPES; ++n) {
        wxColour col = m_options->GetBookmarkBgColour(n);
        if(!col.IsOk()) {
            col = "LIGHT BLUE";
        }
        wxString name = wxString() << _("Bookmark #") << n;
        auto update_colours = [this, n](const wxString&, const wxAny& value) {
            wxColour colour_value;
            if(value.GetAs(&colour_value)) {
                m_options->SetBookmarkBgColour(colour_value, n);
                m_options->SetBookmarkFgColour(colour_value, n);
            }
        };
        auto update_label = [this, n](const wxString&, const wxAny& value) {
            wxString str_value;
            if(value.GetAs(&str_value)) {
                m_options->SetBookmarkLabel(str_value, n);
            }
        };
        AddProperty(name + _(" colour"), col, update_colours);
        AddProperty(name + _(" label"), m_options->GetBookmarkLabel(n), update_label);
    }

    AddHeader(_("Highlight matches"));
    AddProperty(_("Colour"), wxColour(EditorConfigST::Get()->GetString(wxT("WordHighlightColour"))),
                [](const wxString&, const wxAny& value) {
                    wxColour colour_value;
                    if(value.GetAs(&colour_value)) {
                        EditorConfigST::Get()->SetString(wxT("WordHighlightColour"),
                                                         colour_value.GetAsString(wxC2S_HTML_SYNTAX));
                    }
                });
    AddProperty(_("Find/Find Next clears highlit matching words"), m_options->GetClearHighlitWordsOnFind(),
                UPDATE_BOOL_CB(SetClearHighlitWordsOnFind));
}
