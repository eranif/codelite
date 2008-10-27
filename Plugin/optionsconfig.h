//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : optionsconfig.h
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

#ifndef OPTIONS_CONFIG_H
#define OPTIONS_CONFIG_H

#include "wx/string.h"
#include "wx/xml/xml.h"
#include "wx/colour.h"
#include "wx/font.h"
#include "configuration_object.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

class WXDLLIMPEXP_LE_SDK OptionsConfig : public ConfObject
{
    bool m_displayFoldMargin;
    bool m_underlineFoldLine;
    wxString m_foldStyle;
    bool m_displayBookmarkMargin;
    wxString m_bookmarkShape;
    wxColour m_bookmarkBgColour;
    wxColour m_bookmarkFgColour;
    bool m_highlightCaretLine;
    bool m_displayLineNumbers;
    bool m_showIndentationGuidelines;
    wxColour m_caretLineColour;
    bool	m_indentUsesTabs;
    int		m_iconsSize;
    wxFontEncoding m_fileFontEncoding;
    int m_showWhitspaces;
    bool m_foldCompact;
    bool m_foldAtElse;
    bool m_foldPreprocessor;
    int m_edgeMode;
    int m_edgeColumn;
    wxColour m_edgeColour;

public:
    OptionsConfig(wxXmlNode *node);
    virtual ~OptionsConfig(void);

    //-------------------------------------
    // Setters/Getters
    //-------------------------------------
    bool GetDisplayFoldMargin() const {
        return m_displayFoldMargin;
    }
    bool GetUnderlineFoldLine() const {
        return m_underlineFoldLine;
    }
    wxString GetFoldStyle() const {
        return m_foldStyle;
    }
    bool GetDisplayBookmarkMargin() const {
        return m_displayBookmarkMargin;
    }
    wxString GetBookmarkShape() const {
        return m_bookmarkShape;
    }
    wxColour GetBookmarkFgColour() const {
        return m_bookmarkFgColour;
    }
    wxColour GetBookmarkBgColour() const {
        return m_bookmarkBgColour;
    }
    bool GetHighlightCaretLine() const {
        return m_highlightCaretLine;
    }
    bool GetDisplayLineNumbers() const {
        return m_displayLineNumbers;
    }
    bool GetShowIndentationGuidelines() const {
        return m_showIndentationGuidelines;
    }
    wxColour GetCaretLineColour() const {
        return m_caretLineColour;
    }


    void SetDisplayFoldMargin(bool b) {
        m_displayFoldMargin = b;
    }
    void SetUnderlineFoldLine(bool b) {
        m_underlineFoldLine = b;
    }
    void SetFoldStyle(wxString s) {
        m_foldStyle = s;
    }
    void SetDisplayBookmarkMargin(bool b) {
        m_displayBookmarkMargin = b;
    }
    void SetBookmarkShape(wxString s) {
        m_bookmarkShape = s;
    }
    void SetBookmarkFgColour(wxColour c) {
        m_bookmarkFgColour = c;
    }
    void SetBookmarkBgColour(wxColour c) {
        m_bookmarkBgColour = c;
    }
    void SetHighlightCaretLine(bool b) {
        m_highlightCaretLine = b;
    }
    void SetDisplayLineNumbers(bool b) {
        m_displayLineNumbers = b;
    }
    void SetShowIndentationGuidelines(bool b) {
        m_showIndentationGuidelines = b;
    }
    void SetCaretLineColour(wxColour c) {
        m_caretLineColour = c;
    }

    void SetIndentUsesTabs(const bool& indentUsesTabs) {
        this->m_indentUsesTabs = indentUsesTabs;
    }
    const bool& GetIndentUsesTabs() const {
        return m_indentUsesTabs;
    }

    void SetIconsSize(const int& iconsSize) {
        this->m_iconsSize = iconsSize;
    }
    const int& GetIconsSize() const {
        return m_iconsSize;
    }

    void SetFileFontEncoding(const wxString& strFileFontEncoding);
    wxFontEncoding GetFileFontEncoding() const {
        return m_fileFontEncoding;
    }

    void SetShowWhitspaces(const int& showWhitspaces) {
        this->m_showWhitspaces = showWhitspaces;
    }
    const int& GetShowWhitspaces() const {
        return m_showWhitspaces;
    }

    void SetFoldAtElse(const bool& foldAtElse) {
        this->m_foldAtElse = foldAtElse;
    }
    void SetFoldCompact(const bool& foldCompact) {
        this->m_foldCompact = foldCompact;
    }
    const bool& GetFoldAtElse() const {
        return m_foldAtElse;
    }
    const bool& GetFoldCompact() const {
        return m_foldCompact;
    }
    void SetFoldPreprocessor(const bool& foldPreprocessor) {
        this->m_foldPreprocessor = foldPreprocessor;
    }
    const bool& GetFoldPreprocessor() const {
        return m_foldPreprocessor;
    }

    void SetEdgeColour(const wxColour& edgeColour) {
        this->m_edgeColour = edgeColour;
    }
    void SetEdgeColumn(int edgeColumn) {
        this->m_edgeColumn = edgeColumn;
    }
    void SetEdgeMode(int edgeMode) {
        this->m_edgeMode = edgeMode;
    }
    
    const wxColour& GetEdgeColour() const {
        return m_edgeColour;
    }
    int GetEdgeColumn() const {
        return m_edgeColumn;
    }
    int GetEdgeMode() const {
        return m_edgeMode;
    }
    
    /**
     * Return an XML representation of this object
     * \return XML node
     */
    wxXmlNode *ToXml() const;
};

typedef SmartPtr<OptionsConfig> OptionsConfigPtr;

#endif // OPTIONS_CONFIG_H
