//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : stringhighlighterjob.h
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
#ifndef __stringhighlighterjob__
#define __stringhighlighterjob__

#include "job.h"
#include <vector>

struct StringHighlightOutput
{
	wxString                           filename;
	std::vector<std::pair<int, int> >* matches;

	StringHighlightOutput() : filename(wxT("")), matches(NULL)
	{}

	~StringHighlightOutput()
	{
		if(matches) {
			delete matches;
			matches = NULL;
		}
	}
};

class StringHighlighterJob : public Job
{

	wxString m_str;
	wxString m_word;
	wxString m_filename;

public:
	StringHighlighterJob(wxEvtHandler *parent, const wxChar *str, const wxChar *word, const wxChar *filename);
	virtual ~StringHighlighterJob();

public:
	virtual void Process(wxThread *thread);
};
#endif // __stringhighlighterjob__
