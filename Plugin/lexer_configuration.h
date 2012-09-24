//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : lexer_configuration.h
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
#ifndef LEXER_CONFIGURATION_H
#define LEXER_CONFIGURATION_H

#include "wx/string.h"
#include "wx/filename.h"
#include "attribute_style.h"
#include "wx/xml/xml.h"
#include <wx/font.h>
#include "smart_ptr.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK LexerConf
{
    StylePropertyList m_properties;
    int               m_lexerId;
    wxString          m_name;
    wxString          m_extension;
    wxString          m_keyWords[10];
    bool              m_styleWithinPreProcessor;


public:
    // Return an xml representation from this object
    wxXmlNode *ToXml() const;

    // Parse lexer object from xml node
    void FromXml(wxXmlNode *node);

public:
    LexerConf();
    virtual ~LexerConf();

    /**
     * Get the lexer ID, which should be in sync with values of Scintilla
     * \return
     */
    int GetLexerId() const {
        return m_lexerId;
    }

    void SetStyleWithinPreProcessor(bool styleWithinPreProcessor) {
        this->m_styleWithinPreProcessor = styleWithinPreProcessor;
    }

    bool GetStyleWithinPreProcessor() const {
        return m_styleWithinPreProcessor;
    }

    /**
     * Set the lexer ID
     * \param id
     */
    void SetLexerId(int id) {
        m_lexerId = id;
    }

    /**
     * Return the lexer description as described in the XML file
     */
    const wxString &GetName() const {
        return m_name;
    }
    /**
     * Return the lexer keywords
     * \return
     */
    const wxString &GetKeyWords(int set) const {
        return m_keyWords[set];
    }

    void SetKeyWords(const wxString &keywords, int set) {
        m_keyWords[set] = keywords;
    }

    /**
     * File patterns that this lexer should apply to
     */
    const wxString &GetFileSpec() const {
        return m_extension;
    }
    /**
     * Return a list of the lexer properties
     * \return
     */
    const StylePropertyList &GetProperties() const {
        return m_properties;
    }
    /**
     * Set the lexer properties
     * \param &properties
     */
    void SetProperties(StylePropertyList &properties) {
        m_properties = properties;
    }
    /**
     * Set file spec for the lexer
     * \param &spec
     */
    void SetFileSpec(const wxString &spec) {
        m_extension = spec;
    }
    
    /**
     * @brief return the font for a given style id
     * @return return wxNullFont if error occured or could locate the style
     */
    wxFont GetFontForSyle(int styleId) const;
};

typedef SmartPtr<LexerConf> LexerConfPtr;

#endif // LEXER_CONFIGURATION_H
