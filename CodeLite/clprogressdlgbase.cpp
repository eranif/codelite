///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 22 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "clprogressdlgbase.h"

///////////////////////////////////////////////////////////////////////////

clProgressDlgBase::clProgressDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,-1 ), wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticLabel = new wxStaticText( this, wxID_ANY, _("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticLabel->Wrap( -1 );
	mainSizer->Add( m_staticLabel, 0, wxALL|wxEXPAND, 5 );
	
	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_gauge->SetMinSize( wxSize( 400,-1 ) );
	
	mainSizer->Add( m_gauge, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

clProgressDlgBase::~clProgressDlgBase()
{
}
