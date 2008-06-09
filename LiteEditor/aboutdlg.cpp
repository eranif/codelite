///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/xrc/xmlres.h>
#include "aboutdlg.h"

///////////////////////////////////////////////////////////////////////////

AboutDlg::AboutDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );

	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("About")), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_bitmap, 1, wxALL|wxEXPAND, 5 );

	m_staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );

	m_buttonOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault();
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );

	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	this->SetSizer( mainSizer );
	mainSizer->Fit(this);
	// focus the OK button, this will allow use to dismiss the dialog with ESC key
	m_bitmap->SetFocus();
	// center it, othewise, Mac will present it on top left
	CenterOnScreen();
	this->Layout();
}

AboutDlg::~AboutDlg()
{
}
