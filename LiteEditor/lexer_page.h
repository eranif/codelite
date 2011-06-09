//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : lexer_page.h
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
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  1 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __lexer_page__
#define __lexer_page__

#include "lexer_configuration.h"
#include "syntaxhighlightbasedlg.h"

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LexerPage
///////////////////////////////////////////////////////////////////////////////
class LexerPage : public LexerPageBase
{
	LexerConfPtr             m_lexer;
	std::list<StyleProperty> m_propertyList;
	int                      m_selection;
	bool                     m_isModified;

protected:
	// Virtual event handlers, overide them in your derived class
	virtual void OnItemSelected( wxCommandEvent& event );
	virtual void OnFontChanged(wxFontPickerEvent &event);
	virtual void OnColourChanged(wxColourPickerEvent &event);
	virtual void OnEditKeyWordsButton0(wxCommandEvent &event);
	virtual void OnEditKeyWordsButton1(wxCommandEvent &event);
	virtual void OnEditKeyWordsButton2(wxCommandEvent &event);
	virtual void OnEditKeyWordsButton3(wxCommandEvent &event);
	virtual void OnEditKeyWordsButton4(wxCommandEvent &event);
	virtual void OnText(wxCommandEvent &e);
	virtual void OnEolFilled(wxCommandEvent &event);
	virtual void OnStyleWithinPreprocessor(wxCommandEvent &event);
	virtual void OnStyleWithingPreProcessorUI(wxUpdateUIEvent& event);
	virtual void OnAlphaChanged(wxScrollEvent& event);
	virtual void OnSelTextChanged(wxColourPickerEvent& event);
	virtual void OnOutputViewColourChanged(wxColourPickerEvent& event);


	void         EditKeyWords(int set);
	std::list<StyleProperty>::iterator GetSelectedStyle();

public:
	LexerPage( wxWindow* parent, LexerConfPtr lexer, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 285,300 ), int style = wxTAB_TRAVERSAL );
	void SaveSettings();

	const bool& GetIsModified() const {
		return m_isModified;
	}
};

#endif //__lexer_page__
