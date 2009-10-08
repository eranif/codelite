//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : scopejob.cpp
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

#if 0
#include "scopejob.h"
#include "tags_database.h"


const wxEventType wxEVT_CMD_UPDATE_SCOPE = wxNewEventType();

ScopeJob::ScopeJob(wxEvtHandler* parent, const wxChar* file_name, int curr_line, const wxChar *db_path)
		: Job(parent)
		, m_currLine(curr_line)
		, m_fileName(file_name)
		, m_dbPath(db_path)
{
}

ScopeJob::~ScopeJob()
{
}

void ScopeJob::Process(wxThread* thread)
{
	TagsDatabase db;
	db.OpenDatabase(m_dbPath);

	// query the database
	wxString sql;
	sql << wxT("select * from tags where file = '")
		<< m_fileName
		<< wxT("' and line <= ")
		<< m_currLine + 1
		<< wxT(" and kind='function' order by line DESC");

	// we take the first entry
	TagEntry *tag(NULL);
	try {
		wxSQLite3ResultSet rs = db.Query(sql);
		if ( rs.NextRow() ) {
			// Construct a TagEntry from the rescord set
			tag = new TagEntry(rs);
			rs.Finalize();
		}
		rs.Finalize();
	} catch ( wxSQLite3Exception& e) {
		wxUnusedVar(e);
	}

	if (tag) {
		// send an event to parent
		wxCommandEvent event(wxEVT_CMD_UPDATE_SCOPE);

		ScopeJobResult *result = new ScopeJobResult;
		result->last_line = m_currLine;
		result->tag = tag;

		event.SetClientData( result );
		m_parent->AddPendingEvent( event );
	}
}

#endif

