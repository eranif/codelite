//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : EclipseThemeImporterBase.h
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

#include "codelite_exports.h"
#include <wx/xml/xml.h>
#include <wx/string.h>
#include <wx/filename.h>
#include <list>
#include "smart_ptr.h"
#include "macros.h"
#include "lexer_configuration.h"
#include "JSON.h"

class WXDLLIMPEXP_SDK EclipseThemeImporterBase
{
public:
    struct Property {
        wxString colour;
        bool isBold;
        bool isItalic;
    };
    typedef SmartPtr<EclipseThemeImporterBase> Ptr_t;
    typedef std::list<EclipseThemeImporterBase::Ptr_t> List_t;

protected:
    wxXmlDocument m_doc;

    wxString m_keywords0;
    wxString m_keywords1;
    wxString m_keywords2;
    wxString m_keywords3;
    wxString m_keywords4;
    wxString m_fileExtensions;
    Property m_background;
    Property m_foreground;
    Property m_lineNumber;
    Property m_selectionBackground;
    Property m_selectionForeground;
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
    wxString m_langName;

protected:
    void AddProperty(LexerConf::Ptr_t lexer,
                     const wxString& id,
                     const wxString& name,
                     const wxString& colour,
                     const wxString& bgColour,
                     bool bold = false,
                     bool italic = false,
                     bool isEOLFilled = false);

    void AddProperty(LexerConf::Ptr_t lexer,
                     int id,
                     const wxString& name,
                     const wxString& colour,
                     const wxString& bgColour,
                     bool bold = false,
                     bool italic = false,
                     bool isEOLFilled = false)
    {
        AddProperty(lexer, wxString::Format("%d", id), name, colour, bgColour, bold, italic, isEOLFilled);
    }

    void AddBaseProperties(LexerConf::Ptr_t lexer, const wxString& lang, const wxString& id);

    void AddCommonProperties(LexerConf::Ptr_t lexer);
    void DoSetKeywords(wxString& wordset, const wxString& words);

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
    const wxString& GetKeywords0() const { return m_keywords0; }
    const wxString& GetKeywords1() const { return m_keywords1; }
    const wxString& GetKeywords2() const { return m_keywords2; }
    const wxString& GetKeywords3() const { return m_keywords3; }
    const wxString& GetKeywords4() const { return m_keywords4; }

public:
    EclipseThemeImporterBase();
    virtual ~EclipseThemeImporterBase();
    /**
     * @brief load eclispe theme (in xml format)
     */
    LexerConf::Ptr_t InitializeImport(const wxFileName& eclipseXml, const wxString& langName, int langId);

    /**
     * @brief Finalize the import by adding the common lexer part (such as the fold margin, text selection etc)
     * and saving it the file system
     */
    void FinalizeImport(LexerConf::Ptr_t lexer);

    /**
     * @brief get attributes of a given property
     * @param name the property name
     * @param prop [output]
     */
    bool GetProperty(const wxString& name, EclipseThemeImporterBase::Property& prop) const;

    /**
     * @brief return true if the theme is valid
     */
    bool IsValid() const { return m_doc.IsOk(); }

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
     * @param eclipseXmlFile
     * @param codeliteXml [output] the output file name
     */
    virtual LexerConf::Ptr_t Import(const wxFileName& eclipseXmlFile) = 0;
};

#endif // ECLIPSETHEMEIMPORTERBASE_H
