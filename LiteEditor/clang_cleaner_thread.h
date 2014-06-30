//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 The CodeLite Team
// file name            : clang_cleaner_thread.h
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

#ifndef CLANGCLEANERTHREAD_H
#define CLANGCLEANERTHREAD_H

#if HAS_LIBCLANG
#include <wx/thread.h> // Base class: wxThread
#include "macros.h"

class ClangCleanerThread : public wxThread
{
    wxCriticalSection m_cs;
    wxStringSet_t m_files;
    
protected:
    void DeleteTemporaryFiles();
    void DeleteStalePCHFiles();
    
public:
    ClangCleanerThread();
    virtual ~ClangCleanerThread();

public:
    virtual void* Entry();
    
    void Start() {
        Create();
        Run();
    }
    
    /**
     * @brief stop the working thread
     * this function shdould be called from the main thread only
     */
    void Stop() {
        if ( IsAlive() ) {
            Delete(NULL, wxTHREAD_WAIT_BLOCK);
            
        } else {
            Wait(wxTHREAD_WAIT_BLOCK);
        }
    }
    
    /**
     * @brief add a file name to be cleared from the file system
     * @param filename
     */
    void AddFileName( const wxString &filename );
};

#endif // HAS_LIBCLANG
#endif // CLANGCLEANERTHREAD_H
