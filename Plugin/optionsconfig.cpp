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
		, m_foldStyle(wxT("Simple"))
		, m_displayBookmarkMargin(true)
		, m_bookmarkShape(wxT("Small Arrow"))
		, m_bookmarkBgColour(wxColour(12, 133, 222))
		, m_bookmarkFgColour(wxColour(66, 169, 244))
		, m_highlightCaretLine(true)
		, m_displayLineNumbers(false)
		, m_showIndentationGuidelines(false)
		, m_caretLineColour(wxT("LIGHT BLUE"))
		, m_indentUsesTabs(true)
		, m_iconsSize(24)
		, m_showWhitspaces(0/*wxSCI_WS_INVISIBLE*/)
{
	//set the default font name to be UTF8
	SetFileFontEncoding(wxFontMapper::GetEncodingName(wxFONTENCODING_UTF8));
	
	if ( node ) {
		m_displayFoldMargin = XmlUtils::ReadBool(node, wxT("DisplayFoldMargin"));
		m_underlineFoldLine = XmlUtils::ReadBool(node, wxT("UnderlineFoldedLine"));
		m_foldStyle = XmlUtils::ReadString(node, wxT("FoldStyle"));
		m_displayBookmarkMargin = XmlUtils::ReadBool(node, wxT("DisplayBookmarkMargin"));
		m_bookmarkShape = XmlUtils::ReadString(node, wxT("BookmarkShape"));
		m_bookmarkBgColour = XmlUtils::ReadString(node, wxT("BookmarkBgColour"));
		m_bookmarkFgColour = XmlUtils::ReadString(node, wxT("BookmarkFgColour"));
		m_highlightCaretLine = XmlUtils::ReadBool(node, wxT("HighlightCaretLine"));
		m_displayLineNumbers = XmlUtils::ReadBool(node, wxT("ShowLineNumber"));
		m_showIndentationGuidelines = XmlUtils::ReadBool(node, wxT("IndentationGuides"));
		m_caretLineColour = XmlUtils::ReadString(node, wxT("CaretLineColour"), wxColour(255, 255, 220).GetAsString(wxC2S_HTML_SYNTAX));
		m_caretColour = XmlUtils::ReadString(node, wxT("CaretColour"), wxColour(0, 0, 0).GetAsString(wxC2S_HTML_SYNTAX));
		m_indentUsesTabs = XmlUtils::ReadBool(node, wxT("IndentUsesTabs"), true);
		m_iconsSize = XmlUtils::ReadLong(node, wxT("ToolbarIconSize"), 24);
		m_showWhitspaces = XmlUtils::ReadLong(node, wxT("ShowWhitespaces"), 0);
		SetFileFontEncoding(XmlUtils::ReadString(node, wxT("FileFontEncoding"), wxFontMapper::GetEncodingName(wxFONTENCODING_UTF8)));
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
	n->AddProperty(wxT("CaretColour"), m_caretColour.GetAsString(wxC2S_HTML_SYNTAX));
	n->AddProperty(wxT("IndentUsesTabs"), BoolToString(m_indentUsesTabs));
	
	wxString tmp;
	tmp << m_iconsSize;
	n->AddProperty(wxT("ToolbarIconSize"), tmp);
	
	tmp.clear();
	tmp << m_showWhitspaces;
	n->AddProperty(wxT("ShowWhitespaces"), tmp);

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
