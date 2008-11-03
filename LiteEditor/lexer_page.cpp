//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : lexer_page.cpp
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

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "lexer_page.h"
#include "lexer_configuration.h"
#include "attribute_style.h"
#include <wx/font.h>
#include "editor_config.h"
#include "macros.h"
#include "free_text_dialog.h"
#include <wx/gbsizer.h>

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( LexerPage, wxPanel )
	EVT_LISTBOX( wxID_ANY, LexerPage::OnItemSelected )
	EVT_FONTPICKER_CHANGED(wxID_ANY, LexerPage::OnFontChanged)
	EVT_COLOURPICKER_CHANGED(wxID_ANY, LexerPage::OnColourChanged)
END_EVENT_TABLE()

LexerPage::LexerPage( wxWindow* parent, LexerConfPtr lexer, int id, wxPoint pos, wxSize size, int style )
		: wxPanel(parent, id, pos, wxSize(1, 1), style)
		, m_lexer(lexer)
		, m_selection(0)
		, m_isModified(false)
{
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	wxStaticText *static_text = new wxStaticText( this, wxID_ANY, wxT("Language Attributes:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add(static_text, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* sbSizer5;
	sbSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_properties = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL);
	m_properties->SetSizeHints(200, -1);
	sbSizer5->Add( m_properties, 0, wxALL|wxEXPAND, 5 );

	m_propertyList = m_lexer->GetProperties();
	std::list<StyleProperty>::iterator it = m_propertyList.begin();

	for (; it != m_propertyList.end(); it++) {
		m_properties->Append((*it).GetName());
	}
	m_properties->SetSelection(0);

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	wxString initialColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString();
	wxString bgInitialColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString();
	wxFont initialFont = wxNullFont;

	if (m_propertyList.empty() == false) {
		StyleProperty p;
		p = (*m_propertyList.begin());
		initialColor = p.GetFgColour();
		bgInitialColor = p.GetBgColour();

		int size = p.GetFontSize();
		wxString face = p.GetFaceName();
		bool bold = p.IsBold();
		initialFont = wxFont(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
	}
	wxStaticText *text(NULL);
	wxGridBagSizer *gbz = new wxGridBagSizer(0, 5);
	gbz->AddGrowableCol( 1 );
	gbz->SetFlexibleDirection( wxBOTH );

	text = new wxStaticText(this, wxID_ANY, wxT("Edit Keywords:"));
	gbz->Add(text, wxGBPosition(0, 0), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	wxBoxSizer *btnSizers = new wxBoxSizer(wxHORIZONTAL);
	gbz->Add( btnSizers, wxGBPosition(0, 1), wxGBSpan(1, 1), wxALL|wxEXPAND, 5);

	wxButton *btn0 = new wxButton(this, wxID_ANY, wxT("Set &0"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	btnSizers->Add(btn0, 0, wxEXPAND|wxALL, 0);

	wxButton *btn1 = new wxButton(this, wxID_ANY, wxT("Set &1"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	btnSizers->Add(btn1, 0, wxEXPAND|wxALL, 0);

	wxButton *btn2 = new wxButton(this, wxID_ANY, wxT("Set &2"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	btnSizers->Add(btn2, 0, wxEXPAND|wxALL, 0);

	wxButton *btn3 = new wxButton(this, wxID_ANY, wxT("Set &3"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	btnSizers->Add(btn3, 0, wxEXPAND|wxALL, 0);

	wxButton *btn4 = new wxButton(this, wxID_ANY, wxT("Set &4"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	btnSizers->Add(btn4, 0, wxEXPAND|wxALL, 0);

	text = new wxStaticText(this, wxID_ANY, wxT("Style font:"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	gbz->Add( text, wxGBPosition(1, 0), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	m_fontPicker = new wxFontPickerCtrl(this, wxID_ANY, initialFont, wxDefaultPosition, wxDefaultSize, wxFNTP_USEFONT_FOR_LABEL);
	gbz->Add( m_fontPicker, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	text = new wxStaticText(this, wxID_ANY, wxT("Style forground colour:"));
	gbz->Add( text, wxGBPosition(2, 0), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	m_colourPicker = new wxColourPickerCtrl(this, wxID_ANY, wxColour(initialColor), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	gbz->Add( m_colourPicker, wxGBPosition(2, 1), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	text = new wxStaticText(this, wxID_ANY, wxT("Style background colour:"));
	gbz->Add( text, wxGBPosition(3, 0), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	m_bgColourPicker = new wxColourPickerCtrl(this, wxID_ANY, wxColour(bgInitialColor), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	gbz->Add( m_bgColourPicker, wxGBPosition(3, 1), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	//globals settings for whole styles
	wxStaticLine *line = new wxStaticLine(this);
	gbz->Add(line, wxGBPosition(4, 0), wxGBSpan(1, 2), wxALL|wxEXPAND, 10);

	text = new wxStaticText(this, wxID_ANY, wxT("Global font:"));
	gbz->Add( text, wxGBPosition(5, 0), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	m_globalFontPicker = new wxFontPickerCtrl(this, wxID_ANY, initialFont, wxDefaultPosition, wxDefaultSize, wxFNTP_USEFONT_FOR_LABEL);
	gbz->Add( m_globalFontPicker, wxGBPosition(5, 1), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	text = new wxStaticText(this, wxID_ANY, wxT("Global background colour:"));
	gbz->Add( text, wxGBPosition(6, 0), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );

	m_globalBgColourPicker = new wxColourPickerCtrl(this, wxID_ANY, wxColour(bgInitialColor), wxDefaultPosition, wxDefaultSize, wxCLRP_SHOW_LABEL);
	gbz->Add( m_globalBgColourPicker, wxGBPosition(6, 1), wxGBSpan(1, 1), wxALL|wxEXPAND, 5 );
	bSizer7->Add(gbz, 0, wxEXPAND, 5);

	sbSizer5->Add( bSizer7, 1, wxEXPAND, 5 );

	wxBoxSizer *hs = new wxBoxSizer(wxHORIZONTAL);//, this, wxT("File Types:"));
	m_fileSpec = new wxTextCtrl(this, wxID_ANY, m_lexer->GetFileSpec());
	hs->Add(m_fileSpec, 1, wxALL | wxEXPAND, 5);

	bSizer6->Add( sbSizer5, 1, wxEXPAND, 5 );

	static_text = new wxStaticText( this, wxID_ANY, wxT("File Masking:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add(static_text, 0, wxEXPAND | wxALL, 5);
	bSizer6->Add( hs, 0, wxEXPAND, 5 );

	this->SetSizer( bSizer6 );
	this->Layout();

	if (m_propertyList.empty()) {
		m_fontPicker->Enable(false);
		m_colourPicker->Enable(false);
	}

	ConnectButton(btn0, LexerPage::OnEditKeyWordsButton0);
	ConnectButton(btn1, LexerPage::OnEditKeyWordsButton1);
	ConnectButton(btn2, LexerPage::OnEditKeyWordsButton2);
	ConnectButton(btn3, LexerPage::OnEditKeyWordsButton3);
	ConnectButton(btn4, LexerPage::OnEditKeyWordsButton4);
	ConnectCmdTextUpdated(m_fileSpec, LexerPage::OnText)
}

void LexerPage::OnItemSelected(wxCommandEvent & event)
{
	// update colour picker & font pickers
	wxString selectionString = event.GetString();
	m_selection = event.GetSelection();

	std::list<StyleProperty>::iterator iter = m_propertyList.begin();
	for (; iter != m_propertyList.end(); iter++) {
		if (iter->GetName() == selectionString) {
			// update font & color
			StyleProperty p = (*iter);
			wxString colour = p.GetFgColour();
			wxString bgColour = p.GetBgColour();
			wxFont font = wxNullFont;

			int size = p.GetFontSize();
			wxString face = p.GetFaceName();
			bool bold = p.IsBold();

			font = wxFont(size, wxFONTFAMILY_TELETYPE, wxNORMAL, bold ? wxBOLD : wxNORMAL, false, face);
			m_fontPicker->SetSelectedFont(font);
			m_bgColourPicker->SetColour(bgColour);
			m_colourPicker->SetColour(colour);
		}
	}
}

void LexerPage::OnFontChanged(wxFontPickerEvent &event)
{
	m_isModified = true;
	// update font
	wxObject *obj = event.GetEventObject();
	if (obj == m_fontPicker) {
		wxFont font = event.GetFont();
		std::list<StyleProperty>::iterator iter = m_propertyList.begin();
		for (int i=0; i<m_selection; i++)
			iter++;

		iter->SetBold(font.GetWeight() == wxFONTWEIGHT_BOLD);
		iter->SetFaceName(font.GetFaceName());
		iter->SetFontSize(font.GetPointSize());
	} else if (obj == m_globalFontPicker) {
		wxFont font = event.GetFont();
		std::list<StyleProperty>::iterator iter = m_propertyList.begin();
		for (; iter != m_propertyList.end(); iter++) {
			iter->SetBold(font.GetWeight() == wxFONTWEIGHT_BOLD);
			iter->SetFaceName(font.GetFaceName());
			iter->SetFontSize(font.GetPointSize());
		}
		//update the style font picker as well
		m_fontPicker->SetSelectedFont(font);
	}
}

void LexerPage::OnColourChanged(wxColourPickerEvent &event)
{
	m_isModified = true;
	//update colour
	wxObject *obj = event.GetEventObject();
	if (obj == m_colourPicker) {
		wxColour colour = event.GetColour();
		std::list<StyleProperty>::iterator iter = m_propertyList.begin();
		for (int i=0; i<m_selection; i++)
			iter++;

		iter->SetFgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
	} else if (obj == m_bgColourPicker) {
		wxColour colour = event.GetColour();
		std::list<StyleProperty>::iterator iter = m_propertyList.begin();
		for (int i=0; i<m_selection; i++)
			iter++;

		iter->SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
	} else if (obj == m_globalBgColourPicker) {
		wxColour colour = event.GetColour();
		std::list<StyleProperty>::iterator iter = m_propertyList.begin();
		for (; iter != m_propertyList.end(); iter++) {
			iter->SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
		}
		//update the style background colour as well
		m_bgColourPicker->SetColour(colour);
	}
}

void LexerPage::SaveSettings()
{
	if (GetIsModified()) {
		m_lexer->SetProperties( m_propertyList );
		m_lexer->SetFileSpec( m_fileSpec->GetValue() );
		m_lexer->Save();
		m_isModified = false;
	}
}

void LexerPage::OnEditKeyWordsButton0(wxCommandEvent& event)
{
	wxUnusedVar(event);
	EditKeyWords(0);
}

void LexerPage::OnEditKeyWordsButton1(wxCommandEvent& event)
{
	wxUnusedVar(event);
	EditKeyWords(1);
}

void LexerPage::OnEditKeyWordsButton2(wxCommandEvent& event)
{
	wxUnusedVar(event);
	EditKeyWords(2);
}

void LexerPage::OnEditKeyWordsButton3(wxCommandEvent& event)
{
	wxUnusedVar(event);
	EditKeyWords(3);
}

void LexerPage::OnEditKeyWordsButton4(wxCommandEvent& event)
{
	wxUnusedVar(event);
	EditKeyWords(4);
}

void LexerPage::EditKeyWords(int set)
{
	wxString keywords = m_lexer->GetKeyWords(set);
	FreeTextDialog *dlg = new FreeTextDialog(this, keywords);
	if (dlg->ShowModal() == wxID_OK) {
		m_isModified = true;
		m_lexer->SetKeyWords(dlg->GetValue(), set);
	}
	dlg->Destroy();
}

void LexerPage::OnText(wxCommandEvent& e)
{
	m_isModified = true;
	e.Skip();
}
