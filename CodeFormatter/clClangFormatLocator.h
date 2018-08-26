//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clClangFormatLocator.h
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

#ifndef CLCLANGFORMATLOCATOR_H
#define CLCLANGFORMATLOCATOR_H

#include <wx/string.h>
#include <wx/filename.h>

class clClangFormatLocator
{
public:
    clClangFormatLocator();
    virtual ~clClangFormatLocator();

    /**
     * @brief locate clang-format executable
     * @return
     */
    bool Locate(wxString& clangFormat);

    /**
     * @brief return the clang format version installed
     */
    double GetVersion(const wxString& clangFormat) const;
};

#endif // CLCLANGFORMATLOCATOR_H
