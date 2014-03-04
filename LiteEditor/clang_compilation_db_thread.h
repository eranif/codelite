//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2013 by Eran Ifrah
// file name            : clang_compilation_db_thread.h
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

#ifndef CLANGCOMPILATIONDBTHREAD_H
#define CLANGCOMPILATIONDBTHREAD_H

#include <wx/string.h>
#include <wx/thread.h>
#include <wx/msgqueue.h>
#include "singleton.h"

class ClangCompilationDbThread : public wxThread
{
    wxMessageQueue<wxString> m_queue;
    friend class Singleton<ClangCompilationDbThread>;
    
    ClangCompilationDbThread();
    
public:
    virtual ~ClangCompilationDbThread();

public:
    
    void AddFile( const wxString& filename );

    virtual void* Entry();
    void Start() {
        Create();
        Run();
    }
    
    /**
     * @brief stop the thread
     */
    void Stop() {
        if ( IsAlive() ) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);
        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
};
typedef Singleton<ClangCompilationDbThread> ClangCompilationDbThreadST;

#endif // CLANGCOMPILATIONDBTHREAD_H
