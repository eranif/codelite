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

#include "JSON.h"
#include "attribute_style.h"
#include "codelite_exports.h"
#include "wx/filename.h"
#include "wx/string.h"
#include "wx/xml/xml.h"

#include <smart_ptr.h>
#include <wx/font.h>
#include <wx/sharedptr.h>
#include <wx/stc/stc.h>

#define ANNOTATION_STYLE_WARNING 210
#define ANNOTATION_STYLE_ERROR 211
#define ANNOTATION_STYLE_CC_ERROR 212

struct WXDLLIMPEXP_SDK WordSetIndex {
    int index = wxNOT_FOUND;
    // when set to true, the `index` is the style to which we append substyles
    bool is_substyle = false;
    WordSetIndex(int idx, bool b)
        : index(idx)
        , is_substyle(b)
    {
    }
    WordSetIndex() {}
    JSONItem to_json() const
    {
        auto item = JSONItem::createObject();
        item.addProperty("index", index);
        item.addProperty("is_substyle", is_substyle);
        return item;
    }

    void from_json(const JSONItem& json)
    {
        if(json.isNumber()) {
            // old style, for migration purposes
            index = json.toInt(wxNOT_FOUND);
        } else {
            index = json["index"].toInt(wxNOT_FOUND);
            is_substyle = json["is_substyle"].toBool(false);
        }
    }

    bool is_ok() const { return index != wxNOT_FOUND; }
};

class WXDLLIMPEXP_SDK LexerConf
{
public:
    enum eWordSetIndex {
        WS_FIRST = 0,
        WS_CLASS = WS_FIRST,
        WS_FUNCTIONS,
        WS_VARIABLES,
        WS_OTHERS,
        WS_LAST = WS_OTHERS,
    };

private:
    StyleProperty::Vec_t m_properties;
    int m_lexerId;
    wxString m_name;
    wxString m_extension;
    wxString m_keyWords[10];
    wxString m_themeName;
    size_t m_flags = 0;

    WordSetIndex m_wordSets[4];
    int m_substyleBase = wxNOT_FOUND;

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
    void SetSubstyleBase(int style) { m_substyleBase = style; }
    int GetSubStyleBase() const { return m_substyleBase; }
    bool IsSubstyleSupported() const { return m_substyleBase != wxNOT_FOUND; }

    /**
     * @brief convert the lexer settings into a JSON object
     */
    JSONItem ToJSON(bool forExport = false) const;

    /**
     * @brief construt this object from a JSON object
     * @param json
     */
    void FromJSON(const JSONItem& json);

    void SetWordSet(eWordSetIndex index, const WordSetIndex& word_set) { this->m_wordSets[index] = word_set; }
    const WordSetIndex& GetWordSet(eWordSetIndex index) const { return m_wordSets[index]; }
    void ApplyWordSet(wxStyledTextCtrl* ctrl, eWordSetIndex index, const wxString& keywords);

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
     * @brief similar to `Apply`, but use the system colours instead of the theme colours
     */
    void ApplySystemColours(wxStyledTextCtrl* ctrl);

    /**
     * Get the lexer ID, which should be in sync with values of Scintilla
     * @return
     */
    int GetLexerId() const { return m_lexerId; }

    /**
     * Set the lexer ID
     * @param id
     */
    void SetLexerId(int id) { m_lexerId = id; }

    /**
     * Return the lexer description as described in the XML file
     */
    const wxString& GetName() const { return m_name; }

    void SetName(const wxString& name) { m_name = name; }
    /**
     * Return the lexer keywords
     * @return
     */
    const wxString& GetKeyWords(int set) const { return m_keyWords[set]; }

    void SetKeyWords(const wxString& keywords, int set);

    /**
     * File patterns that this lexer should apply to
     */
    const wxString& GetFileSpec() const { return m_extension; }
    /**
     * Return a list of the lexer properties
     * @return
     */
    const StyleProperty::Vec_t& GetLexerProperties() const { return m_properties; }

    /**
     * Return a list of the lexer properties
     * @return
     */
    StyleProperty::Vec_t& GetLexerProperties() { return m_properties; }

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
     * @param &properties
     */
    void SetProperties(StyleProperty::Vec_t& properties) { m_properties.swap(properties); }
    /**
     * Set file spec for the lexer
     * @param &spec
     */
    void SetFileSpec(const wxString& spec) { m_extension = spec; }

    /**
     * @brief return the font for a given style id
     * @return return wxNullFont if error occurred or could locate the style
     */
    wxFont GetFontForStyle(int styleId, const wxWindow* win) const;
};

#endif // LEXER_CONFIGURATION_H
