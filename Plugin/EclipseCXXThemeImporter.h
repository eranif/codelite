//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : EclipseCXXThemeImporter.h
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

#ifndef ECLIPSECXXTHEMEIMPORTER_H
#define ECLIPSECXXTHEMEIMPORTER_H

#include "EclipseThemeImporterBase.h" // Base class: EclipseThemeImporterBase
#include "codelite_exports.h"
#include <vector>

class WXDLLIMPEXP_SDK EclipseCXXThemeImporter : public EclipseThemeImporterBase
{
private:
    /**
     * @brief convert a lexer object into eclipse XML format
     */
    static wxFileName ToEclipseXML(LexerConf::Ptr_t cxxLexer, size_t id);
    
public:
    EclipseCXXThemeImporter();
    virtual ~EclipseCXXThemeImporter();
    
    /**
     * @brief convert _all_ C++ xml files into eclipse format
     */
    static std::vector<wxFileName> ToEclipseXMLs();
    
    /**
     * @brief import an eclipse XML colour theme
     * @param eclipseXmlFile
     * @param codeliteXml [output] the output file name
     */
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile);
};

#endif // ECLIPSECXXTHEMEIMPORTER_H
