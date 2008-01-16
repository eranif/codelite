///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version May  5 2007)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "add_option_dialog.h"
#include "wx/tokenzr.h"
#include <wx/txtstrm.h>
#include <wx/sstream.h>

///////////////////////////////////////////////////////////////////////////

AddOptionDlg::AddOptionDlg( wxWindow* parent, wxString value, int id, wxString title, wxPoint pos, wxSize size, int style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_text = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB );
	bSizer23->Add( m_text, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline9 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline9, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer23->Add( bSizer24, 0, wxALIGN_RIGHT, 5 );
	
	wxStringTokenizer tkz(value, wxT(";"));
	while(tkz.HasMoreTokens()){
		wxString token = tkz.GetNextToken();
		token = token.Trim();
		token = token.Trim(false);
		m_text->AppendText(token + wxT("\n"));	
	}

	this->SetSizer( bSizer23 );
	this->Layout();
}

wxString AddOptionDlg::GetValue() const
{
	wxStringInputStream input(m_text->GetValue());
	wxTextInputStream text(input);

	wxString value;
	while( !input.Eof() ){

		// Read the next line
		value += text.ReadLine();
		value += wxT(";");
	}
	return value.BeforeLast(wxT(';'));
}
