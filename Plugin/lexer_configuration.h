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
#include "codelite_exports.h"
#include <wx/stc/stc.h>
#include <wx/sharedptr.h>
#include <smart_ptr.h>

class WXDLLIMPEXP_SDK LexerConf
{
    StyleProperty::List_t m_properties;
    int m_lexerId;
    wxString m_name;
    wxString m_extension;
    wxString m_keyWords[10];
    bool m_styleWithinPreProcessor;
    wxString m_themeName;
    bool m_isActive;
    bool m_useCustomTextSelectionFgColour;

public:
    typedef SmartPtr<LexerConf> Ptr_t;

public:
    struct FindByNameAndTheme
    {
        wxString m_name;
        wxString m_theme;
        FindByNameAndTheme(const wxString& name, const wxString& theme)
            : m_name(name)
            , m_theme(theme)
        {
        }

        bool operator()(LexerConf::Ptr_t lexer) const
        {
            return lexer->GetName() == m_name && lexer->GetThemeName() == m_theme;
        }
    };

public:
    // Return an xml representation from this object
    wxXmlNode* ToXml() const;

    // Parse lexer object from xml node
    void FromXml(wxXmlNode* node);

public:
    LexerConf();
    virtual ~LexerConf();

    void SetUseCustomTextSelectionFgColour(bool useCustomTextSelectionFgColour)
    {
        this->m_useCustomTextSelectionFgColour = useCustomTextSelectionFgColour;
    }
    bool IsUseCustomTextSelectionFgColour() const { return m_useCustomTextSelectionFgColour; }
    void SetIsActive(bool isActive) { this->m_isActive = isActive; }
    void SetThemeName(const wxString& themeName) { this->m_themeName = themeName; }
    bool IsActive() const { return m_isActive; }
    const wxString& GetThemeName() const { return m_themeName; }
    /**
     * @brief apply the current lexer configuration on an input
     * wxStyledTextCtrl
     */
    void Apply(wxStyledTextCtrl* ctrl, bool applyKeywords = false);

    /**
     * Get the lexer ID, which should be in sync with values of Scintilla
     * \return
     */
    int GetLexerId() const { return m_lexerId; }

    void SetStyleWithinPreProcessor(bool styleWithinPreProcessor)
    {
        this->m_styleWithinPreProcessor = styleWithinPreProcessor;
    }

    bool GetStyleWithinPreProcessor() const { return m_styleWithinPreProcessor; }

    /**
     * Set the lexer ID
     * \param id
     */
    void SetLexerId(int id) { m_lexerId = id; }

    /**
     * Return the lexer description as described in the XML file
     */
    const wxString& GetName() const { return m_name; }
    /**
     * Return the lexer keywords
     * \return
     */
    const wxString& GetKeyWords(int set) const { return m_keyWords[set]; }

    void SetKeyWords(const wxString& keywords, int set) { m_keyWords[set] = keywords; }

    /**
     * File patterns that this lexer should apply to
     */
    const wxString& GetFileSpec() const { return m_extension; }
    /**
     * Return a list of the lexer properties
     * \return
     */
    const StyleProperty::List_t& GetLexerProperties() const { return m_properties; }

    /**
     * Return a list of the lexer properties
     * \return
     */
    StyleProperty::List_t& GetLexerProperties() { return m_properties; }

    /**
     * @brief return property. Check for IsNull() to make sure we got a valid property
     * @param propertyId
     * @return
     */
    StyleProperty& GetProperty(int propertyId);

    /**
     * Set the lexer properties
     * \param &properties
     */
    void SetProperties(StyleProperty::List_t& properties) { m_properties = properties; }
    /**
     * Set file spec for the lexer
     * \param &spec
     */
    void SetFileSpec(const wxString& spec) { m_extension = spec; }

    /**
     * @brief return the font for a given style id
     * @return return wxNullFont if error occured or could locate the style
     */
    wxFont GetFontForSyle(int styleId) const;
};

#endif // LEXER_CONFIGURATION_H
