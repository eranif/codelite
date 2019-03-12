//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : dbcontentcacher.cpp
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

#include "dbcontentcacher.h"
#include <wx/ffile.h>
#include <wx/xrc/xmlres.h>
#include <wx/stopwatch.h>
#include "fileutils.h"

const wxEventType wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED = XRCID("wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED");

DbContentCacher::DbContentCacher(wxEvtHandler* parent, const wxChar* dbfilename)
    : Job(parent)
    , m_filename(dbfilename)
{
}

DbContentCacher::~DbContentCacher() {}

void DbContentCacher::Process(wxThread* thread)
{
    wxStopWatch sw;
    sw.Start();

    wxString fileContent;
    wxCSConv fontEncConv(wxFONTENCODING_ISO8859_1);
    FileUtils::ReadFileContent(m_filename, fileContent, fontEncConv);

    if(m_parent) {
        wxCommandEvent e(wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED);
        e.SetString(wxString::Format(_("Symbols file loaded into OS file system cache (%ld seconds)"), sw.Time() / 1000)
                        .c_str());
        m_parent->AddPendingEvent(e);
    }
}
