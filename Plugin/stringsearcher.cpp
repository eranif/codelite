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

#include "globals.h"
#include "search_thread.h"

#include <algorithm>
#include <string>
#include <wx/regex.h>

static std::wstring Reverse(const std::wstring& str)
{
    // reverse the string content
    std::wstring tmp;
    tmp.reserve(str.length());

    std::wstring::const_reverse_iterator riter = str.rbegin();
    for(; riter != str.rend(); riter++) {
        tmp += *riter;
    }
    return tmp;
}

wxString StringFindReplacer::GetString(const wxString& input, int from, bool search_up)
{
    if(from < 0) {
        from = 0;
    }

    if(!search_up) {

        if(from >= (int)input.Len()) {
            return wxEmptyString;
        }
        return input.Mid((size_t)from);

    } else {
        if(from >= (int)input.Len()) {
            from = (int)input.Len();
        }
        return input.Mid(0, (size_t)from);
    }
}

bool StringFindReplacer::DoWildcardSearch(const wxString& input, int startOffset, const wxString& find_what,
                                          size_t flags, int& pos, int& matchLen)
{
    // Conver the wildcard to regex
    wxString regexPattern = find_what;

    // Escape braces
    regexPattern.Replace("(", "\\(");
    regexPattern.Replace(")", "\\)");
    regexPattern.Replace("[", "\\[");
    regexPattern.Replace("]", "\\]");
    regexPattern.Replace("{", "\\{");
    regexPattern.Replace("}", "\\}");

    // Covnert match syntax to regular expression
    regexPattern.Replace("?", "."); // Any character
    regexPattern.Replace("*",
                         "[^\\n]*?"); // Non greedy wildcard '*', but don't allow matches to go beyond a single line

    return DoRESearch(input, startOffset, regexPattern, flags, pos, matchLen);
}

bool StringFindReplacer::DoRESearch(const wxString& input, int startOffset, const wxString& find_what, size_t flags,
                                    int& pos, int& matchLen)
{
    wxString str = GetString(input, startOffset, flags & wxSD_SEARCH_BACKWARD ? true : false);
    if(str.IsEmpty()) {
        return false;
    }

#ifndef __WXMAC__
    int re_flags = wxRE_ADVANCED;
#else
    int re_flags = wxRE_DEFAULT;
#endif
    wxRegEx re;
    bool matchCase = flags & wxSD_MATCHCASE ? true : false;
    if(!matchCase)
        re_flags |= wxRE_ICASE;
    re_flags |= wxRE_NEWLINE; // Handle \n as a special character
    re.Compile(find_what, re_flags);

    // incase we are scanning NOT backwared, set the offset
    if(!(flags & wxSD_SEARCH_BACKWARD)) {
        pos = startOffset;
    }

    if(re.IsValid()) {
        if(flags & wxSD_SEARCH_BACKWARD) {
            size_t start(0), len(0);
            bool matched(false);

            // get the last match
            while(re.Matches(str)) {
                re.GetMatch(&start, &len);
                if(len == 0) {
                    matched = false;
                    break;
                }
                pos += start;
                if(matched) {
                    pos += matchLen;
                }
                matchLen = len;
                matched = true;
                str = str.Mid(start + len);
            }

            if(matched) {
                return true;
            }

        } else if(re.Matches(str)) {
            size_t start, len;
            re.GetMatch(&start, &len);
            pos += start;
            matchLen = len;
            return true;
        }
    }
    return false;
}

bool StringFindReplacer::DoSimpleSearch(const wchar_t* pinput, int startOffset, const wchar_t* pfind_what, size_t flags,
                                        int& pos, int& matchLen)
{
#if wxVERSION_NUMBER >= 2900
    std::wstring input(pinput);
    std::wstring find_what(pfind_what);

    int from = startOffset;
    if(from < 0) {
        from = 0;
    }
    std::wstring str;
    bool search_up = flags & wxSD_SEARCH_BACKWARD;
    if(!search_up) {

        if(from >= (int)input.length()) {
            str.clear();
            return false;
        }
        str = input.substr((size_t)from);

    } else {
        if(from >= (int)input.length()) {
            from = (int)input.length();
        }
        str = input.substr(0, (size_t)from);
    }
#else
    wxString input(pinput);
    wxString find_what(pfind_what);
    std::wstring str = GetString(pinput, startOffset, flags & wxSD_SEARCH_BACKWARD ? true : false).c_str();
#endif

    size_t init_size = str.length();

    if(str.empty()) {
        return false;
    }

    std::wstring find_str(find_what);
    size_t offset(0);

    // incase we are scanning backwared, revert the strings
    if(flags & wxSD_SEARCH_BACKWARD) {
        find_str = Reverse(find_str);
        str = Reverse(str);
    } else {
        offset = startOffset;
    }

    bool matchCase = flags & wxSD_MATCHCASE ? true : false;
    if(!matchCase) {
        std::transform(find_str.begin(), find_str.end(), find_str.begin(), towlower);
        std::transform(str.begin(), str.end(), str.begin(), towlower);
    }

    size_t upos = str.find(find_str);

    while(upos != std::wstring::npos) {
        if(flags & wxSD_MATCHWHOLEWORD) {
            // full word match
            // test that the characeter at upos - 1 & the character at upos + find_str.Len() are not
            // valid word char [a-zA-Z0-9_]
            if(upos > 0) {
                if(isalpha(str[upos - 1]) || isdigit(str[upos - 1]) || (int)str[upos - 1] == (int)'_') {
                    // remove the part that already been scanned
                    // and search again
                    str = str.substr(upos + find_what.length());
                    offset += upos + find_what.length();
                    upos = str.find(find_str);
                    continue;
                }
            }
            int charAfterOff = upos + find_str.length();
            if(charAfterOff < (int)str.length()) {
                if(isalpha(str[charAfterOff]) || isdigit(str[charAfterOff]) || (int)str[charAfterOff] == (int)'_') {
                    // remove the part that already been scanned
                    // and search again
                    str = str.substr(upos + find_what.length());
                    offset += upos + find_what.length();
                    upos = str.find(find_str);
                    continue;
                }
            }

            matchLen = (int)find_str.length();
            // mirror the result as well
            if(flags & wxSD_SEARCH_BACKWARD) {
                upos = (init_size - (upos + offset + matchLen));
            } else {
                upos += offset;
            }
            pos = (int)upos;
            return true;
        } else {
            // we got a match
            matchLen = (int)find_str.length();
            if(flags & wxSD_SEARCH_BACKWARD) {
                upos = (init_size - (upos + offset + matchLen));
            } else {
                upos += offset;
            }
            pos = (int)upos;
            return true;
        }
    }
    return false;
}

bool StringFindReplacer::Search(const wchar_t* input, int startOffset, const wchar_t* find_what, size_t flags, int& pos,
                                int& matchLen, int& posInChars, int& matchLenInChars)
{
    // adjust startOffset due to it is in bytes but should be in chars
    int iSO = startOffset;
    bool isUTF8 = false;
    if(iSO) {
        int utfLen = clUTF8Length(input, iSO);
        isUTF8 = (utfLen != iSO);
    }

    while(isUTF8 && (iSO > 0) && ((int)clUTF8Length(input, iSO) > startOffset))
        iSO--;
    startOffset = iSO;

    bool bResult = false;
    if(flags & wxSD_WILDCARD) {
        bResult = DoWildcardSearch(input, startOffset, find_what, flags, posInChars, matchLenInChars);
        flags |= wxSD_REGULAREXPRESSION;

    } else if(flags & wxSD_REGULAREXPRESSION) {
        bResult = DoRESearch(input, startOffset, find_what, flags, posInChars, matchLenInChars);

    } else {
        bResult = DoSimpleSearch(input, startOffset, find_what, flags, posInChars, matchLenInChars);
    }

    // correct search Pos and Length owing to non plain ASCII multibyte characters
    if(bResult) {
        pos = clUTF8Length(input, posInChars);
        if(flags & wxSD_REGULAREXPRESSION) {
            matchLen = clUTF8Length(input, posInChars + matchLenInChars) - pos;
        } else {
            matchLen = clUTF8Length(find_what, matchLenInChars);
        }
    }
    return bResult;
}

bool StringFindReplacer::Search(const wchar_t* input, int startOffset, const wchar_t* find_what, size_t flags, int& pos,
                                int& matchLen)
{
    int posInChars(0), matchLenInChars(0);
    return StringFindReplacer::Search(input, startOffset, find_what, flags, pos, matchLen, posInChars, matchLenInChars);
}
