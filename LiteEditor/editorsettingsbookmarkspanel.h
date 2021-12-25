//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : editorsettingsbookmarkspanel.h
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

#ifndef __editorsetthingsbookmarkspanel__
#define __editorsetthingsbookmarkspanel__

#include "editorsettingsbookmarksbasepanel.h"
#include "globals.h"
#include "treebooknodebase.h"

#include <vector>

struct BookmarkData {
    wxColour fg;
    wxColour bg;
    wxString label;
    wxString defaultLabel;
};

class EditorSettingsBookmarksPanel : public EditorSettingsBookmarksBasePanel,
                                     public TreeBookNode<EditorSettingsBookmarksPanel>
{
    StringManager m_stringManager;
    std::vector<BookmarkData> m_bookmarksData;
    int m_previous;

public:
    EditorSettingsBookmarksPanel(wxWindow* parent);
    void Save(OptionsConfigPtr options);

protected:
    virtual void OnBookmarkChanged(wxCommandEvent& event);
    void ChangeSelection(int index);
};

#endif // __editorsetthingsbookmarkspanel__
