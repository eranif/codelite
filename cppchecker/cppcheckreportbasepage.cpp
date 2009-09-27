///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
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
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( CppCheckReportBasePage::m_splitter1OnIdle ), NULL, this );
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_filelist = new CppCheckFileListCtrl(m_panel1);
	bSizer3->Add( m_filelist, 1, wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_listCtrlReport = new wxListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer2->Add( m_listCtrlReport, 1, wxALL|wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer2 );
	m_panel2->Layout();
	bSizer2->Fit( m_panel2 );
	m_splitter1->SplitHorizontally( m_panel1, m_panel2, 90 );
	mainSizer->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonClear = new wxButton( this, wxID_ANY, _("&Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonClear->SetToolTip( _("Clear the CppCheck report view") );
	
	bSizer4->Add( m_buttonClear, 0, wxALL, 5 );
	
	m_buttonStop = new wxButton( this, wxID_ANY, _("&Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonStop->SetToolTip( _("Stop the curreny analysis") );
	
	bSizer4->Add( m_buttonStop, 0, wxALL, 5 );
	
	m_buttonSkip = new wxButton( this, wxID_ANY, _("Skip File"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSkip->SetToolTip( _("Skip the current file") );
	
	bSizer4->Add( m_buttonSkip, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer4, 0, wxEXPAND, 5 );
	
	bSizer5->Add( mainSizer, 1, wxEXPAND, 5 );
	
	m_progress = new ProgressCtrl(this);
	bSizer5->Add( m_progress, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	this->Layout();
	
	// Connect Events
	m_listCtrlReport->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CppCheckReportBasePage::OnListCtrlItemActivated ), NULL, this );
	m_buttonClear->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnClearReport ), NULL, this );
	m_buttonClear->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnClearReportUI ), NULL, this );
	m_buttonStop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnStopChecking ), NULL, this );
	m_buttonStop->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnStopCheckingUI ), NULL, this );
	m_buttonSkip->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnSkipFile ), NULL, this );
	m_buttonSkip->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnSkipFileUI ), NULL, this );
}

CppCheckReportBasePage::~CppCheckReportBasePage()
{
	// Disconnect Events
	m_listCtrlReport->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CppCheckReportBasePage::OnListCtrlItemActivated ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnClearReport ), NULL, this );
	m_buttonClear->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnClearReportUI ), NULL, this );
	m_buttonStop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnStopChecking ), NULL, this );
	m_buttonStop->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnStopCheckingUI ), NULL, this );
	m_buttonSkip->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CppCheckReportBasePage::OnSkipFile ), NULL, this );
	m_buttonSkip->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CppCheckReportBasePage::OnSkipFileUI ), NULL, this );
}
