//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : svntab.cpp              
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
 #include "svntab.h"
#include "wx/wxscintilla.h"

SvnTab::SvnTab(wxWindow *parent)
: wxPanel(parent)
{
	Initialize();
}

SvnTab::~SvnTab()
{
}

void SvnTab::Initialize()
{
	wxBoxSizer *sz = new wxBoxSizer(wxVERTICAL);
	SetSizer( sz );
	
	m_sci = new wxScintilla(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	sz->Add(m_sci, 1, wxEXPAND);
	
	// Hide margins
	m_sci->SetLexer(wxSCI_LEX_SVN);
	m_sci->StyleClearAll();

	// symbol margin
	m_sci->SetMarginType(1, wxSCI_MARGIN_SYMBOL);
	m_sci->SetMarginWidth(2, 0);
	m_sci->SetMarginWidth(1, 0);
	m_sci->SetMarginWidth(0, 0);

	wxFont defFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
	wxFont font(defFont.GetPointSize(), wxFONTFAMILY_TELETYPE, wxNORMAL, wxNORMAL);
	
	//set the font for different styles
	m_sci->StyleSetFont(0, font);
	m_sci->StyleSetBackground(0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_sci->StyleSetBackground(wxSCI_STYLE_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_sci->SetReadOnly(true);
	
	m_sci->StyleSetFont(wxSCI_LEX_SVN_ADDED, font);
	m_sci->StyleSetForeground(wxSCI_LEX_SVN_ADDED, wxT("FOREST GREEN"));

	m_sci->StyleSetFont(wxSCI_LEX_SVN_CONFLICT, font);
	m_sci->StyleSetForeground(wxSCI_LEX_SVN_CONFLICT, wxT("RED"));

	m_sci->StyleSetFont(wxSCI_LEX_SVN_UPDATED, font);
	m_sci->StyleSetForeground(wxSCI_LEX_SVN_UPDATED, wxT("FOREST GREEN"));

	m_sci->StyleSetFont(wxSCI_LEX_SVN_DEFAULT, font);
	m_sci->StyleSetForeground(wxSCI_LEX_SVN_DEFAULT, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));

	m_sci->StyleSetFont(wxSCI_LEX_SVN_DELETED, font);
	m_sci->StyleSetForeground(wxSCI_LEX_SVN_DELETED, wxT("FOREST GREEN"));

	m_sci->StyleSetFont(wxSCI_LEX_SVN_MERGED, font);
	m_sci->StyleSetForeground(wxSCI_LEX_SVN_MERGED, wxT("FOREST GREEN"));
	
	m_sci->StyleSetBackground(0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_sci->SetWrapMode(wxSCI_WRAP_WORD);
	m_sci->SetWrapVisualFlags(1);
	sz->Layout();
}

void SvnTab::AppendText(const wxString &text)
{
	//----------------------------------------------
	// enable writing
	m_sci->SetReadOnly(false);

	// the next 4 lines make sure that the caret is at last line
	// and is visible
	m_sci->SetSelectionEnd(m_sci->GetLength());
	m_sci->SetSelectionStart(m_sci->GetLength());
	m_sci->SetCurrentPos(m_sci->GetLength());
	m_sci->EnsureCaretVisible();

	// add the text
	m_sci->AddText( text );

	// the next 4 lines make sure that the caret is at last line
	// and is visible
	m_sci->SetSelectionEnd(m_sci->GetLength());
	m_sci->SetSelectionStart(m_sci->GetLength());
	m_sci->SetCurrentPos(m_sci->GetLength());
	m_sci->EnsureCaretVisible();

	// enable readonly mode
	m_sci->SetReadOnly(true);
}

void SvnTab::Clear()
{
	m_sci->SetReadOnly(false);
	m_sci->ClearAll();
	m_sci->SetReadOnly(true);
}
