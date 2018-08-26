//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : stringsearcher.h
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
#ifndef __stringsearcher__
#define __stringsearcher__

#include <wx/string.h>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK StringFindReplacer
{

protected:
    static wxString GetString(const wxString& input, int from, bool search_up);
    static bool DoRESearch(const wxString& input,
                           int startOffset,
                           const wxString& find_what,
                           size_t flags,
                           int& pos,
                           int& matchLen);
    static bool DoWildcardSearch(const wxString& input,
                                 int startOffset,
                                 const wxString& find_what,
                                 size_t flags,
                                 int& pos,
                                 int& matchLen);
    static bool DoSimpleSearch(const wchar_t* pinput,
                               int startOffset,
                               const wchar_t* find_what,
                               size_t flags,
                               int& pos,
                               int& matchLen);

public:
    static bool
    Search(const wchar_t* input, int startOffset, const wchar_t* find_what, size_t flags, int& pos, int& matchLen);
    // overloaded method because of that ReplaceAll methods works on wxString and needs results in chars and other
    // methods
    // using selection needs results in bytes
    static bool Search(const wchar_t* input,
                       int startOffset,
                       const wchar_t* find_what,
                       size_t flags,
                       int& pos,
                       int& matchLen,
                       int& posInChars,
                       int& matchLenInChars);
};
#endif // __stringsearcher__
