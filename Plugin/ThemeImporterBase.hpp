//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : ThemeImporterBase.h
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

#ifndef ECLIPSETHEMEIMPORTERBASE_H
#define ECLIPSETHEMEIMPORTERBASE_H

#include "JSON.h"
#include "codelite_exports.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "smart_ptr.h"

#include <list>
#include <unordered_map>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/xml/xml.h>

class WXDLLIMPEXP_SDK ThemeImporterBase
{
public:
    struct Property {
        wxString fg_colour;
        wxString bg_colour;
        bool isBold = false;
        bool isItalic = false;
    };

    struct EclipseProperty {
        wxString color;
        bool isBold = false;
        bool isItalic = false;
    };

    typedef SmartPtr<ThemeImporterBase> Ptr_t;
    typedef std::list<ThemeImporterBase::Ptr_t> List_t;

protected:
    wxString m_keywords0;
    wxString m_keywords1;
    wxString m_keywords2;
    wxString m_keywords3;
    wxString m_keywords4;
    wxString m_keywords5;
    wxString m_fileExtensions;
    Property m_editor;
    Property m_lineNumber;
    Property m_lineNumberActive;
    Property m_selection;
    Property m_caret;
    Property m_singleLineComment;
    Property m_multiLineComment;
    Property m_number;
    Property m_string;
    Property m_oper;
    Property m_keyword;
    Property m_klass;
    Property m_variable;
    Property m_javadoc;
    Property m_javadocKeyword;
    Property m_function;
    Property m_field;
    Property m_enum;
    wxString m_langName;
    wxString m_themeName;
    bool m_isDarkTheme = false;
    std::unordered_map<wxString, EclipseProperty> m_xmlProperties;

private:
    // the index for special word sets
    WordSetIndex m_classesIndex;
    WordSetIndex m_functionsIndex;
    WordSetIndex m_localsIndex;
    WordSetIndex m_othersIndex;

protected:
    void AddProperty(LexerConf::Ptr_t lexer, const wxString& id, const wxString& name, const wxString& colour,
                     const wxString& bgColour, bool bold = false, bool italic = false, bool isEOLFilled = false);

    void AddPropertySubstyle(LexerConf::Ptr_t lexer, int id, const wxString& name, const Property& prop);

    void AddProperty(LexerConf::Ptr_t lexer, const wxString& id, const wxString& name, const Property& prop)
    {
        AddProperty(lexer, id, name, prop.fg_colour, prop.bg_colour.empty() ? m_editor.bg_colour : prop.bg_colour,
                    prop.isBold, prop.isItalic, false);
    }

    void AddProperty(LexerConf::Ptr_t lexer, int id, const wxString& name, const Property& prop)
    {
        AddProperty(lexer, wxString() << id, name, prop);
    }

    void AddProperty(LexerConf::Ptr_t lexer, int id, const wxString& name, const wxString& colour,
                     const wxString& bgColour, bool bold = false, bool italic = false, bool isEOLFilled = false)
    {
        AddProperty(lexer, wxString() << id, name, colour, bgColour, bold, italic, isEOLFilled);
    }

    void AddBaseProperties(LexerConf::Ptr_t lexer, const wxString& lang, const wxString& id);

    void AddCommonProperties(LexerConf::Ptr_t lexer);
    void DoSetKeywords(wxString& wordset, const wxString& words);
    LexerConf::Ptr_t ImportEclipseXML(const wxFileName& theme_file, const wxString& langName, int langId);
    LexerConf::Ptr_t ImportVSCodeJSON(const wxFileName& theme_file, const wxString& langName, int langId);
    void GetVSCodeColour(const wxStringMap_t& scopes_to_colours_map, const std::vector<wxString>& scopes,
                         Property& colour);
    void GetEditorVSCodeColour(JSONItem& colours, const wxString& bg_prop, const wxString& fg_prop, Property& colour);

private:
    /**
     * @brief read the background / foreground properties from the XML into `prop`
     * if a property is not found, we use the `m_editor` proprties
     */
    void GetEclipseXmlProperty(const wxString& bg_prop, const wxString& fg_prop,
                               ThemeImporterBase::Property& prop) const;

public:
    const wxString& GetLangName() const { return m_langName; }
    void SetLangName(const wxString& langName) { this->m_langName = langName; }

    // Setters/Getters
    void SetFileExtensions(const wxString& fileExtensions) { this->m_fileExtensions = fileExtensions; }
    const wxString& GetFileExtensions() const { return m_fileExtensions; }
    void SetKeywords0(const wxString& keywords0) { DoSetKeywords(this->m_keywords0, keywords0); }
    void SetKeywords1(const wxString& keywords1) { DoSetKeywords(this->m_keywords1, keywords1); }
    void SetKeywords2(const wxString& keywords2) { DoSetKeywords(this->m_keywords2, keywords2); }
    void SetKeywords3(const wxString& keywords3) { DoSetKeywords(this->m_keywords3, keywords3); }
    void SetKeywords4(const wxString& keywords4) { DoSetKeywords(this->m_keywords4, keywords4); }
    void SetKeywords5(const wxString& keywords4) { DoSetKeywords(this->m_keywords5, keywords4); }

    const wxString& GetKeywords0() const { return m_keywords0; }
    const wxString& GetKeywords1() const { return m_keywords1; }
    const wxString& GetKeywords2() const { return m_keywords2; }
    const wxString& GetKeywords3() const { return m_keywords3; }
    const wxString& GetKeywords4() const { return m_keywords4; }
    const wxString& GetKeywords5() const { return m_keywords5; }

    const WordSetIndex& GetClassWordSetIndex() const { return m_classesIndex; }
    const WordSetIndex& GetLocalsSetIndex() const { return m_localsIndex; }
    const WordSetIndex& GetFunctionWordSetIndex() const { return m_functionsIndex; }
    const WordSetIndex& GetOthersWordSetIndex() const { return m_othersIndex; }

    void SetClassWordSetIndex(int index, bool is_substyle = false)
    {
        m_classesIndex.is_substyle = is_substyle;
        m_classesIndex.index = index;
    }

    void SetLocalsWordSetIndex(int index, bool is_substyle = false)
    {
        m_localsIndex.is_substyle = is_substyle;
        m_localsIndex.index = index;
    }

    void SetFunctionsWordSetIndex(int index, bool is_substyle = false)
    {
        m_functionsIndex.is_substyle = is_substyle;
        m_functionsIndex.index = index;
    }

    void SetOthersWordSetIndex(int index, bool is_substyle = false)
    {
        m_othersIndex.is_substyle = is_substyle;
        m_othersIndex.index = index;
    }

public:
    ThemeImporterBase();
    virtual ~ThemeImporterBase();
    /**
     * @brief intiailise the import by reading base parts of the lexer
     */
    LexerConf::Ptr_t InitializeImport(const wxFileName& theme_file, const wxString& langName, int langId);

    /**
     * @brief Finalize the import by adding the common lexer part (such as the fold margin, text selection etc)
     * and saving it the file system
     */
    void FinalizeImport(LexerConf::Ptr_t lexer);

    /**
     * @brief return true if the loaded theme is dark or bright
     */
    bool IsDarkTheme() const;

    /**
     * @brief return the theme name
     */
    wxString GetName() const;

    /**
     * @brief get the lexer output file (name and extension only)
     * @param language
     * @return
     */
    wxString GetOutputFile(const wxString& language) const;

    /**
     * @brief import an eclipse XML colour theme
     * @param theme_file
     * @param codeliteXml [output] the output file name
     */
    virtual LexerConf::Ptr_t Import(const wxFileName& theme_file) = 0;
};

#endif // ECLIPSETHEMEIMPORTERBASE_H
