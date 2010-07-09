//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : ccvirtuallistctrl.cpp
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
#include "ccvirtuallistctrl.h"
#include <wx/settings.h>
CCVirtualListCtrl::CCVirtualListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
		: wxListView(parent, id, pos, size, style)
{
}

CCVirtualListCtrl::~CCVirtualListCtrl()
{
}

wxListItemAttr* CCVirtualListCtrl::OnGetItemAttr(long item) const
{
#if defined(__WXMSW__)
	static wxListItemAttr attr;
	static bool           first(true);

	if(first) {
		first = false;
		wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		fnt.SetWeight(wxBOLD);
		attr.SetFont(fnt);
	}

	if (GetFirstSelected() == item) {
		return &attr;

	} else
		return NULL;
#else // Mac
	return NULL;
#endif
}

int CCVirtualListCtrl::OnGetItemColumnImage(long item, long column) const
{
	wxUnusedVar(column);
	return OnGetItemImage(item);
}

int CCVirtualListCtrl::OnGetItemImage(long item) const
{
	CCItemInfo info;
	if (item >= (long)m_tags.size()) {
		return wxNOT_FOUND;
	}
	info = m_tags.at(item);
	return info.imgId;
}

wxString CCVirtualListCtrl::OnGetItemText(long item, long column) const
{
	CCItemInfo info;
	if (item >= (long)m_tags.size()) {
		return wxEmptyString;
	}
	info = m_tags.at(item);
	return info.displayName;
}

int CCVirtualListCtrl::FindMatch(const wxString& word, bool &fullMatch)
{
	fullMatch = false;
	int  firstMatch(-1);
	int  exactMatch(-1);
	int  matchCount(0 );
	// first try to match case sensetive
	for (size_t i=0; i<m_tags.size(); i++) {

		CCItemInfo info = m_tags.at(i);
		wxString s2(info.displayName);

		if (s2.StartsWith(word)) {
			matchCount++;
			if(firstMatch == -1)
				firstMatch = static_cast<int>(i);

			if(s2 == word) {
				exactMatch = static_cast<int>(i);
			}
		}
	}

	if(matchCount) {
		if(matchCount == 1 && exactMatch != -1) {
			// we have a single match, and it is a FULL match
			fullMatch = true;
			return exactMatch;

		} else {
			// More than one match or one match but not a FULL match
			fullMatch = false;
			return firstMatch;

		}
	}

	// if we are here, it means we failed, try case insensitive
	wxString s1(word);
	s1.MakeLower();

	for (size_t i=0; i<m_tags.size(); i++) {
		CCItemInfo info = m_tags.at(i);

		wxString s2(info.displayName);
		s2.MakeLower();
		if (s2.StartsWith(s1)) {
			return static_cast<int>(i);
		}
	}

	return wxNOT_FOUND;
}

bool CCVirtualListCtrl::GetItemTagEntry(int index, CCItemInfo& tag)
{
	if(index < 0 || (size_t)index >= m_tags.size()) {
		return false;
	}

	tag = m_tags.at(index);
	return true;
}
