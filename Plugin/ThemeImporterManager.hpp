//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : ThemeImporterManager.h
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

#ifndef ECLIPSETHEMEIMPORTERMANAGER_H
#define ECLIPSETHEMEIMPORTERMANAGER_H

#include "ThemeImporterBase.hpp"
#include "codelite_exports.h"

#include <wx/string.h>

class WXDLLIMPEXP_SDK ThemeImporterManager
{
    ThemeImporterBase::List_t m_importers;

public:
    ThemeImporterManager();
    virtual ~ThemeImporterManager();

    /**
     * @brief import the XML and create a lexer out of it
     * @param theme_file path to the XML file
     * @return the new theme name
     */
    wxString Import(const wxString& theme_file);
};

#endif // ECLIPSETHEMEIMPORTERMANAGER_H
