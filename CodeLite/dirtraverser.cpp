//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : dirtraverser.cpp
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
#include "dirtraverser.h"
#include "fileutils.h"

#include <wx/filename.h>
#include <wx/log.h>
#include <wx/tokenzr.h>

DirTraverser::DirTraverser(const wxString &filespec, bool includeExtLessFiles)
    : wxDirTraverser()
    , m_filespec(filespec)
    , m_extlessFiles(includeExtLessFiles)
{
    m_specArray = wxStringTokenize(filespec, wxT(";"), wxTOKEN_STRTOK);
}

wxDirTraverseResult DirTraverser::OnFile(const wxString& filename)
{
    // add the file to our array
    wxFileName fn(filename);
    
    if(FileUtils::WildMatch(m_filespec, fn)) {
        m_files.Add(filename);
        return wxDIR_CONTINUE;
    }

    // if we reached this point, no pattern was suitable for our file
    // test for extensionless file flag
    if (fn.GetExt().IsEmpty() && m_extlessFiles) {
        m_files.Add(filename);
    }

    return wxDIR_CONTINUE;
}

wxDirTraverseResult DirTraverser::OnDir(const wxString &dirname)
{
    for (size_t i=0; i<m_excludeDirs.GetCount(); i++) {
        wxString tmpDir(dirname);
        tmpDir.Replace(wxT("\\"), wxT("/"));
        wxArrayString toks = wxStringTokenize(tmpDir, wxT("/"), wxTOKEN_STRTOK);
        wxString dir = m_excludeDirs.Item(i);
        wxString onlyDirName;

        if (toks.GetCount() > 0) {
            onlyDirName = toks.Last();
        }

        if (onlyDirName == dir) {
            return wxDIR_IGNORE;
        }
    }
    return wxDIR_CONTINUE;
}

DirTraverser::~DirTraverser()
{
}
