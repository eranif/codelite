//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : bookmark_manager.h
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

#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include <wx/arrstr.h>
#include <wx/event.h>
#include "codelite_exports.h"
#include <map>
#include <vector>
#include "cl_defs.h"
#include <wx/menu.h>
#include <wx/stc/stc.h>
#include <unordered_map>
#include "ieditor.h"

#define MAX_BOOKMARK CL_N0_OF_BOOKMARK_TYPES

// NB The following are sci markers, which are zero based. So smt_bookmark is actually the eighth of them (important
// when masking it!)
// If you add another type here, watch out for smt_LAST_BP_TYPE; and you need also to add to the enum 'marker_mask_type'
// below
// The higher the value, the nearer the top of the pecking order displaywise. So keep the most important breakpoint at
// the top i.e. smt_breakpoint,
// but have smt_breakpointsmt_indicator above it, so you can see the indicator when there's a breakpt too
enum sci_marker_types { /* markers 0-2 are unused atm */
                        smt_FIRST_BMK_TYPE = 3,
                        smt_LAST_BMK_TYPE = smt_FIRST_BMK_TYPE + CL_N0_OF_BOOKMARK_TYPES - 1,
                        smt_bookmark1 = smt_FIRST_BMK_TYPE,
                        smt_bookmark2,
                        smt_bookmark3,
                        smt_bookmark4,
                        smt_find_bookmark = smt_LAST_BMK_TYPE,
                        smt_FIRST_BP_TYPE = 8,
                        smt_cond_bp_disabled = smt_FIRST_BP_TYPE,
                        smt_bp_cmdlist_disabled,
                        smt_bp_disabled,
                        smt_bp_ignored,
                        smt_cond_bp,
                        smt_bp_cmdlist,
                        smt_breakpoint,
                        smt_LAST_BP_TYPE = smt_breakpoint,
                        smt_indicator,
                        smt_warning,
                        smt_error
};

// These are bitmap masks of the various margin markers.
// So 256 == 0x100 == 100000000, 2^9, and masks the ninth marker, smt_cond_bp_disabled==8 (as the markers are
// zero-based)
// 0x7f00 is binary 111111100000000 and masks all the 7 current breakpoint types. If you add others, change it
enum marker_mask_type {
    mmt_standard_bookmarks = 0x78,
    mmt_find_bookmark = 0x80,
    mmt_all_bookmarks = 0xF8,
    mmt_FIRST_BP_TYPE = 0x100,
    mmt_cond_bp_disabled = mmt_FIRST_BP_TYPE,
    mmt_bp_cmdlist_disabled = 0x200,
    mmt_bp_disabled = 0x400,
    mmt_bp_ignored = 0x800,
    mmt_cond_bp = 0x1000,
    mmt_bp_cmdlist = 0x2000,
    mmt_breakpoint = 0x4000,
    mmt_LAST_BP_TYPE = mmt_breakpoint,
    mmt_all_breakpoints = 0x7f00,
    mmt_indicator = 0x8000,
    mmt_compiler = 0x30000 /* masks compiler errors/warnings */,
    mmt_folds = wxSTC_MASK_FOLDERS /* 0xFE000000 */
};

class WXDLLIMPEXP_SDK BookmarkManager : public wxEvtHandler
{
public:
    typedef std::unordered_map<int, wxString> Map_t;

private:
    int m_activeBookmarkType;
    BookmarkManager::Map_t m_markerLabels;

private:
    BookmarkManager();
    virtual ~BookmarkManager();

protected:
    void OnEditorSettingsChanged(wxCommandEvent& e);
    void DoPopulateDefaultLabels();

public:
    static BookmarkManager& Get();

    void SetActiveBookmarkType(int activeBookmarkType) { this->m_activeBookmarkType = activeBookmarkType; }
    int GetActiveBookmarkType() const { return m_activeBookmarkType; }

    wxString GetMarkerLabel(int index) const;
    wxMenu* CreateBookmarksSubmenu(wxMenu* parentMenu);
};

#endif // BOOKMARKMANAGER_H
