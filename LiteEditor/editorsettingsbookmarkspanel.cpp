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

#include "globals.h"
#include "editorsettingsbookmarkspanel.h"

EditorSettingsBookmarksPanel::EditorSettingsBookmarksPanel(wxWindow* parent)
    : EditorSettingsBookmarksBasePanel(parent)
    , TreeBookNode<EditorSettingsBookmarksPanel>()
    , m_previous(wxNOT_FOUND)
{
    m_highlightColor->SetColour(wxString(wxT("LIGHT BLUE")));

    // get the editor's options from the disk
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    m_displaySelection->SetValue(options->GetDisplayBookmarkMargin());

    // These are localised inside m_bookMarkShape. However serialising the translated strings will break other
    // locales...
    const wxString UnlocalisedShapes[] = { wxTRANSLATE("Small Rectangle"), wxTRANSLATE("Rounded Rectangle"),
                                           wxTRANSLATE("Circle"), wxTRANSLATE("Small Arrow"), wxTRANSLATE("Bookmark") };
    m_stringManager.AddStrings(sizeof(UnlocalisedShapes) / sizeof(wxString), UnlocalisedShapes,
                               options->GetBookmarkShape(), m_bookMarkShape);

    for(size_t n = 0; n < CL_N0_OF_BOOKMARK_TYPES; ++n) {
        BookmarkData arr;
        wxColour col = options->GetBookmarkBgColour(n);
        if(!col.IsOk()) { col = options->GetBookmarkBgColour(0); }
        arr.bg = col;

        col = options->GetBookmarkFgColour(n);
        if(!col.IsOk()) { col = options->GetBookmarkFgColour(0); }
        arr.fg = col;

        arr.defaultLabel = m_choiceBMType->GetString(n); // Store the default  value
        arr.label = options->GetBookmarkLabel(n);
        if(!arr.label.empty()) {
            wxCHECK_RET(n < m_choiceBMType->GetCount(), "More bookmark types than there are Choice entries");
            m_choiceBMType->SetString(n, arr.label);
        }

        m_bookmarksData.push_back(arr);
    }
    ChangeSelection(0); // Fake a change to display the standard-bookmark values

    wxString val1 = EditorConfigST::Get()->GetString(wxT("WordHighlightColour"));
    if(val1.IsEmpty() == false) { m_highlightColor->SetColour(val1); }

    long alpha = EditorConfigST::Get()->GetInteger(wxT("WordHighlightAlpha"));
    if(alpha != wxNOT_FOUND) { m_spinCtrlHighlightAlpha->SetValue(alpha); }
    m_clearHighlitWords->SetValue(options->GetClearHighlitWordsOnFind());
}

void EditorSettingsBookmarksPanel::Save(OptionsConfigPtr options)
{
    options->SetDisplayBookmarkMargin(m_displaySelection->IsChecked());

    // Get the bookmark selection, unlocalised
    wxString bmShape = m_stringManager.GetStringSelection();
    if(bmShape.IsEmpty()) { bmShape = wxT("Bookmark"); }
    options->SetBookmarkShape(bmShape);

    ChangeSelection(m_choiceBMType->GetSelection()); // Fake a change to store any altered values

    for(size_t n = 0; n < CL_N0_OF_BOOKMARK_TYPES; ++n) {
        BookmarkData& arr = m_bookmarksData.at(n);
        options->SetBookmarkBgColour(arr.bg, n);
        options->SetBookmarkFgColour(arr.fg, n);
        options->SetBookmarkLabel(arr.label, n);
    }

    EditorConfigST::Get()->SetString(wxT("WordHighlightColour"), m_highlightColor->GetColour().GetAsString());
    EditorConfigST::Get()->SetInteger(wxT("WordHighlightAlpha"), (long)m_spinCtrlHighlightAlpha->GetValue());
    options->SetClearHighlitWordsOnFind(m_clearHighlitWords->IsChecked());
}

void EditorSettingsBookmarksPanel::ChangeSelection(int index)
{
    wxCHECK_RET(index < CL_N0_OF_BOOKMARK_TYPES, "Bookmark type out of range");

    if(m_previous >= 0) {
        // Store the current data in case it was just altered
        BookmarkData& arr = m_bookmarksData.at(m_previous);
        arr.bg = m_backgroundColor->GetColour();
        arr.fg = m_foregroundColor->GetColour();
        arr.label = m_BookmarkLabel->GetValue();
        if(m_BookmarkLabel->GetValue().empty()) {
            m_choiceBMType->SetString(m_previous, arr.defaultLabel);
        } else {
            m_choiceBMType->SetString(m_previous, arr.label);
        }
    }

    BookmarkData arr = m_bookmarksData.at(index);
    m_backgroundColor->SetColour(arr.bg);
    m_foregroundColor->SetColour(arr.fg);
    m_BookmarkLabel->ChangeValue(arr.label);

    m_previous = index;
}

void EditorSettingsBookmarksPanel::OnBookmarkChanged(wxCommandEvent& event) { ChangeSelection(event.GetSelection()); }
