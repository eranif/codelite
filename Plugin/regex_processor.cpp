//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : regex_processor.cpp
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
#include "regex_processor.h"

#include "macros.h"
#include "wx/regex.h"

RegexProcessor::RegexProcessor(const wxString& reStr)
{
#ifndef __WXMAC__
    m_re = new wxRegEx(reStr);
#else
    m_re = new wxRegEx(reStr, wxRE_ADVANCED);
#endif
}

RegexProcessor::~RegexProcessor() { delete m_re; }

bool RegexProcessor::GetGroup(const wxString& str, int grp, wxString& out)
{
    if(m_re && m_re->IsValid()) {
        if(m_re->Matches(str)) {
            out = m_re->GetMatch(str, grp);
            TrimString(out);
            return true;
        }
    }
    return false;
}
