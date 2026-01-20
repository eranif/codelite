//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dirtraverser.h
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
#ifndef DIRTRAVERSER_H
#define DIRTRAVERSER_H

#include "codelite_exports.h"

#include <wx/arrstr.h>
#include <wx/dir.h>

/**
 * A helper class that help us recurse into sub-directories.
 * For each found directory, sink.OnDir() is called and sink.OnFile() is called for every file.
 * Depending on the return value, the enumeration may continue or stop of one of the files, the
 * recurse may stop.
 * \ingroup CodeLite
 * \version 1.0
 * first version
 * \date 11-08-2006
 * \author Eran
 */
class WXDLLIMPEXP_CL DirTraverser : public wxDirTraverser
{
    wxArrayString m_files;
    wxString m_filespec;
    wxArrayString m_specArray;

    wxArrayString m_excludeDirs;

public:

    /**
     * Construct a DirTraverser with a given file spec
     */
    DirTraverser(const wxString &filespec);
    virtual ~DirTraverser() = default;

    /**
     * This function is called once a file is found. The traverse of the directories
     * can be stopped based on the return value from this function:
     * - wxDIR_IGNORE = -1,      // ignore this directory but continue with others
     * - wxDIR_STOP,             // stop traversing
     * - wxDIR_CONTINUE          // continue into this directory
     * \param filename name of the file that was found
     * \return one of the values wxDIR_STOP, wxDIR_IGNORE or wxDIR_CONTINUE
     */
    virtual wxDirTraverseResult  OnFile(const wxString& filename);

    /**
     * This function is called once a directory is found. The traverse of the directories
     * can be stopped based on the return value from this function:
     * - wxDIR_IGNORE = -1,      // ignore this directory but continue with others
     * - wxDIR_STOP,             // stop traversing
     * - wxDIR_CONTINUE          // continue into this directory
     * \param dirname name of the directory that was found
     * \return one of the values wxDIR_STOP, wxDIR_IGNORE or wxDIR_CONTINUE
     */
    virtual wxDirTraverseResult OnDir(const wxString& dirname);

    void SetExcludeDirs(const wxArrayString& excludeDirs) {
        this->m_excludeDirs = excludeDirs;
    }
    const wxArrayString& GetExcludeDirs() const {
        return m_excludeDirs;
    }

    /**
     * Return list of files found
     * \return Return list of files found
     */
    wxArrayString& GetFiles() {
        return m_files;
    }
};

#endif //DIRTRAVERSER_H
