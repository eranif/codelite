//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : Markup.cpp
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

#include "Markup.h"

MarkupParser::MarkupParser(const wxString& tip)
    : m_tip(tip)
{
    m_patterns.push_back(MarkupSearchPattern("\n", NEW_LINE));
    m_patterns.push_back(MarkupSearchPattern("<b>", BOLD_START));
    m_patterns.push_back(MarkupSearchPattern("</b>", BOLD_END));
    m_patterns.push_back(MarkupSearchPattern("<i>", ITALIC_START));
    m_patterns.push_back(MarkupSearchPattern("</i>", ITALIC_END));
    m_patterns.push_back(MarkupSearchPattern("<code>", CODE_START));
    m_patterns.push_back(MarkupSearchPattern("</code>", CODE_END));
    m_patterns.push_back(MarkupSearchPattern("<strong>", BOLD_START));
    m_patterns.push_back(MarkupSearchPattern("</strong>", BOLD_END));
    m_patterns.push_back(MarkupSearchPattern("<p>", MARKUP_VOID));
    m_patterns.push_back(MarkupSearchPattern("</p>", MARKUP_VOID));
    m_patterns.push_back(MarkupSearchPattern("<hr>", HORIZONTAL_LINE));
    m_patterns.push_back(MarkupSearchPattern("</color>", COLOR_END));
    m_patterns.push_back(MarkupSearchPattern("^<color=\"[a-zA-Z _]+\">", COLOR_START, true));
    m_patterns.push_back(MarkupSearchPattern("^@link[ \t]+([^ \t\n\v\r]+)", LINK_URL, true, 1));

    wxRegEx reParam("@param");
    if(reParam.Matches(m_tip)) {
        reParam.ReplaceAll(&m_tip, "<b>@param</b>");
    }

    wxRegEx reReturns("@return[s]{0,1}");
    if(reReturns.Matches(m_tip)) {
        reReturns.ReplaceAll(&m_tip, "<b>@return</b>");
    }
}

bool MarkupParser::Next()
{
    if(m_tip.IsEmpty()) {
        return false;
    }

    wxString match;
    int type;
    if(IsMatchPattern(match, type)) {
        m_token = match;
        m_type = type;
        return true;
    }

    // Move to the next char
    m_token = m_tip.GetChar(0);
    m_tip.Remove(0, 1);
    m_type = wxNOT_FOUND;
    return true;
}

bool MarkupParser::IsMatchPattern(wxString& match, int& type)
{
    MarkupSearchPattern::List_t::iterator iter = m_patterns.begin();
    for(; iter != m_patterns.end(); ++iter) {
        if(iter->Match(m_tip, type, match)) {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------

bool MarkupSearchPattern::Match(wxString& inString, int& type, wxString& matchString)
{
    if(m_regex && m_regex->IsValid() && m_regex->Matches(inString)) {
        // get the entire match
        matchString = m_regex->GetMatch(inString, m_matchIndex);
        wxString full_match = m_regex->GetMatch(inString);
        inString = inString.Mid(full_match.length());
        type = m_type;
        return true;
    } else if(!m_regex) {
        bool match = inString.StartsWith(m_pattern, &inString);
        if(match) {
            type = m_type;
        }
        return match;
    }
    return false;
}

MarkupSearchPattern::MarkupSearchPattern(const wxString& search, int type, bool isRegex, int matchIndex)
    : m_isRegex(isRegex)
    , m_type(type)
    , m_matchIndex(matchIndex)
{
    if(isRegex) {
        m_regex = new wxRegEx(search);

    } else {
        m_pattern = search;
    }
}

MarkupSearchPattern::~MarkupSearchPattern() {}
