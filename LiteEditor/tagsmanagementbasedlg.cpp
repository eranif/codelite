///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "tagsmanagementbasedlg.h"

///////////////////////////////////////////////////////////////////////////

TagsManagementBaseDlg::TagsManagementBaseDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE );
	m_splitter1->SetMinimumPaneSize( 50 );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( TagsManagementBaseDlg::m_splitter1OnIdle ), NULL, this );
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlDatabases = new wxListCtrl( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer3->Add( m_listCtrlDatabases, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonAdd = new wxButton( m_panel1, wxID_ANY, _("Add..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonAdd->SetToolTip( _("Locate a tags database on the hard drive and add it to the\nmanagement pane") );
	
	bSizer4->Add( m_buttonAdd, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonRemove = new wxButton( m_panel1, wxID_ANY, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonRemove->SetToolTip( _("Remove the selected databse from the management pane. \nThis action does not delete the file from the hard drive") );
	
	bSizer4->Add( m_buttonRemove, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonSetActive = new wxButton( m_panel1, wxID_ANY, _("Set Active"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonSetActive->SetToolTip( _("Set the selected tags database as the active tags database\nto be used by CodeLite") );
	
	bSizer4->Add( m_buttonSetActive, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonFixPaths = new wxButton( m_panel1, wxID_ANY, _("Fix Paths..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_buttonFixPaths, 0, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer3 );
	m_panel1->Layout();
	bSizer3->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, _("Tags were generated from the following paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	m_listCtrlPaths = new wxListCtrl( m_panel2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	m_listCtrlPaths->SetToolTip( _("The 'original path' is the path stored in the database. The 'actual path' is the actual path to be used by CodeLite to locate the symbols") );
	
	bSizer5->Add( m_listCtrlPaths, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticTextFullPath = new wxStaticText( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextFullPath->Wrap( -1 );
	sbSizer1->Add( m_staticTextFullPath, 0, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	m_panel2->SetSizer( bSizer5 );
	m_panel2->Layout();
	bSizer5->Fit( m_panel2 );
	m_splitter1->SplitHorizontally( m_panel1, m_panel2, 0 );
	mainSizer->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_listCtrlDatabases->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( TagsManagementBaseDlg::OnTagsDbActivated ), NULL, this );
	m_listCtrlDatabases->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( TagsManagementBaseDlg::OnTagsDbDeSelected ), NULL, this );
	m_listCtrlDatabases->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( TagsManagementBaseDlg::OnTagsDbSelected ), NULL, this );
	m_buttonAdd->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnAddDatabase ), NULL, this );
	m_buttonRemove->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnRemoveDatabase ), NULL, this );
	m_buttonRemove->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsManagementBaseDlg::OnRemoveDatabaseUI ), NULL, this );
	m_buttonSetActive->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnSetActive ), NULL, this );
	m_buttonSetActive->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsManagementBaseDlg::OnRemoveDatabaseUI ), NULL, this );
	m_buttonFixPaths->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnFixPaths ), NULL, this );
	m_buttonFixPaths->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsManagementBaseDlg::OnRemoveDatabaseUI ), NULL, this );
	m_listCtrlPaths->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( TagsManagementBaseDlg::OnItemRightClick ), NULL, this );
	m_buttonOk->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnOk ), NULL, this );
}

TagsManagementBaseDlg::~TagsManagementBaseDlg()
{
	// Disconnect Events
	m_listCtrlDatabases->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( TagsManagementBaseDlg::OnTagsDbActivated ), NULL, this );
	m_listCtrlDatabases->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( TagsManagementBaseDlg::OnTagsDbDeSelected ), NULL, this );
	m_listCtrlDatabases->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( TagsManagementBaseDlg::OnTagsDbSelected ), NULL, this );
	m_buttonAdd->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnAddDatabase ), NULL, this );
	m_buttonRemove->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnRemoveDatabase ), NULL, this );
	m_buttonRemove->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsManagementBaseDlg::OnRemoveDatabaseUI ), NULL, this );
	m_buttonSetActive->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnSetActive ), NULL, this );
	m_buttonSetActive->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsManagementBaseDlg::OnRemoveDatabaseUI ), NULL, this );
	m_buttonFixPaths->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnFixPaths ), NULL, this );
	m_buttonFixPaths->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( TagsManagementBaseDlg::OnRemoveDatabaseUI ), NULL, this );
	m_listCtrlPaths->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( TagsManagementBaseDlg::OnItemRightClick ), NULL, this );
	m_buttonOk->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagsManagementBaseDlg::OnOk ), NULL, this );
}
