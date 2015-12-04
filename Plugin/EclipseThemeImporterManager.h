//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : EclipseThemeImporterManager.h
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

#include "codelite_exports.h"
#include <wx/string.h>
#include "EclipseThemeImporterBase.h"

class WXDLLIMPEXP_SDK EclipseThemeImporterManager
{
    EclipseThemeImporterBase::List_t m_importers;
    
public:
    EclipseThemeImporterManager();
    virtual ~EclipseThemeImporterManager();
    
    bool Import(const wxString& eclipseXml);
    
    /**
     * @brief scan, convert all C++ existing lexers into eclipse XML and then import them
     * to all other languages that we support. This is used internally and not exposed to 
     * the end user
     */
    bool ImportCxxToAll();
};

#endif // ECLIPSETHEMEIMPORTERMANAGER_H
