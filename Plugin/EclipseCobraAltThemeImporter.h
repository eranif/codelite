//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : EclipseCobraAltThemeImporter.h
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

#ifndef ECLIPSECOBRAALTTHEMEIMPORTER_H
#define ECLIPSECOBRAALTTHEMEIMPORTER_H

#include "EclipsePythonThemeImporter.h" // Base class: EclipsePythonThemeImporter
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK EclipseCobraAltThemeImporter : public EclipsePythonThemeImporter
{
public:
    EclipseCobraAltThemeImporter();
    virtual ~EclipseCobraAltThemeImporter();

};

#endif // ECLIPSECOBRAALTTHEMEIMPORTER_H
