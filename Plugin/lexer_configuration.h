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
#include "JSON.h"

#define ANNOTATION_STYLE_WARNING 210
#define ANNOTATION_STYLE_ERROR 211
#define ANNOTATION_STYLE_CC_ERROR 212

class WXDLLIMPEXP_SDK LexerConf
{
    StyleProperty::Map_t m_properties;
    int m_lexerId;
    wxString m_name;
    wxString m_extension;
    wxString m_keyWords[10];
    wxString m_themeName;
    size_t m_flags;

public:
    typedef SmartPtr<LexerConf> Ptr_t;

protected:
    enum eLexerConfFlags {
        kNone = 0,
        kStyleInPP = (1 << 0),
        kIsActive = (1 << 1),
        kUseCustomTextSelectionFgColour = (1 << 2),
    };

    inline void EnableFlag(eLexerConfFlags flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    inline bool HasFlag(eLexerConfFlags flag) const { return m_flags & flag; }

public:
    struct FindByNameAndTheme {
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

    /**
     * @brief convert the lexer settings into a JSON object
     */
    JSONItem ToJSON(bool forExport = false) const;

    /**
     * @brief construt this object from a JSON object
     * @param json
     */
    void FromJSON(const JSONItem& json);

public:
    LexerConf();
    virtual ~LexerConf();

    void SetUseCustomTextSelectionFgColour(bool b) { EnableFlag(kUseCustomTextSelectionFgColour, b); }
    bool IsUseCustomTextSelectionFgColour() const { return HasFlag(kUseCustomTextSelectionFgColour); }
    void SetStyleWithinPreProcessor(bool b) { EnableFlag(kStyleInPP, b); }
    bool GetStyleWithinPreProcessor() const { return HasFlag(kStyleInPP); }
    void SetIsActive(bool b) { EnableFlag(kIsActive, b); }
    bool IsActive() const { return HasFlag(kIsActive); }

    void SetThemeName(const wxString& themeName) { this->m_themeName = themeName; }
    const wxString& GetThemeName() const { return m_themeName; }

    /**
     * @brief return true if the colours represented by this lexer are a "dark" theme
     */
    bool IsDark() const;
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

    /**
     * Set the lexer ID
     * \param id
     */
    void SetLexerId(int id) { m_lexerId = id; }

    /**
     * Return the lexer description as described in the XML file
     */
    const wxString& GetName() const { return m_name; }

    void SetName(const wxString& name) { m_name = name; }
    /**
     * Return the lexer keywords
     * \return
     */
    const wxString& GetKeyWords(int set) const { return m_keyWords[set]; }

    void SetKeyWords(const wxString& keywords, int set);

    /**
     * File patterns that this lexer should apply to
     */
    const wxString& GetFileSpec() const { return m_extension; }
    /**
     * Return a list of the lexer properties
     * \return
     */
    const StyleProperty::Map_t& GetLexerProperties() const { return m_properties; }

    /**
     * Return a list of the lexer properties
     * \return
     */
    StyleProperty::Map_t& GetLexerProperties() { return m_properties; }

    /**
     * @brief return property. Check for IsNull() to make sure we got a valid property
     * @param propertyId
     * @return
     */
    StyleProperty& GetProperty(int propertyId);
    const StyleProperty& GetProperty(int propertyId) const;

    /**
     * @brief set the line numbers colour
     */
    void SetLineNumbersFgColour(const wxColour& colour);

    /**
     * @brief set the default fg colour
     */
    void SetDefaultFgColour(const wxColour& colour);

    /**
     * Set the lexer properties
     * \param &properties
     */
    void SetProperties(StyleProperty::Map_t& properties) { m_properties.swap(properties); }
    /**
     * Set file spec for the lexer
     * \param &spec
     */
    void SetFileSpec(const wxString& spec) { m_extension = spec; }

    /**
     * @brief return the font for a given style id
     * @return return wxNullFont if error occurred or could locate the style
     */
    wxFont GetFontForSyle(int styleId) const;
};

#endif // LEXER_CONFIGURATION_H
