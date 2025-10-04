//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : ThemeImporterSCSS.h
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

#ifndef ECLIPSESCSSTHEMEIMPORTER_H
#define ECLIPSESCSSTHEMEIMPORTER_H

#include "ThemeImporterBase.hpp"

class WXDLLIMPEXP_SDK ThemeImporterSCSS : public ThemeImporterBase
{
public:
    ThemeImporterSCSS();
    virtual ~ThemeImporterSCSS() = default;

public:
    virtual LexerConf::Ptr_t Import(const wxFileName& theme_file);
};

#endif // ECLIPSESCSSTHEMEIMPORTER_H
