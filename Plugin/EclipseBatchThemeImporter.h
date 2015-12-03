//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 The CodeLite Team
// File name            : EclipseBatchThemeImporter.h
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

#ifndef ECLIPSEBATCHTHEMEIMPORTER_H
#define ECLIPSEBATCHTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseBatchThemeImporter : public EclipseThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
    EclipseBatchThemeImporter();
    ~EclipseBatchThemeImporter();
};

#endif // ECLIPSEBATCHTHEMEIMPORTER_H
