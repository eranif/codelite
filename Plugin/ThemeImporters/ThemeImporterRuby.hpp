//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2021 Eran Ifrah
// File name            : ThemeImporterRuby.hpp
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

#ifndef ECLIPSERUBYTHEMEIMPORTER_H
#define ECLIPSERUBYTHEMEIMPORTER_H

#include "ThemeImporterBase.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK ThemeImporterRuby : public ThemeImporterBase
{
public:
    virtual LexerConf::Ptr_t Import(const wxFileName& theme_file);
    ThemeImporterRuby();
    ~ThemeImporterRuby();
};

#endif // ECLIPSERUBYTHEMEIMPORTER_H
