//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : opentypevlistctrl.cpp
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

#include "opentypevlistctrl.h"

OpenTypeVListCtrl::OpenTypeVListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                     long style)
    : wxListView(parent, id, pos, size, style)
{
}

OpenTypeVListCtrl::~OpenTypeVListCtrl() {}

wxListItemAttr* OpenTypeVListCtrl::OnGetItemAttr(long item) const { return NULL; }

int OpenTypeVListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if(item >= (long)m_tags.size()) {
        return wxNOT_FOUND;
    }

    if(column > 0) {
        return wxNOT_FOUND;
    }

    wxString kind = m_tags.at(item)->GetKind();
    if(kind == wxT("namespace"))
        return 0;
    if(kind == wxT("class"))
        return 1;
    if(kind == wxT("struct"))
        return 2;
    if(kind == wxT("typedef"))
        return 3;
    if(kind == wxT("enum"))
        return 4;
    if(kind == wxT("union"))
        return 2;
    return 1;
}

int OpenTypeVListCtrl::OnGetItemImage(long item) const
{
    if(item >= (long)m_tags.size()) {
        return wxNOT_FOUND;
    }

    wxString kind = m_tags.at(item)->GetKind();

    if(kind == wxT("namespace"))
        return 0;
    if(kind == wxT("class"))
        return 1;
    if(kind == wxT("struct"))
        return 2;
    if(kind == wxT("typedef"))
        return 3;
    if(kind == wxT("enum"))
        return 4;
    if(kind == wxT("union"))
        return 2;
    return 1;
}

wxString OpenTypeVListCtrl::OnGetItemText(long item, long column) const
{
    if(item >= (long)m_tags.size()) {
        return wxEmptyString;
    }
    TagEntryPtr t = m_tags.at(item);

    switch(column) {
    case 0: // name
        return t->GetName();
    case 1: // scope
        return t->GetScope();
    case 2: // file
        return t->GetFile();
    case 3: // line
    {
        wxString l;
        l << t->GetLine();
        return l;
    }
    default:
        return wxEmptyString;
    }
    return wxEmptyString;
}

int OpenTypeVListCtrl::FindMatch(const wxString& word)
{
    // first try to match case sensetive
    int possible_match(wxNOT_FOUND);

    // full match
    for(size_t i = 0; i < m_tags.size(); i++) {
        TagEntryPtr t = m_tags.at(i);

        wxString s1(word);
        wxString s2(t->GetName());

        // first try full match
        if(s2.StartsWith(s1)) {
            if(possible_match == wxNOT_FOUND) {
                possible_match = (int)i;
            }

            if(s2 == s1) {
                return (int)i;
            }
        } else {
            if(possible_match != wxNOT_FOUND) {
                // since the tags are sorted, we will not find any matches beyond this point
                return possible_match;
            }
        }
    }

    // if we reached this part, possible_match is wxNOT_FOUND
    for(size_t i = 0; i < m_tags.size(); i++) {
        TagEntryPtr t = m_tags.at(i);

        wxString s1(word);
        wxString s2(t->GetName());
        s1.MakeLower();
        s2.MakeLower();

        // first try full match
        if(s2.StartsWith(s1)) {
            if(possible_match == wxNOT_FOUND) {
                possible_match = (int)i;
            }

            if(s2 == s1) {
                return (int)i;
            }
        } else {
            if(possible_match != wxNOT_FOUND) {
                // we will not find any matches beyond this point
                return possible_match;
            }
        }
    }

    return wxNOT_FOUND;
}

TagEntryPtr OpenTypeVListCtrl::GetTagAt(long item)
{
    if(item >= (long)m_tags.size()) {
        return NULL;
    }
    return m_tags.at(item);
}
