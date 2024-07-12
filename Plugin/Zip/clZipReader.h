//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clZipReader.h
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

#ifndef CLZIP_H
#define CLZIP_H

#include "codelite_exports.h"
#include "wxStringHash.h"

#include <unordered_map>
#include <wx/buffer.h>
#include <wx/filename.h>
#include <wx/mstream.h>
#include <wx/stream.h>
#include <wx/zipstrm.h>

class WXDLLIMPEXP_SDK clZipReader
{
    wxMemoryBuffer m_mb;
    wxInputStream* m_file = nullptr;
    wxZipInputStream* m_zip = nullptr;

public:
    struct Entry {
        void* buffer = nullptr;
        size_t len = 0;
    };

protected:
    void DoExtractEntry(wxZipEntry* entry, const wxString& directory);

public:
    clZipReader(const wxFileName& zipfile);
    ~clZipReader();

    /**
     * @brief extract filename into 'path' (directory)
     * @param filename file name to extract. Wildcards ('*'/'?') can be used here
     * @param directory the target directory
     */
    void Extract(const wxString& filename, const wxString& directory);

    /**
     * @brief extract the entire content of a zip archive into a directory
     */
    void ExtractAll(const wxString& directory);

    /**
     * @brief extract all zip entries and constract a map of name:memory-output-stream ptr
     */
    void ExtractAll(std::unordered_map<wxString, Entry>& buffers);

    /**
     * @brief close the zip archive
     */
    void Close();
};

#endif // CLZIP_H
