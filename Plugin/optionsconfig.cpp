//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : optionsconfig.cpp
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
#include "optionsconfig.h"
#include <wx/fontmap.h>
#include "xmlutils.h"
#include "macros.h"


OptionsConfig::OptionsConfig(wxXmlNode *node)
		: m_displayFoldMargin(true)
		, m_underlineFoldLine(false)
		, m_foldStyle(wxT("Arrows with Background Colour"))
		, m_displayBookmarkMargin(true)
		, m_bookmarkShape(wxT("Small Arrow"))
		, m_bookmarkBgColour(wxColour(12, 133, 222))
		, m_bookmarkFgColour(wxColour(66, 169, 244))
		, m_highlightCaretLine(true)
		, m_displayLineNumbers(false)
		, m_showIndentationGuidelines(false)
		, m_caretLineColour(wxT("LIGHT BLUE"))
		, m_indentUsesTabs(true)
        , m_indentWidth(4)
        , m_tabWidth(4)
		, m_iconsSize(24)
		, m_showWhitspaces(0/*wxSCI_WS_INVISIBLE*/)
		, m_foldCompact(false)
		, m_foldAtElse(false)
		, m_foldPreprocessor(false)
        , m_edgeMode(0/*wxSCI_EDGE_NONE*/)
        , m_edgeColumn(80)
        , m_edgeColour(wxColour(wxT("LIGHT GREY")))
		, m_highlightMatchedBraces(true)
		, m_autoAddMatchedBraces(true)
		, m_foldBgColour(wxColour(240, 240, 240))
{
	//set the default font name to be UTF8
	SetFileFontEncoding(wxFontMapper::GetEncodingName(wxFONTENCODING_UTF8));

	if ( node ) {
		m_displayFoldMargin = XmlUtils::ReadBool(node, wxT("DisplayFoldMargin"), m_displayFoldMargin);
		m_underlineFoldLine = XmlUtils::ReadBool(node, wxT("UnderlineFoldedLine"), m_underlineFoldLine);
		m_foldStyle = XmlUtils::ReadString(node, wxT("FoldStyle"), m_foldStyle);
		m_displayBookmarkMargin = XmlUtils::ReadBool(node, wxT("DisplayBookmarkMargin"), m_displayBookmarkMargin);
		m_bookmarkShape = XmlUtils::ReadString(node, wxT("BookmarkShape"),m_bookmarkShape);
		m_bookmarkBgColour = XmlUtils::ReadString(node, wxT("BookmarkBgColour"), m_bookmarkBgColour.GetAsString(wxC2S_HTML_SYNTAX));
		m_bookmarkFgColour = XmlUtils::ReadString(node, wxT("BookmarkFgColour"), m_bookmarkFgColour.GetAsString(wxC2S_HTML_SYNTAX));
		m_highlightCaretLine = XmlUtils::ReadBool(node, wxT("HighlightCaretLine"), m_highlightCaretLine);
		m_displayLineNumbers = XmlUtils::ReadBool(node, wxT("ShowLineNumber"), m_displayLineNumbers);
		m_showIndentationGuidelines = XmlUtils::ReadBool(node, wxT("IndentationGuides"), m_showIndentationGuidelines);
		m_caretLineColour = XmlUtils::ReadString(node, wxT("CaretLineColour"), m_caretLineColour.GetAsString(wxC2S_HTML_SYNTAX));
		m_indentUsesTabs = XmlUtils::ReadBool(node, wxT("IndentUsesTabs"), m_indentUsesTabs);
        m_indentWidth = XmlUtils::ReadLong(node, wxT("IndentWidth"), m_indentWidth);
        m_tabWidth = XmlUtils::ReadLong(node, wxT("TabWidth"), m_tabWidth);
		m_iconsSize = XmlUtils::ReadLong(node, wxT("ToolbarIconSize"), m_iconsSize);
		m_showWhitspaces = XmlUtils::ReadLong(node, wxT("ShowWhitespaces"), m_showWhitspaces);
		m_foldCompact = XmlUtils::ReadBool(node, wxT("FoldCompact"), m_foldCompact);
		m_foldAtElse = XmlUtils::ReadBool(node, wxT("FoldAtElse"), m_foldAtElse);
		m_foldPreprocessor = XmlUtils::ReadBool(node, wxT("FoldPreprocessor"), m_foldPreprocessor);
		SetFileFontEncoding(XmlUtils::ReadString(node, wxT("FileFontEncoding"), wxFontMapper::GetEncodingName(wxFONTENCODING_UTF8)));
        m_edgeMode = XmlUtils::ReadLong(node, wxT("EdgeMode"), m_edgeMode);
        m_edgeColumn = XmlUtils::ReadLong(node, wxT("EdgeColumn"), m_edgeColumn);
        m_edgeColour = XmlUtils::ReadString(node, wxT("EdgeColour"), m_edgeColour.GetAsString(wxC2S_HTML_SYNTAX));
		m_highlightMatchedBraces = XmlUtils::ReadBool(node, wxT("HighlightMatchedBraces"), m_highlightMatchedBraces);
		m_autoAddMatchedBraces = XmlUtils::ReadBool(node, wxT("AutoAddMatchedBraces"), m_autoAddMatchedBraces);
		m_foldBgColour = XmlUtils::ReadString(node, wxT("FoldBgColour"), m_foldBgColour.GetAsString(wxC2S_HTML_SYNTAX));
	}
}

OptionsConfig::~OptionsConfig(void)
{
}

wxXmlNode *OptionsConfig::ToXml() const
{
	wxXmlNode *n = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Options"));
	n->AddProperty(wxT("DisplayFoldMargin"), BoolToString(m_displayFoldMargin));
	n->AddProperty(wxT("UnderlineFoldedLine"), BoolToString(m_underlineFoldLine));
	n->AddProperty(wxT("FoldStyle"), m_foldStyle);
	n->AddProperty(wxT("DisplayBookmarkMargin"), BoolToString(m_displayBookmarkMargin));
	n->AddProperty(wxT("BookmarkShape"), m_bookmarkShape);
	n->AddProperty(wxT("BookmarkBgColour"), m_bookmarkBgColour.GetAsString(wxC2S_HTML_SYNTAX));
	n->AddProperty(wxT("BookmarkFgColour"), m_bookmarkFgColour.GetAsString(wxC2S_HTML_SYNTAX));
	n->AddProperty(wxT("HighlightCaretLine"), BoolToString(m_highlightCaretLine));
	n->AddProperty(wxT("ShowLineNumber"), BoolToString(m_displayLineNumbers));
	n->AddProperty(wxT("IndentationGuides"), BoolToString(m_showIndentationGuidelines));
	n->AddProperty(wxT("CaretLineColour"), m_caretLineColour.GetAsString(wxC2S_HTML_SYNTAX));
	n->AddProperty(wxT("IndentUsesTabs"), BoolToString(m_indentUsesTabs));
	n->AddProperty(wxT("FoldCompact"), BoolToString(m_foldCompact));
	n->AddProperty(wxT("FoldAtElse"), BoolToString(m_foldAtElse));
	n->AddProperty(wxT("FoldPreprocessor"), BoolToString(m_foldPreprocessor));
	n->AddProperty(wxT("HighlightMatchedBraces"), BoolToString(m_highlightMatchedBraces));
	n->AddProperty(wxT("AutoAddMatchedBraces"), BoolToString(m_autoAddMatchedBraces));
	n->AddProperty(wxT("FoldBgColour"), m_foldBgColour.GetAsString(wxC2S_HTML_SYNTAX));

	wxString tmp;
    tmp << m_indentWidth;
    n->AddProperty(wxT("IndentWidth"), tmp);

    tmp.clear();
    tmp << m_tabWidth;
    n->AddProperty(wxT("TabWidth"), tmp);

    tmp.clear();
	tmp << m_iconsSize;
	n->AddProperty(wxT("ToolbarIconSize"), tmp);

	tmp.clear();
	tmp << m_showWhitspaces;
	n->AddProperty(wxT("ShowWhitespaces"), tmp);

    tmp.clear();
    tmp << m_edgeMode;
    n->AddProperty(wxT("EdgeMode"), tmp);

    tmp.clear();
    tmp << m_edgeColumn;
    n->AddProperty(wxT("EdgeColumn"), tmp);

    n->AddProperty(wxT("EdgeColour"), m_edgeColour.GetAsString(wxC2S_HTML_SYNTAX));

	tmp.clear();
	tmp = wxFontMapper::GetEncodingName(m_fileFontEncoding);
	n->AddProperty(wxT("FileFontEncoding"), tmp);

	return n;
}

void OptionsConfig::SetFileFontEncoding(const wxString& strFileFontEncoding)
{
	this->m_fileFontEncoding = wxFontMapper::Get()->CharsetToEncoding(strFileFontEncoding, false);

	if (wxFONTENCODING_SYSTEM == this->m_fileFontEncoding) {
		this->m_fileFontEncoding = wxFONTENCODING_UTF8;
	}
}
