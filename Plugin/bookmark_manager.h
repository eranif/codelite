#ifndef BOOKMARKMANAGER_H
#define BOOKMARKMANAGER_H

#include <wx/arrstr.h>
#include <wx/event.h>
#include "codelite_exports.h"
#include <map>
#include "cl_defs.h"
#include <wx/menu.h>

#define MAX_BOOKMARK CL_N0_OF_BOOKMARK_TYPES

// NB The following are sci markers, which are zero based. So smt_bookmark is actually the eighth of them (important when masking it!)
// If you add another type here, watch out for smt_LAST_BP_TYPE; and you need also to add to the enum 'marker_mask_type' below
// The higher the value, the nearer the top of the pecking order displaywise. So keep the most important breakpoint at the top i.e. smt_breakpoint,
// but have smt_breakpointsmt_indicator above it, so you can see the indicator when there's a breakpt too
enum sci_marker_types { /* markers 0-2 are unused atm */
    smt_FIRST_BMK_TYPE=3, smt_LAST_BMK_TYPE=smt_FIRST_BMK_TYPE + CL_N0_OF_BOOKMARK_TYPES - 1,
    smt_bookmark1=smt_FIRST_BMK_TYPE, smt_bookmark2, smt_bookmark3, smt_bookmark4, smt_find_bookmark=smt_LAST_BMK_TYPE,
    smt_FIRST_BP_TYPE=8, smt_cond_bp_disabled = smt_FIRST_BP_TYPE, smt_bp_cmdlist_disabled, smt_bp_disabled, smt_bp_ignored,
    smt_cond_bp, smt_bp_cmdlist, smt_breakpoint, smt_LAST_BP_TYPE = smt_breakpoint,
    smt_indicator, smt_warning, smt_error
};

class WXDLLIMPEXP_SDK BookmarkManager : public wxEvtHandler
{
public:
    typedef std::map<int, wxString> Map_t;
    
private:
    int           m_activeBookmarkType;
    BookmarkManager::Map_t m_markerLabels;

private:
    BookmarkManager();
    virtual ~BookmarkManager();
    
protected:
    void OnEditorSettingsChanged(wxCommandEvent &e);
    void DoPopulateDefaultLabels();
    
public:
    static BookmarkManager& Get();

    void SetActiveBookmarkType(int activeBookmarkType) {
        this->m_activeBookmarkType = activeBookmarkType;
    }
    int GetActiveBookmarkType() const {
        return m_activeBookmarkType;
    }

    wxString GetMarkerLabel(int index) const;
    
    wxMenu* CreateBookmarksSubmenu(wxMenu* parentMenu);
};

#endif // BOOKMARKMANAGER_H
