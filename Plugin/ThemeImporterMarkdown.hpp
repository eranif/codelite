//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2022 Eran Ifrah
// File name            : ThemeImporterMarkdown.h
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

#ifndef ECLIPSEMARKDOWNTHEMEIMPORTER_H
#define ECLIPSEMARKDOWNTHEMEIMPORTER_H

#include "ThemeImporterBase.hpp" // Base class: ThemeImporterBase
#include "codelite_exports.h"

#include <vector>

class WXDLLIMPEXP_SDK ThemeImporterMarkdown : public ThemeImporterBase
{
private:
    /**
     * @brief convert a lexer object into eclipse XML format
     */
    static wxFileName ToEclipseXML(LexerConf::Ptr_t cxxLexer, size_t id);

public:
    ThemeImporterMarkdown();
    virtual ~ThemeImporterMarkdown();

    /**
     * @brief import an eclipse XML colour theme
     * @param theme_file
     * @param codeliteXml [output] the output file name
     */
    LexerConf::Ptr_t Import(const wxFileName& theme_file) override;
};

#endif // ECLIPSECXXTHEMEIMPORTER_H
