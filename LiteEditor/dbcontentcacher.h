//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : dbcontentcacher.h
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

#ifndef DBCONTENTCACHER_H
#define DBCONTENTCACHER_H

#include "job.h" // Base class: Job

extern const wxEventType wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED;

class DbContentCacher : public Job 
{
	wxString m_filename;
public:
	DbContentCacher(wxEvtHandler* parent, const wxChar* dbfilename);
	virtual ~DbContentCacher();

public:
	virtual void Process(wxThread* thread);
};

#endif // DBCONTENTCACHER_H
