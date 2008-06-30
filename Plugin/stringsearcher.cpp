//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : stringsearcher.cpp
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
#include "stringsearcher.h"
#include <wx/regex.h>
#include <algorithm>

extern unsigned int UTF8Length(const wchar_t *uptr, unsigned int tlen);

wxString StringFindReplacer::GetString(const wxString& input, int from, bool search_up)
{
	if (from < 0) {
		from = 0;
	}

	if ( !search_up ) {

		if (from >= (int)input.Len()) {
			return wxEmptyString;
		}
		return input.Mid((size_t)from);

	} else {
		if (from >= (int)input.Len()) {
			from = (int)input.Len();
		}
		return input.Mid(0, (size_t)from);
	}
}

bool StringFindReplacer::DoRESearch(const wxString& input, int startOffset, const wxString& find_what, size_t flags, int& pos, int& matchLen)
{
	wxString str = GetString(input, startOffset, flags & wxSD_SEARCH_BACKWARD ? true : false);
	if (str.IsEmpty()) {
		return false;
	}

	int re_flags = wxRE_DEFAULT;
	wxRegEx re;
	bool matchCase = flags & wxSD_MATCHCASE ? true : false;
	if ( !matchCase ) re_flags |= wxRE_ICASE;
	re.Compile(find_what, re_flags);

	// incase we are scanning NOT backwared, set the offset
	if (!( flags & wxSD_SEARCH_BACKWARD )) {
		pos = startOffset;
	}

	if ( re.IsValid() ) {
		if ( flags & wxSD_SEARCH_BACKWARD ) {
			size_t start(0), len(0);
			bool matched(false);

			// get the last match
			while (re.Matches(str)) {
				re.GetMatch(&start, &len);
				pos += start;
				if ( matched ) {
					pos += matchLen;
				}
				matchLen = len;
				matched = true;
				str = str.Mid(start + len);
			}

			if ( matched ) {
				return true;
			}

		} else if ( re.Matches(str)) {
			size_t start, len;
			re.GetMatch(&start, &len);
			pos += start;
			matchLen = len;
			return true;
		}
	}
	return false;
}

bool StringFindReplacer::DoSimpleSearch(const wxString& input, int startOffset, const wxString& find_what, size_t flags, int& pos, int& matchLen)
{
	wxString str = GetString(input, startOffset, flags & wxSD_SEARCH_BACKWARD ? true : false);
	size_t init_size = str.length();

	if (str.IsEmpty()) {
		return false;
	}

	wxString find_str(find_what);
	size_t offset(0);

	// incase we are scanning backwared, revert the strings
	if ( flags & wxSD_SEARCH_BACKWARD ) {
		std::reverse(find_str.begin(), find_str.end());
		std::reverse(str.begin(), str.end());
	} else {
		offset = startOffset;
	}

	bool matchCase = flags & wxSD_MATCHCASE ? true : false;
	if ( !matchCase ) {
		find_str.MakeLower();
		str.MakeLower();
	}

	pos = str.Find(find_str);

	while ( pos != wxNOT_FOUND ) {
		if (flags & wxSD_MATCHWHOLEWORD) {
			// full word match
			// test that the characeter at pos - 1 & the character at pos + find_str.Len() are not
			// valid word char [a-zA-Z0-9_]
			if (pos - 1 > 0) {
				wxString str_before(str.GetChar(pos-1));

				if (str_before.find_first_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")) != wxString::npos) {

					// remove the part that already been scanned
					// and search again
					str = str.Mid(pos+find_what.Len());
					offset += pos+find_what.Len();
					pos = str.Find(find_str);
					continue;
				}
			}
			int charAfterOff = pos + find_str.Len();
			if (charAfterOff < (int)str.Len()) {
				wxString str_after(str.GetChar(charAfterOff));

				if ( str_after.find_first_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_")) != wxString::npos) {
					// remove the part that already been scanned
					// and search again
					str = str.Mid(pos+find_what.Len());
					offset += pos+find_what.Len();
					pos = str.Find(find_str);
					continue;
				}
			}

			matchLen = (int)find_str.Len();
			// mirror the result as well
			if (flags & wxSD_SEARCH_BACKWARD) {
				pos = (init_size - (pos + offset + matchLen));
			} else {
				pos += offset;
			}
			return true;
		} else {
			// we got a match
			matchLen = (int)find_str.Len();
			if (flags & wxSD_SEARCH_BACKWARD) {
				pos = (init_size - (pos + offset + matchLen));
			} else {
				pos += offset;
			}
			return true;
		}
	}
	return false;
}

bool StringFindReplacer::Search(const wxString& input, int startOffset, const wxString& find_what, size_t flags,
                                int& pos, int& matchLen, int& posInChars, int& matchLenInChars)
{
	bool bResult = false;
	if (flags & wxSD_REGULAREXPRESSION) {
		bResult = DoRESearch(input, startOffset, find_what, flags, posInChars, matchLenInChars);
	} else {
		bResult = DoSimpleSearch(input, startOffset, find_what, flags, posInChars, matchLenInChars);
	}
	// correct search Pos and Length owing to non plain ASCII multibyte characters
	if (bResult) {
		pos = UTF8Length(input.c_str(), posInChars);
		if (flags & wxSD_REGULAREXPRESSION) {
			matchLen = UTF8Length(input.c_str(), posInChars + matchLenInChars) - pos;
		} else {
			matchLen = UTF8Length(find_what.c_str(), matchLenInChars);
		}
	}
	return bResult;
}

bool StringFindReplacer::Search(const wxString& input, int startOffset, const wxString& find_what, size_t flags, int& pos, int& matchLen)
{
	int posInChars(0), matchLenInChars(0);
	return StringFindReplacer::Search(input, startOffset, find_what, flags, pos, matchLen, posInChars, matchLenInChars);
}
