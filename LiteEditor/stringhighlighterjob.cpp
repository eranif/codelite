//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : stringhighlighterjob.cpp
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
#include "search_thread.h"
#include "stringhighlighterjob.h"
#include "stringsearcher.h"
#include <vector>

StringHighlighterJob::StringHighlighterJob(const wxString& str, const wxString& word, int offset)
    : m_str(str)
    , m_word(word)
    , m_offset(offset)
{
}

StringHighlighterJob::~StringHighlighterJob() {}

void StringHighlighterJob::Set(const wxString& str, const wxString& word, int offset)
{
    SetStr(str);
    SetWord(word);
    SetOffset(offset);
}

void StringHighlighterJob::Process()
{
    if(m_str.IsEmpty() || m_word.IsEmpty()) { return; }

    int pos(0);
    int match_len(0);

    // remove reverse search
    int offset(0);

    const wchar_t* pin = m_str.c_str().AsWChar();
    const wchar_t* pwo = m_word.c_str().AsWChar();

    while(StringFindReplacer::Search(pin, offset, pwo, wxSD_MATCHCASE | wxSD_MATCHWHOLEWORD, pos, match_len)) {
        // add result pair(offset, len)
        std::pair<int, int> match;
        match.first = pos + m_offset; // add the offset
        match.second = match_len;
        m_output.matches.push_back(match);
        offset = pos + match_len;
    }
}
