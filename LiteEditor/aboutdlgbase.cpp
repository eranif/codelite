///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include <wx/xrc/xmlres.h>
#include "aboutdlgbase.h"

///////////////////////////////////////////////////////////////////////////

AboutDlgBase::AboutDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxXmlResource::Get()->LoadBitmap(wxT("About")), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_bitmap, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextHomePage = new wxStaticText( this, wxID_ANY, wxT("Home Page:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextHomePage->Wrap( -1 );
	m_staticTextHomePage->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticTextHomePage, 1, wxALL|wxEXPAND, 5 );
	
	m_hyperlink1 = new wxHyperlinkCtrl( this, wxID_ANY, wxT("http://www.codelite.org"), wxT("http://www.codelite.org"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	fgSizer1->Add( m_hyperlink1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("CodeLite Forum:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	m_staticText2->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText2, 0, wxALL, 5 );
	
	m_hyperlink2 = new wxHyperlinkCtrl( this, wxID_ANY, wxT("http://www.codelite.org/forum/"), wxT("http://www.codelite.org/forum/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	fgSizer1->Add( m_hyperlink2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Version:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	m_staticText3->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer1->Add( m_staticText3, 0, wxALL, 5 );
	
	m_staticTextInformation = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextInformation->Wrap( -1 );
	fgSizer1->Add( m_staticTextInformation, 0, wxALL, 5 );
	
	sbSizer1->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

AboutDlgBase::~AboutDlgBase()
{
}
