//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : PHPIndex.h
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

#ifndef PHPINDEX_H
#define PHPINDEX_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/arrstr.h>

/**
 * @class PHPIndex this class represents a collection of PHPSourceFiles
 */
class WXDLLIMPEXP_CL PHPIndex
{
public:
    PHPIndex();
    virtual ~PHPIndex();
    
    /**
     * @brief index list of files and store them into the database
     */
    void Index(const wxArrayString& files, const wxString &dbpath);
};

#endif // PHPINDEX_H
