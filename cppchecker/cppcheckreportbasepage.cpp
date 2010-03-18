///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "cppcheckreportbasepage.h"

///////////////////////////////////////////////////////////////////////////

CppCheckReportBasePage::CppCheckReportBasePage( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxVERTICAL );
	
	m_outputText = new wxScintilla(this);
	bSizer41->Add( m_outputText, 1, wxALL|wxEXPAND, 5 );
	
	m_staticTextFile = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFile->Wrap( -1 );
	bSizer41->Add( m_staticTextFile, 0, wxALL|wxEXPAND, 5 );
	
	m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_gauge->SetMinSize( wxSize( -1,15 ) );
	
	bSizer41->Add( m_gauge, 0, wxEXPAND|wxALL, 5 );
	
	mainSizer->Add( bSizer41, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonClear = new wxButton( this, wxID_CLEAR, _("&Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClear->SetToolTip( _("Clear the CppCheck report view") );
	
	bSizer4->Add( m_buttonClear, 0, wxALL, 5 );
	
	m_buttonStop = new wxButton( this, wxID_STOP, _("&Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonStop->SetDefault(); 
	m_buttonStop->SetToolTip( _("Stop the curreny analysis") );
	
	bSizer4->Add( m_buttonStop, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer4, 0, wxEXPAND, 5 );
	
	bSizer5->Add( mainSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	
	// Connect Events
	m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnClearReport ), NULL, this );
	m_buttonClear->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnClearReportUI ), NULL, this );
	m_buttonStop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnStopChecking ), NULL, this );
	m_buttonStop->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnStopCheckingUI ), NULL, this );
}

CppCheckReportBasePage::~CppCheckReportBasePage()
{
	// Disconnect Events
	m_buttonClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnClearReport ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnClearReportUI ), NULL, this );
	m_buttonStop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnStopChecking ), NULL, this );
	m_buttonStop->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnStopCheckingUI ), NULL, this );
}
