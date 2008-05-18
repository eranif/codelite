#include "stringsearcher.h"
#include "search_thread.h"
#include <wx/regex.h>
#include <algorithm>

wxString StringFindReplacer::GetString(const wxString& input, int from)
{
	if (from < 0) { 
		from = 0;
	}
	if (from >= (int)input.Len()) {
		return wxEmptyString;
	}

	return input.Mid((size_t)from);
}
bool StringFindReplacer::DoRESearch(const wxString& input, int startOffset, const wxString& find_what, size_t flags, int& pos, int& matchLen)
{
	wxString str = GetString(input, startOffset);
	if (str.IsEmpty()) {
		return false;
	}

	int re_flags = wxRE_DEFAULT;
	wxRegEx re;
	bool matchCase = flags & wxSD_MATCHCASE ? true : false;
	if ( !matchCase ) re_flags |= wxRE_ICASE;
	re.Compile(find_what, re_flags);
	pos = startOffset;

	if ( re.IsValid() ) {
		if ( flags & wxSD_SEARCH_BACKWARD ) {
			size_t start(0), len(0);
			bool matched(false);
			
			// get the last match 
			while (re.Matches(str)){
				re.GetMatch(&start, &len);
				pos += start;
				if( matched ) {pos += matchLen;}				
				matchLen = len;
				matched = true;
				str = str.Mid(start + len);
			}
			
			if( matched ) {
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
	wxString str = GetString(input, startOffset);
	size_t init_size = str.length();
	
	if (str.IsEmpty()) {
		return false;
	}
	
	wxString find_str(find_what);
	
	// incase we are scanning backwared, revert the strings
	if( flags & wxSD_SEARCH_BACKWARD ) {
		std::reverse(find_str.begin(), find_str.end());
		std::reverse(str.begin(), str.end());
	}
		
	bool matchCase = flags & wxSD_MATCHCASE ? true : false;
	if ( !matchCase ) {
		find_str.MakeLower();
		str.MakeLower();
	}

	pos = str.Find(find_str);
	size_t offset(startOffset);
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
			
			// mirror the result as well
			if(flags & wxSD_SEARCH_BACKWARD) {
				pos = init_size - pos - matchLen;	
			}
			matchLen = (int)find_str.Len();
			pos += offset;
			return true;
		} else {
			// we got a match
			matchLen = (int)find_str.Len();
			if(flags & wxSD_SEARCH_BACKWARD) {
				pos = init_size - pos - matchLen;	
			}
			pos += offset;
			return true;
		}
	}
	return false;
}

bool StringFindReplacer::Search(const wxString& input, int startOffset, const wxString& find_what, size_t flags, int& pos, int& matchLen)
{
	if (flags & wxSD_REGULAREXPRESSION) {
		return DoRESearch(input, startOffset, find_what, flags, pos, matchLen);
	} else {
		return DoSimpleSearch(input, startOffset, find_what, flags, pos, matchLen);
	}
}
