//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : opentypevlistctrl.h
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

#ifndef __opentypevlistctrl__
#define __opentypevlistctrl__

#include "codelite_exports.h"
#include "entry.h"

#include <vector>
#include <wx/listctrl.h>

class WXDLLIMPEXP_SDK OpenTypeVListCtrl : public wxListView
{
    std::vector<TagEntryPtr> m_tags;

public:
    OpenTypeVListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
    virtual ~OpenTypeVListCtrl();

public:
    void SetItems(const std::vector<TagEntryPtr>& tags) { m_tags = tags; }

    virtual wxListItemAttr* OnGetItemAttr(long item) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual int OnGetItemImage(long item) const;
    virtual wxString OnGetItemText(long item, long column) const;
    int FindMatch(const wxString& word);
    TagEntryPtr GetTagAt(long item);
};

#endif // __opentypevlistctrl__
