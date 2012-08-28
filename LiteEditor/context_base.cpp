//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : context_base.cpp
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
#include <wx/xrc/xmlres.h>
#include "context_base.h"
#include "drawingutils.h"
#include <vector>
#include "editor_config.h"
#include "cl_editor.h"
#include "frame.h"

#define CL_LINE_MODIFIED_STYLE      200
#define CL_LINE_SAVED_STYLE         201

static wxColor GetInactiveColor(const wxColor& col)
{
	wxUnusedVar(col);
#ifdef __WXGTK__
	return wxColor(wxT("GREY"));
#else
	return wxColor(wxT("LIGHT GREY"));
#endif
}

ContextBase::ContextBase(LEditor *container)
		: m_container(container)
		, m_name(wxEmptyString)
{
}

ContextBase::ContextBase(const wxString &name)
		: m_name(name)
{
}

ContextBase::~ContextBase()
{
}

//provide basic indentation
void ContextBase::AutoIndent(const wxChar &ch)
{
	if (ch == wxT('\n')) {
		//just copy the previous line indentation
		LEditor &rCtrl = GetCtrl();
		int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
		rCtrl.SetLineIndentation(line, rCtrl.GetLineIndentation(line-1));
		//place the caret at the end of the line
        rCtrl.SetCaretAt(rCtrl.GetLineIndentPosition(line));
        rCtrl.ChooseCaretX();
	}
}

void ContextBase::DoApplySettings(LexerConfPtr lexPtr)
{
	LEditor &rCtrl = GetCtrl();

	rCtrl.StyleClearAll();
	rCtrl.SetStyleBits(rCtrl.GetStyleBitsNeeded());
	
	// Define the styles for the editing margin
	rCtrl.StyleSetBackground(CL_LINE_SAVED_STYLE,    wxColour(wxT("FOREST GREEN")));
	rCtrl.StyleSetBackground(CL_LINE_MODIFIED_STYLE, wxColour(wxT("ORANGE")));

	// by default indicators are set to be opaque rounded box
	rCtrl.IndicatorSetStyle(1, wxSCI_INDIC_ROUNDBOX);
	rCtrl.IndicatorSetStyle(2, wxSCI_INDIC_ROUNDBOX);

	rCtrl.IndicatorSetAlpha(1, 80);
	rCtrl.IndicatorSetAlpha(2, 80);

	bool tooltip(false);

	std::list<StyleProperty> styles;
    if (lexPtr) {
        styles = lexPtr->GetProperties();
		rCtrl.SetProperty(wxT("styling.within.preprocessor"), lexPtr->GetStyleWithinPreProcessor() ? wxT("1") : wxT("0"));
    }
	
	// Find the default style
	wxFont defaultFont;
	bool foundDefaultStyle = false;
	std::list<StyleProperty>::iterator iter = styles.begin();
	for (; iter != styles.end(); iter++) {
		if(iter->GetId() == 0) {
			defaultFont = wxFont(iter->GetFontSize(), 
						  wxFONTFAMILY_TELETYPE, 
						  iter->GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
						  iter->IsBold() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, 
						  iter->GetUnderlined(), 
						  iter->GetFaceName());
			foundDefaultStyle = true;
			break;
		}
	}
	
	if (foundDefaultStyle) {
		for(int i=0; i<256; i++) {
			rCtrl.StyleSetFont(i, defaultFont);
		}
	}
	
	iter = styles.begin();
	for (; iter != styles.end(); iter++) {

		StyleProperty sp        = (*iter);
		int           size      = sp.GetFontSize();
		wxString      face      = sp.GetFaceName();
		bool          bold      = sp.IsBold();
		bool          italic    = sp.GetItalic();
		bool          underline = sp.GetUnderlined();
		int           alpha     = sp.GetAlpha();

		// handle special cases
		if ( sp.GetId() == FOLD_MARGIN_ATTR_ID ) {

			// fold margin foreground colour
			rCtrl.SetFoldMarginColour(true, sp.GetBgColour());
			rCtrl.SetFoldMarginHiColour(true, sp.GetFgColour());

		} else if ( sp.GetId() == SEL_TEXT_ATTR_ID ) {

			// selection colour
			if(wxColour(sp.GetBgColour()).IsOk()) {
				rCtrl.SetSelBackground(true, sp.GetBgColour());
#ifndef __WXMAC__
				rCtrl.SetSelAlpha(alpha);
#endif
			}

		} else if ( sp.GetId() == CARET_ATTR_ID ) {

			// caret colour
			if(wxColour(sp.GetFgColour()).IsOk()) {
				rCtrl.SetCaretForeground(sp.GetFgColour());
			}

		} else {
			int fontSize( size );

			wxFont font = wxFont(size, wxFONTFAMILY_TELETYPE, italic ? wxITALIC : wxNORMAL , bold ? wxBOLD : wxNORMAL, underline, face);
			if (sp.GetId() == 0) { //default
				rCtrl.StyleSetFont(wxSCI_STYLE_DEFAULT, font);
				rCtrl.StyleSetSize(wxSCI_STYLE_DEFAULT, size);
				rCtrl.StyleSetForeground(wxSCI_STYLE_DEFAULT, (*iter).GetFgColour());
				
				// Inactive state is greater by 64 from its counterpart
				wxColor inactiveColor = GetInactiveColor((*iter).GetFgColour());
				rCtrl.StyleSetForeground(wxSCI_STYLE_DEFAULT + 64, inactiveColor);
				rCtrl.StyleSetFont(wxSCI_STYLE_DEFAULT + 64,       font);
				rCtrl.StyleSetSize(wxSCI_STYLE_DEFAULT + 64,       size);
				
				rCtrl.StyleSetBackground(wxSCI_STYLE_DEFAULT, (*iter).GetBgColour());
				rCtrl.StyleSetSize(wxSCI_STYLE_LINENUMBER, size);
				rCtrl.SetFoldMarginColour(true, (*iter).GetBgColour());
				rCtrl.SetFoldMarginHiColour(true, (*iter).GetBgColour());

				// test the background colour of the editor, if it is considered "dark"
				// set the indicator to be hollow rectanlgle
				StyleProperty sp = (*iter);
				if ( DrawingUtils::IsDark(sp.GetBgColour()) ) {
					rCtrl.IndicatorSetStyle(1, wxSCI_INDIC_BOX);
					rCtrl.IndicatorSetStyle(2, wxSCI_INDIC_BOX);
				}
			} else if(sp.GetId() == wxSCI_STYLE_CALLTIP){
				tooltip = true;
				if(sp.GetFaceName().IsEmpty()){
					font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
					fontSize = font.GetPointSize();
				}
			}

			rCtrl.StyleSetFont(sp.GetId(), font);
			rCtrl.StyleSetSize(sp.GetId(), fontSize);
			rCtrl.StyleSetEOLFilled(sp.GetId(), iter->GetEolFilled());

			if(iter->GetId() == LINE_NUMBERS_ATTR_ID) {
				// Set the line number colours only if requested
				// otherwise, use default colours provided by scintilla
				if(sp.GetBgColour().IsEmpty() == false)
					rCtrl.StyleSetBackground(sp.GetId(), sp.GetBgColour());

				if(sp.GetFgColour().IsEmpty() == false)
					rCtrl.StyleSetForeground(sp.GetId(), sp.GetFgColour());
				else
					rCtrl.StyleSetForeground(sp.GetId(), wxT("BLACK"));

			} else {
				rCtrl.StyleSetForeground(sp.GetId(), sp.GetFgColour());
				
				// Inactive state is greater by 64 from its counterpart
				wxColor inactiveColor = GetInactiveColor(iter->GetFgColour());
				rCtrl.StyleSetForeground(sp.GetId() + 64, inactiveColor);
				rCtrl.StyleSetFont(sp.GetId() + 64,       font);
				rCtrl.StyleSetSize(sp.GetId() + 64,       size);
				
				rCtrl.StyleSetBackground(sp.GetId(), sp.GetBgColour());
			}
		}
	}

	// set the calltip font
	if( !tooltip ) {
		wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
		rCtrl.StyleSetFont(wxSCI_STYLE_CALLTIP, font);
	}
}

int ContextBase::GetHyperlinkRange(int pos, int &start, int &end)
{
    LEditor &rCtrl = GetCtrl();
    int curstyle = rCtrl.GetStyleAt(pos);
    if (curstyle == wxSCI_C_WORD2 || curstyle == wxSCI_C_GLOBALCLASS || curstyle == wxSCI_C_IDENTIFIER) {
        // get tag as hyperlink
        start = rCtrl.WordStartPos(pos, true);
        end = rCtrl.WordEndPos(pos, true);
        if (start < end)
            return XRCID("find_tag");
    }
    return wxID_NONE;
}

void ContextBase::GoHyperlink(int start, int end, int type, bool alt)
{
    wxUnusedVar(start);
    wxUnusedVar(end);
	wxUnusedVar(type);
	wxUnusedVar(alt);
}

wxMenu* ContextBase::GetMenu()
{
	return wxXmlResource::Get()->LoadMenu(wxT("editor_right_click_default"));
}

void ContextBase::PrependMenuItem(wxMenu *menu, const wxString &text, wxObjectEventFunction func, int eventId)
{
	wxMenuItem *item;
	wxString menuItemText;
    if( eventId == -1 ) 
        eventId = wxNewId();
        
	item = new wxMenuItem(menu, eventId, text);
	menu->Prepend(item);
	menu->Connect(eventId, wxEVT_COMMAND_MENU_SELECTED, func, NULL, this);
	m_dynItems.push_back(item);
}

void ContextBase::PrependMenuItem(wxMenu *menu, const wxString &text, int id)
{
	wxMenuItem *item;
	wxString menuItemText;
	item = new wxMenuItem(menu, id, text);
	menu->Prepend(item);
	m_dynItems.push_back(item);
}

void ContextBase::PrependMenuItemSeparator(wxMenu *menu)
{
	wxMenuItem *item;
	item = new wxMenuItem(menu, wxID_SEPARATOR);
	menu->Prepend(item);
	m_dynItems.push_back(item);
}

int ContextBase::DoGetCalltipParamterIndex()
{
	int index(0);
	LEditor &ctrl =  GetCtrl();
	int pos = ctrl.DoGetOpenBracePos();
	if (pos != wxNOT_FOUND) {

		// loop over the text from pos -> current position and count the number of commas found
		int depth(0);
		bool exit_loop(false);

		while ( pos < ctrl.GetCurrentPos() && !exit_loop ) {
			wxChar ch        = ctrl.SafeGetChar(pos);
			wxChar ch_before = ctrl.SafeGetChar(ctrl.PositionBefore(pos));

			if (IsCommentOrString(pos)) {
				pos = ctrl.PositionAfter(pos);
				continue;
			}

			switch (ch) {
			case wxT(','):
				if (depth == 0) index++;
				break;
			case wxT('{'):
			case wxT('}'):
			case wxT(';'):
				// error?
				exit_loop = true;
				break;
			case wxT('('):
			case wxT('<'):
			case wxT('['):
				depth++;
				break;
			case wxT('>'):
				if ( ch_before == wxT('-') ) {
					// operator noting to do
					break;
				}
				// fall through
			case wxT(')'):
			case wxT(']'):
				depth--;
				break;
			default:
				break;
			}
			pos = ctrl.PositionAfter(pos);
		}
	}
	return index;
}
