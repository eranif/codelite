//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : fileutils.h
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
#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "wx/filename.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_CL FileUtils
{
public:
    static bool ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv = wxConvUTF8);
    
    /**
     * @brief set the file content (replacing it)
     */
    static bool WriteFileContent(const wxFileName& fn, const wxString &content, const wxMBConv& conv = wxConvUTF8);
    
    /**
     * @brief open file explorer at given path
     */
    static void OpenFileExplorer(const wxString& path);
    
    /**
     * @brief launch the OS default terminal at a given path
     */
    static void OpenTerminal(const wxString& path);
};
#endif // FILEUTILS_H
