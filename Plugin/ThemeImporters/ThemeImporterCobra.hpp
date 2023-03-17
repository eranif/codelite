//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : ThemeImporterCobra.h
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

#ifndef ECLIPSECOBRATHEMEIMPORTER_H
#define ECLIPSECOBRATHEMEIMPORTER_H

#include "ThemeImporterPython.hpp" // Base class: ThemeImporterPython
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ThemeImporterCobra : public ThemeImporterPython
{
public:
    ThemeImporterCobra();
    virtual ~ThemeImporterCobra();
};

#endif // ECLIPSECOBRATHEMEIMPORTER_H
