//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : scopejob.h
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

#ifndef __scopejob__
#define __scopejob__

#if 0
#include "job.h"

extern const wxEventType wxEVT_CMD_UPDATE_SCOPE;


class TagEntry;

struct ScopeJobResult {
	TagEntry *tag;
	int last_line;
};

class ScopeJob : public Job {
	int m_currLine;
	wxString m_fileName;
	wxString m_dbPath;

public:
	ScopeJob(wxEvtHandler *parent, const wxChar *file_name, int curr_line, const wxChar *db_path);
	virtual ~ScopeJob();

public:
	virtual void Process(wxThread *thread);
};
#endif

#endif // __scopejob__
