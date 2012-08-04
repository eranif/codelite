///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 13 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gitui.h"

///////////////////////////////////////////////////////////////////////////

GitSettingsDlgBase::GitSettingsDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Colour for tracked files:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourTrackedFile = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 0, 128, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourTrackedFile, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Colour for files with diffs:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourDiffFile = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 255, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_colourDiffFile, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Path to git executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pathGIT = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	fgSizer1->Add( m_pathGIT, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Path to gitk executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pathGITK = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	fgSizer1->Add( m_pathGITK, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	
	mainSizer->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	bSizer3->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
}

GitSettingsDlgBase::~GitSettingsDlgBase()
{
}

GitCommitDlgBase::GitCommitDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter2 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH );
	m_splitter2->SetSashGravity( 0.5 );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( GitCommitDlgBase::m_splitter2OnIdle ), NULL, this );
	
	m_panel3 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH );
	m_splitter1->SetSashGravity( 0.5 );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( GitCommitDlgBase::m_splitter1OnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText6 = new wxStaticText( m_panel1, wxID_ANY, _("Modified paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer11->Add( m_staticText6, 0, wxTOP|wxBOTTOM, 5 );
	
	wxArrayString m_listBoxChoices;
	m_listBox = new wxCheckListBox( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_listBoxChoices, 0 );
	bSizer11->Add( m_listBox, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_panel1->SetSizer( bSizer11 );
	m_panel1->Layout();
	bSizer11->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText7 = new wxStaticText( m_panel2, wxID_ANY, _("Diff:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	bSizer9->Add( m_staticText7, 0, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_editor = new GitCommitEditor(m_panel2);
	bSizer9->Add( m_editor, 1, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_panel2->SetSizer( bSizer9 );
	m_panel2->Layout();
	bSizer9->Fit( m_panel2 );
	m_splitter1->SplitVertically( m_panel1, m_panel2, 0 );
	bSizer12->Add( m_splitter1, 1, wxEXPAND|wxALL, 5 );
	
	m_panel3->SetSizer( bSizer12 );
	m_panel3->Layout();
	bSizer12->Fit( m_panel3 );
	m_panel4 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText8 = new wxStaticText( m_panel4, wxID_ANY, _("Commit message:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer13->Add( m_staticText8, 0, wxALL, 5 );
	
	m_commitMessage = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2|wxTE_WORDWRAP );
	m_commitMessage->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer13->Add( m_commitMessage, 1, wxALL|wxEXPAND, 5 );
	
	m_panel4->SetSizer( bSizer13 );
	m_panel4->Layout();
	bSizer13->Fit( m_panel4 );
	m_splitter2->SplitHorizontally( m_panel3, m_panel4, 0 );
	bSizer4->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer4->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button5 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button5->SetDefault(); 
	bSizer5->Add( m_button5, 0, wxALL, 5 );
	
	m_button6 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_button6, 0, wxALL, 5 );
	
	bSizer4->Add( bSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer4 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_listBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GitCommitDlgBase::OnChangeFile ), NULL, this );
}

GitCommitDlgBase::~GitCommitDlgBase()
{
	// Disconnect Events
	m_listBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GitCommitDlgBase::OnChangeFile ), NULL, this );
	
}

GitCommitListDlgBase::GitCommitListDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Commit List") ), wxVERTICAL );
	
	m_commitListBox = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_VRULES );
	sbSizer1->Add( m_commitListBox, 1, wxALL|wxEXPAND, 5 );
	
	bSizer17->Add( sbSizer1, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Modified files") ), wxVERTICAL );
	
	m_fileListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer3->Add( m_fileListBox, 1, wxALL|wxEXPAND, 5 );
	
	bSizer18->Add( sbSizer3, 1, wxEXPAND|wxRIGHT, 5 );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Diff") ), wxVERTICAL );
	
	m_editor = new GitCommitEditor(this);
	sbSizer5->Add( m_editor, 1, wxALL|wxEXPAND, 5 );
	
	bSizer18->Add( sbSizer5, 2, wxEXPAND, 5 );
	
	bSizer17->Add( bSizer18, 2, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Commit Message") ), wxVERTICAL );
	
	m_commitMessage = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	sbSizer2->Add( m_commitMessage, 1, wxALL|wxEXPAND, 5 );
	
	bSizer17->Add( sbSizer2, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer17->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1->Realize();
	bSizer17->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer17 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_commitListBox->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( GitCommitListDlgBase::OnChangeCommit ), NULL, this );
	m_fileListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GitCommitListDlgBase::OnChangeFile ), NULL, this );
}

GitCommitListDlgBase::~GitCommitListDlgBase()
{
	// Disconnect Events
	m_commitListBox->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( GitCommitListDlgBase::OnChangeCommit ), NULL, this );
	m_fileListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GitCommitListDlgBase::OnChangeFile ), NULL, this );
	
}

GitDiffDlgBase::GitDiffDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Modified files") ), wxVERTICAL );
	
	m_fileListBox = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	sbSizer3->Add( m_fileListBox, 1, wxALL|wxEXPAND, 5 );
	
	bSizer18->Add( sbSizer3, 1, wxEXPAND|wxRIGHT, 5 );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Diff") ), wxVERTICAL );
	
	m_editor = new GitCommitEditor(this);
	sbSizer5->Add( m_editor, 1, wxALL|wxEXPAND, 5 );
	
	bSizer18->Add( sbSizer5, 2, wxEXPAND, 5 );
	
	bSizer17->Add( bSizer18, 2, wxALL|wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer17->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1->Realize();
	bSizer17->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer17 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_fileListBox->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GitDiffDlgBase::OnChangeFile ), NULL, this );
}

GitDiffDlgBase::~GitDiffDlgBase()
{
	// Disconnect Events
	m_fileListBox->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( GitDiffDlgBase::OnChangeFile ), NULL, this );
	
}
