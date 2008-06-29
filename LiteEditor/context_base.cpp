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
#include "context_base.h"
#include "drawingutils.h"
#include <vector>
#include "editor_config.h"
#include "cl_editor.h"

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
		int indentSize = rCtrl.GetIndent();
		int line = rCtrl.LineFromPosition(rCtrl.GetCurrentPos());
		int prevLine = line - 1;
		//take the previous line indentation size
		int prevLineIndet = rCtrl.GetLineIndentation(prevLine);
		rCtrl.SetLineIndentation(line, prevLineIndet);
		//place the caret at the end of the line
		int dummy = rCtrl.GetLineIndentation(line);
		if (rCtrl.GetUseTabs()) {
			dummy = dummy / indentSize;
		}
		rCtrl.SetCaretAt(rCtrl.GetCurrentPos() + dummy);
	}
}

void ContextBase::DoApplySettings(LexerConfPtr lexPtr)
{
	LEditor &rCtrl = GetCtrl();

	rCtrl.StyleClearAll();
	rCtrl.SetStyleBits(rCtrl.GetStyleBitsNeeded());

	// by default indicators are set to be opaque rounded box
	rCtrl.IndicatorSetStyle(1, wxSCI_INDIC_BOX);
	rCtrl.IndicatorSetStyle(2, wxSCI_INDIC_BOX);

	std::list<StyleProperty> styles = lexPtr->GetProperties();
	std::list<StyleProperty>::iterator iter = styles.begin();
	for (; iter != styles.end(); iter++) {
		StyleProperty st = (*iter);
		int size = st.GetFontSize();
		wxString face = st.GetFaceName();
		bool bold = st.IsBold();

		// handle special cases
		if ( st.GetId() == -1 ) {
			// fold margin foreground colour
			rCtrl.SetFoldMarginColour(true, st.GetBgColour());
			rCtrl.SetFoldMarginHiColour(true, st.GetFgColour());
		} else if ( st.GetId() == -2 ) {
			// selection colour
			rCtrl.SetSelForeground(true, st.GetFgColour());
			rCtrl.SetSelBackground(true, st.GetBgColour());
		} else if ( st.GetId() == -3 ) {
			// caret colour
			rCtrl.SetCaretForeground(st.GetFgColour());
		} else {
			wxFont font;
			if (st.GetId() == wxSCI_STYLE_CALLTIP) {
				font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
			} else {
				font = wxFont(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
			}

			if (st.GetId() == 0) { //default
				rCtrl.StyleSetFont(wxSCI_STYLE_DEFAULT, font);
				rCtrl.StyleSetSize(wxSCI_STYLE_DEFAULT, (*iter).GetFontSize());
				rCtrl.StyleSetForeground(wxSCI_STYLE_DEFAULT, (*iter).GetFgColour());
				rCtrl.StyleSetBackground(wxSCI_STYLE_DEFAULT, (*iter).GetBgColour());
				rCtrl.StyleSetSize(wxSCI_STYLE_LINENUMBER, (*iter).GetFontSize());
				rCtrl.SetFoldMarginColour(true, (*iter).GetBgColour());
				rCtrl.SetFoldMarginHiColour(true, (*iter).GetBgColour());

				// test the background colour of the editor, if it is considered "dark"
				// set the indicator to be hollow rectanlgle
				StyleProperty sp = (*iter);
				if ( DrawingUtils::IsDark(sp.GetBgColour()) ) {
					rCtrl.IndicatorSetStyle(1, wxSCI_INDIC_BOX);
					rCtrl.IndicatorSetStyle(2, wxSCI_INDIC_BOX);
				}
			}

			rCtrl.StyleSetFont(st.GetId(), font);
			rCtrl.StyleSetSize(st.GetId(), (*iter).GetFontSize());
			rCtrl.StyleSetForeground(st.GetId(), (*iter).GetFgColour());
			rCtrl.StyleSetBackground(st.GetId(), (*iter).GetBgColour());
		}
	}

}
