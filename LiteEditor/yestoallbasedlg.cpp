///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  8 2009)
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

#include "yestoallbasedlg.h"

///////////////////////////////////////////////////////////////////////////

YesToAllBaseDlg::YesToAllBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* panelSizer;
	panelSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticMsg = new wxStaticText( m_mainPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticMsg->Wrap( -1 );
	panelSizer->Add( m_staticMsg, 1, wxALL|wxALIGN_CENTER_HORIZONTAL, 15 );
	
	m_checkBoxAll = new wxCheckBox( m_mainPanel, wxID_ANY, wxT("Apply to all"), wxDefaultPosition, wxDefaultSize, 0 );
	panelSizer->Add( m_checkBoxAll, 0, wxALL, 15 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1Yes = new wxButton( m_mainPanel, wxID_YES );
	m_sdbSizer1->AddButton( m_sdbSizer1Yes );
	m_sdbSizer1No = new wxButton( m_mainPanel, wxID_NO );
	m_sdbSizer1->AddButton( m_sdbSizer1No );
	m_sdbSizer1->Realize();
	panelSizer->Add( m_sdbSizer1, 1, wxALIGN_CENTER, 5 );
	
	m_mainPanel->SetSizer( panelSizer );
	m_mainPanel->Layout();
	panelSizer->Fit( m_mainPanel );
	mainSizer->Add( m_mainPanel, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_sdbSizer1No->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YesToAllBaseDlg::OnNoClicked ), NULL, this );
	m_sdbSizer1Yes->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YesToAllBaseDlg::OnYesClicked ), NULL, this );
}

YesToAllBaseDlg::~YesToAllBaseDlg()
{
	// Disconnect Events
	m_sdbSizer1No->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YesToAllBaseDlg::OnNoClicked ), NULL, this );
	m_sdbSizer1Yes->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( YesToAllBaseDlg::OnYesClicked ), NULL, this );
}
