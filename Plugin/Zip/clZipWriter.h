//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clZipWriter.h
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

#ifndef CLZIPWRITER_H
#define CLZIPWRITER_H

#include "codelite_exports.h"

#include <wx/filename.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

class WXDLLIMPEXP_SDK clZipWriter
{
    wxFileName m_filename;
    wxZipOutputStream* m_zip;
    wxFileOutputStream* m_file;

public:
    clZipWriter(const wxFileName& zipfile);
    ~clZipWriter();

    /**
     * @brief add file to the zip
     */
    void Add(const wxFileName& file);

    /**
     * @brief add a directory to the archive
     * @param path directory path
     * @param pattern files to add mask
     */
    void AddDirectory(const wxString& path, const wxString& pattern);

    /**
     * @brief return the zip filename
     */
    const wxFileName& GetFilename() const { return m_filename; }

    /**
     * @brief close the zip
     */
    void Close();
};

#endif // CLZIPWRITER_H
