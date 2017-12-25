//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clang_cleaner_thread.cpp
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

#include "clang_cleaner_thread.h"
#include "file_logger.h"
#include "fileutils.h"
#include <wx/dir.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/utils.h>
#if HAS_LIBCLANG

ClangCleanerThread::ClangCleanerThread()
    : wxThread(wxTHREAD_JOINABLE)
{
}

ClangCleanerThread::~ClangCleanerThread() {}

void* ClangCleanerThread::Entry()
{
    wxLogNull nl;
    size_t ticks(0);
    while(!TestDestroy()) {

        if(ticks % 5 == 0) {
            // Clear all temporary files created for clang parsing
            DeleteStalePCHFiles();

            // Clear stale .pch files from %TMP%
            // libclang does not clear properly any stale files under %TMP% (this is at least under Windows)
            DeleteTemporaryFiles();
        }
        wxThread::This()->Sleep(1000);
        ++ticks;
    }
    return NULL;
}

void ClangCleanerThread::AddFileName(const wxString& filename)
{
    wxCriticalSectionLocker locker(m_cs);
    if(m_files.count(filename) == 0) { m_files.insert(filename); }
}

void ClangCleanerThread::DeleteTemporaryFiles()
{
    wxStringSet_t files;
    {
        wxCriticalSectionLocker locker(m_cs);
        files.swap(m_files);
        m_files.clear();
    }

    wxStringSet_t failedSet;
    wxStringSet_t::const_iterator iter = files.begin();
    while(iter != files.end()) {
        if(wxFileName::Exists(*iter) && !clRemoveFile(*iter)) {
            failedSet.insert(*iter);
            CL_DEBUG("Failed to delete file %s", *iter);

        } else {
            CL_DEBUG("Successfully deleted file %s", *iter);
        }
        ++iter;
    }

    if(!failedSet.empty()) {
        wxCriticalSectionLocker locker(m_cs);

        // add the "failed" files to the set, we will try again later
        m_files.insert(failedSet.begin(), failedSet.end());
    }
}

void ClangCleanerThread::DeleteStalePCHFiles()
{
    wxString tmpdir;
    if(::wxGetEnv("TMP", &tmpdir)) {
        wxArrayString files;
        if(wxDir::GetAllFiles(tmpdir, &files, "*.pch", wxDIR_FILES)) {
            for(size_t i = 0; i < files.GetCount(); ++i) {
                wxFileName fn(files.Item(i));
                if(fn.GetFullName().StartsWith("preamble")) {
                    if(clRemoveFile(files.Item(i))) { CL_DEBUG("Deleting stale file: %s", files.Item(i)); }
                }
            }
        }
    }
}

#endif // HAS_LIBCLANG
