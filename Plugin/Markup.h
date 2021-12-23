//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : Markup.h
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

#ifndef MARKUP_H__
#define MARKUP_H__

#include "codelite_exports.h"

#include <list>
#include <wx/regex.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

#define MARKUP_VOID 269
#define BOLD_START 270
#define BOLD_END 271
#define HORIZONTAL_LINE 272
#define NEW_LINE 273
#define CODE_START 274
#define CODE_END 275
#define COLOR_START 276
#define COLOR_END 277
#define ITALIC_START 278
#define ITALIC_END 279
#define LINK_START 280
#define LINK_END 281
#define LINK_URL 282

class WXDLLIMPEXP_SDK MarkupSearchPattern
{
protected:
    wxString m_pattern;
    bool m_isRegex;
    int m_type;
    wxSharedPtr<wxRegEx> m_regex;
    int m_matchIndex;

public:
    typedef std::list<MarkupSearchPattern> List_t;

public:
    MarkupSearchPattern()
        : m_isRegex(false)
        , m_type(0)
        , m_regex(NULL)
    {
    }
    MarkupSearchPattern(const wxString& search, int type, bool isRegex = false, int matchIndex = 0);
    virtual ~MarkupSearchPattern();
    bool Match(wxString& inString, int& type, wxString& matchString);
};

class WXDLLIMPEXP_SDK MarkupParser
{
    MarkupSearchPattern::List_t m_patterns;
    wxString m_tip;
    wxString m_token;
    int m_type;

protected:
    bool IsMatchPattern(wxString& match, int& type);

public:
    MarkupParser(const wxString& tip);
    virtual ~MarkupParser() {}
    bool Next();

    const wxString& GetToken() const { return m_token; }
    int GetType() const { return m_type; }
};

#endif // MARKUP_H__
