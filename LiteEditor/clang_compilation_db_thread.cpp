//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : clang_compilation_db_thread.cpp
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

#include "clang_compilation_db_thread.h"
#include "compilation_database.h"
#include "file_logger.h"

ClangCompilationDbThread::ClangCompilationDbThread()
    : wxThread(wxTHREAD_JOINABLE)
{
}

ClangCompilationDbThread::~ClangCompilationDbThread()
{
}

void* ClangCompilationDbThread::Entry()
{
    CL_DEBUG("ClangCompilationDbThread: Started");
    wxString filename;
    while ( !TestDestroy() ) {
        if ( m_queue.ReceiveTimeout( 50, filename ) == wxMSGQUEUE_NO_ERROR ) {

            // success
            // Process the file
            CL_DEBUG("ClangCompilationDbThread: Processing file " + filename);
            CompilationDatabase cdb(filename);
            cdb.Initialize();
            CL_DEBUG("ClangCompilationDbThread: Processing file " + filename + "... done");
            
        }
    }
    CL_DEBUG("ClangCompilationDbThread: Going down");
    return NULL;
}

void ClangCompilationDbThread::AddFile(const wxString& filename)
{
    m_queue.Post( filename );
}
