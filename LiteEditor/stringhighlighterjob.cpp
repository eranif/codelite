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
#include "stringsearcher.h"
#include "stringhighlighterjob.h"
#include <vector>

StringHighlighterJob::StringHighlighterJob(wxEvtHandler *parent, const wxChar *str, const wxChar *word, const wxChar* filename)
		: Job(parent)
		, m_str(str)
		, m_word(word)
		, m_filename(filename)
{
}

StringHighlighterJob::~StringHighlighterJob()
{
}

void StringHighlighterJob::Process(wxThread* thread)
{
	wxUnusedVar(thread);
	if ( m_str.IsEmpty() || m_word.IsEmpty() ) {
		return;
	}

	int pos(0);
	int match_len(0);

	// remove reverse search
	int offset(0);

	// allocate result on the heap (will be freed by the caller)
	StringHighlightOutput *results = new StringHighlightOutput;

	results->filename = m_filename.c_str();
	results->matches  = new std::vector<std::pair<int, int> >;

	while ( StringFindReplacer::Search(m_str, offset, m_word, wxSD_MATCHCASE | wxSD_MATCHWHOLEWORD, pos, match_len) ) {
		// add result
		std::pair<int, int> match;
		match.first = pos;
		match.second = match_len;

		results->matches->push_back( match );
		offset = pos + match_len;
	}

	// report the result back to parent
	Post((void*) results);
}
