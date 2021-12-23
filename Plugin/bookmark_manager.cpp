//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : bookmark_manager.cpp
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

#include "bookmark_manager.h"

#include "editor_config.h"
#include "event_notifier.h"
#include "ieditor.h"
#include "plugin.h"
#include "wx/xrc/xmlres.h"

BookmarkManager::BookmarkManager()
    : m_activeBookmarkType(smt_bookmark1)
{
    wxCommandEvent dummy;
    OnEditorSettingsChanged(dummy);
    EventNotifier::Get()->Connect(wxEVT_EDITOR_SETTINGS_CHANGED,
                                  wxCommandEventHandler(BookmarkManager::OnEditorSettingsChanged), NULL, this);
}

BookmarkManager::~BookmarkManager()
{
    EventNotifier::Get()->Disconnect(wxEVT_EDITOR_SETTINGS_CHANGED,
                                     wxCommandEventHandler(BookmarkManager::OnEditorSettingsChanged), NULL, this);
}

BookmarkManager& BookmarkManager::Get()
{
    static BookmarkManager mgr;
    return mgr;
}

void BookmarkManager::OnEditorSettingsChanged(wxCommandEvent& e)
{
    e.Skip();
    DoPopulateDefaultLabels();
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    for(int i = smt_FIRST_BMK_TYPE; i <= smt_LAST_BMK_TYPE; ++i) {
        wxString new_label = options->GetBookmarkLabel(i - smt_FIRST_BMK_TYPE);
        new_label.Trim().Trim(false);

        if(!new_label.IsEmpty()) {
            m_markerLabels.erase(i);
            m_markerLabels.insert(std::make_pair(i, new_label));
        }
    }
}

wxString BookmarkManager::GetMarkerLabel(int index) const
{
    if(m_markerLabels.count(index)) {
        return m_markerLabels.find(index)->second;
    }
    return wxEmptyString;
}

wxMenu* BookmarkManager::CreateBookmarksSubmenu(wxMenu* parentMenu)
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("next_bookmark"), _("Next Bookmark"));
    menu->Append(XRCID("previous_bookmark"), _("Previous Bookmark"));
    menu->AppendSeparator();

    wxMenu* submenu = new wxMenu; // For the individual BM types
    static int bmktypes = smt_LAST_BMK_TYPE - smt_FIRST_BMK_TYPE + 1;

    int current = BookmarkManager::Get().GetActiveBookmarkType();
    wxCHECK_MSG(current >= smt_FIRST_BMK_TYPE && current < smt_find_bookmark, menu,
                "Out-of-range standard bookmarktype");

    for(int bmt = 1; bmt < bmktypes; ++bmt) { // Not <= as we don't want smt_find_bookmark here
        wxMenuItem* item = submenu->AppendRadioItem(XRCID("BookmarkTypes[start]") + bmt,
                                                    GetMarkerLabel((sci_marker_types)(smt_FIRST_BMK_TYPE + bmt - 1)));
        if(bmt == (current - smt_FIRST_BMK_TYPE + 1)) {
            item->Check();
        }
    }

    wxMenuItem* item = new wxMenuItem(menu, XRCID("change_active_bookmark_type"), _("Change Active Bookmark Type..."),
                                      "", wxITEM_NORMAL, submenu);
    menu->Append(item);

    menu->AppendSeparator();
    menu->Append(XRCID("removeall_current_bookmarks"), _("Remove All Currently-Active Bookmarks"));

    if(parentMenu) {
        item = new wxMenuItem(parentMenu, XRCID("more_bookmark_options"), _("More..."), "", wxITEM_NORMAL, menu);
        parentMenu->Append(item);
    }

    return menu;
}

void BookmarkManager::DoPopulateDefaultLabels()
{
    m_markerLabels.clear();
    for(int i = smt_FIRST_BMK_TYPE; i <= smt_LAST_BMK_TYPE; ++i) {
        wxString label;
        switch(i) {
        case smt_bookmark1:
            label << _("Normal bookmark");
            break;
        case smt_find_bookmark:
            label << _("Find bookmark");
            break;
        default:
            label << _("Bookmark Type") << ' ' << (i - smt_FIRST_BMK_TYPE + 1);
            break;
        }
        m_markerLabels.insert(std::make_pair(i, label));
    }
}
