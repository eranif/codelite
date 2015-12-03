//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 The CodeLite Team
// File name            : FilesCollector.h
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

#ifndef FILESCOLLECTOR_H
#define FILESCOLLECTOR_H

#include <wx/dir.h>
#include <wx/arrstr.h>
#include <wx/progdlg.h>
#include "macros.h"

#define FOLDER_MARKER "folder.marker"

class FilesCollector : public wxDirTraverser
{
    wxArrayString m_specArray;
    wxArrayString m_filesAndFolders;
    wxProgressDialog* m_progress;
    wxStringSet_t m_excludeFolders;
    
public:
    FilesCollector(const wxString& filespec,
                   const wxString& excludeFolders = "",
                   wxProgressDialog* progress = NULL);
    virtual ~FilesCollector();

    const wxArrayString& GetFilesAndFolders() const { return m_filesAndFolders; }
    wxArrayString& GetFilesAndFolders() { return m_filesAndFolders; }

public:
    virtual wxDirTraverseResult OnDir(const wxString& dirname);
    virtual wxDirTraverseResult OnFile(const wxString& filename);
};

#endif // FILESCOLLECTOR_H
