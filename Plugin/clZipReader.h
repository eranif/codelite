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
#include <wx/zipstrm.h>
#include <wx/wfstream.h>
#include <wx/stream.h>
#include <wx/filename.h>

class WXDLLIMPEXP_SDK clZipReader
{
    wxFileInputStream *m_file;
    wxZipInputStream *m_zip;
    
public:
    clZipReader(const wxFileName& zipfile);
    ~clZipReader();
    
    /**
     * @brief extract filename into 'path' (directory)
     * @param filename file name to extract. Wildcards ('*'/'?') can be used here
     * @param directory the target directory
     */
    void Extract(const wxString &filename, const wxString &directory);
    
    /**
     * @brief close the zip archive
     */
    void Close();
};

#endif // CLZIP_H
