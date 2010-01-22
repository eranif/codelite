///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "subversion2_ui.h"

///////////////////////////////////////////////////////////////////////////

SubversionPageBase::SubversionPageBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrlRootDir = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	bSizer2->Add( m_textCtrlRootDir, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonChangeRootDir = new wxButton( this, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_buttonChangeRootDir->SetToolTip( _("Select a different root directory") );
	
	bSizer2->Add( m_buttonChangeRootDir, 0, wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	mainSizer->Add( bSizer2, 0, wxEXPAND, 5 );
	
	m_treeCtrl = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_MULTIPLE );
	mainSizer->Add( m_treeCtrl, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_buttonChangeRootDir->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SubversionPageBase::OnChangeRootDir ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( SubversionPageBase::OnItemActivated ), NULL, this );
	m_treeCtrl->Connect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( SubversionPageBase::OnTreeMenu ), NULL, this );
}

SubversionPageBase::~SubversionPageBase()
{
	// Disconnect Events
	m_buttonChangeRootDir->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SubversionPageBase::OnChangeRootDir ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( SubversionPageBase::OnItemActivated ), NULL, this );
	m_treeCtrl->Disconnect( wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEventHandler( SubversionPageBase::OnTreeMenu ), NULL, this );
}

CommitDialogBase::CommitDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE|wxSP_NO_XP_THEME );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( CommitDialogBase::m_splitter1OnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, _("Modified Paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer5->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );
	
	wxArrayString m_checkListFilesChoices;
	m_checkListFiles = new wxCheckListBox( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkListFilesChoices, 0 );
	bSizer5->Add( m_checkListFiles, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer5 );
	m_panel1->Layout();
	bSizer5->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panel2, wxID_ANY, _("Message:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer6->Add( m_staticText1, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlMessage = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_RICH2 );
	bSizer6->Add( m_textCtrlMessage, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer6 );
	m_panel2->Layout();
	bSizer6->Fit( m_panel2 );
	m_splitter1->SplitVertically( m_panel1, m_panel2, 0 );
	bSizer3->Add( m_splitter1, 1, wxEXPAND|wxALL, 5 );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, _("Recent commit messages:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	bSizer3->Add( m_staticText27, 0, wxALL|wxEXPAND, 5 );
	
	wxArrayString m_choiceMessagesChoices;
	m_choiceMessages = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceMessagesChoices, 0 );
	m_choiceMessages->SetSelection( 0 );
	bSizer3->Add( m_choiceMessages, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer3->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button2->SetDefault(); 
	bSizer4->Add( m_button2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_button3 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button3, 0, wxALL, 5 );
	
	bSizer3->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	bSizer3->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_choiceMessages->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CommitDialogBase::OnChoiceMessage ), NULL, this );
}

CommitDialogBase::~CommitDialogBase()
{
	// Disconnect Events
	m_choiceMessages->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CommitDialogBase::OnChoiceMessage ), NULL, this );
}

SvnCopyDialogBase::SvnCopyDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Source URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlSourceURL = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlSourceURL, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Target URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlTargetURL = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlTargetURL, 1, wxALL|wxEXPAND, 5 );
	
	bSizer7->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_staticline3 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline3, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Comment:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer7->Add( m_staticText5, 0, wxALL, 5 );
	
	m_textCtrlComment = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	bSizer7->Add( m_textCtrlComment, 1, wxALL|wxEXPAND, 5 );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button4 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button4->SetDefault(); 
	bSizer8->Add( m_button4, 0, wxALL, 5 );
	
	m_button5 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_button5, 0, wxALL, 5 );
	
	bSizer7->Add( bSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer7 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

SvnCopyDialogBase::~SvnCopyDialogBase()
{
}

SvnShellBase::SvnShellBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_sci = new wxScintilla(this);
	bSizer9->Add( m_sci, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer9 );
	this->Layout();
}

SvnShellBase::~SvnShellBase()
{
}

SvnLoginDialogBase::SvnLoginDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Username:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer2->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlUsername = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlUsername->SetMinSize( wxSize( 300,-1 ) );
	
	fgSizer2->Add( m_textCtrlUsername, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer2->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlPassword = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD|wxTE_RICH2 );
	fgSizer2->Add( m_textCtrlPassword, 0, wxALL|wxEXPAND, 5 );
	
	bSizer10->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer10->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button6 = new wxButton( this, wxID_OK, _("&Login"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button6->SetDefault(); 
	bSizer11->Add( m_button6, 0, wxALL, 5 );
	
	m_button7 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_button7, 0, wxALL, 5 );
	
	bSizer10->Add( bSizer11, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer10 );
	this->Layout();
	bSizer10->Fit( this );
	
	this->Centre( wxBOTH );
}

SvnLoginDialogBase::~SvnLoginDialogBase()
{
}

SvnPreferencesDialogBase::SvnPreferencesDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );
	
	m_notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	wxSize m_notebookImageSize = wxSize( 32,32 );
	int m_notebookIndex = 0;
	wxImageList* m_notebookImages = new wxImageList( m_notebookImageSize.GetWidth(), m_notebookImageSize.GetHeight() );
	m_notebook->AssignImageList( m_notebookImages );
	wxBitmap m_notebookBitmap;
	wxImage m_notebookImage;
	m_panel3 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticTextExe = new wxStaticText( m_panel3, wxID_ANY, _("Executable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextExe->Wrap( -1 );
	fgSizer3->Add( m_staticTextExe, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlSvnExecutable = new wxTextCtrl( m_panel3, wxID_ANY, _("svn"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlSvnExecutable, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowse = new wxButton( m_panel3, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_buttonBrowse, 0, wxALL, 5 );
	
	bSizer15->Add( fgSizer3, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText9 = new wxStaticText( m_panel3, wxID_ANY, _("Ignore the following file patterns:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer15->Add( m_staticText9, 0, wxALL, 5 );
	
	m_textCtrlIgnorePattern = new wxTextCtrl( m_panel3, wxID_ANY, _("*.o *.obj *.exe *.lib *.so *.dll *.a *.dynlib *.exp *.ilk *.pdb *.d *.tags *.suo *.ncb *.bak *.orig *.dll *.mine *.o.d *.session Debug Release DebugUnicode ReleaseUnicode"), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	bSizer15->Add( m_textCtrlIgnorePattern, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel3, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_checkBoxAddToSvn = new wxCheckBox( m_panel3, wxID_ANY, _("When adding file(s) to project, add it to svn as well"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxAddToSvn, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxRetag = new wxCheckBox( m_panel3, wxID_ANY, _("Retag workspace after svn update, revert or applying patch"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxRetag, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxRenameFile = new wxCheckBox( m_panel3, wxID_ANY, _("When renaming a file in the project, rename it in the repository as well"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxRenameFile, 0, wxALL|wxEXPAND, 5 );
	
	bSizer15->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	m_panel3->SetSizer( bSizer15 );
	m_panel3->Layout();
	bSizer15->Fit( m_panel3 );
	m_notebook->AddPage( m_panel3, _("General"), true );
	m_panel4 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxUseExternalDiff = new wxCheckBox( m_panel4, wxID_ANY, _("Use external diff tool"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( m_checkBoxUseExternalDiff, 0, wxALL|wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText10 = new wxStaticText( m_panel4, wxID_ANY, _("External Diff Viewer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer5->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlDiffViewer = new wxTextCtrl( m_panel4, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textCtrlDiffViewer, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_buttonBrowseExtDiff = new wxButton( m_panel4, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_buttonBrowseExtDiff, 0, wxALL, 5 );
	
	bSizer16->Add( fgSizer5, 0, wxEXPAND|wxALL, 5 );
	
	m_panel4->SetSizer( bSizer16 );
	m_panel4->Layout();
	bSizer16->Fit( m_panel4 );
	m_notebook->AddPage( m_panel4, _("External Diff"), false );
	m_panel5 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer161;
	bSizer161 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer6->AddGrowableCol( 1 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText20 = new wxStaticText( m_panel5, wxID_ANY, _("SSH Client:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	fgSizer6->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlSSHClient = new wxTextCtrl( m_panel5, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlSSHClient, 0, wxALL|wxEXPAND, 5 );
	
	m_button12 = new wxButton( m_panel5, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_button12, 0, wxALL, 5 );
	
	m_staticText21 = new wxStaticText( m_panel5, wxID_ANY, _("SSH Client arguments:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	fgSizer6->Add( m_staticText21, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlSshClientArgs = new wxTextCtrl( m_panel5, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer6->Add( m_textCtrlSshClientArgs, 0, wxALL|wxEXPAND, 5 );
	
	bSizer161->Add( fgSizer6, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel5, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText22 = new wxStaticText( m_panel5, wxID_ANY, _("The SSH client field should contain the command to be\nused by the SVN command line client for establishing a secured channel. \n\nFor example, on Windows it should contain something like:\n/path/to/plink.exe -l <user name> -pw <svn password>\n\nIf you dont need SSH channel, leave this field empty"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText22->Wrap( -1 );
	sbSizer2->Add( m_staticText22, 0, wxALL|wxEXPAND, 5 );
	
	bSizer161->Add( sbSizer2, 1, wxEXPAND|wxALL, 5 );
	
	m_panel5->SetSizer( bSizer161 );
	m_panel5->Layout();
	bSizer161->Fit( m_panel5 );
	m_notebook->AddPage( m_panel5, _("SSH Client"), false );
	m_panel6 = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxExposeRevisionMacro = new wxCheckBox( m_panel6, wxID_ANY, _("Add revision number as preprocessor definition in the compilation line"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer23->Add( m_checkBoxExposeRevisionMacro, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText29 = new wxStaticText( m_panel6, wxID_ANY, _("Preprocessor name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer24->Add( m_staticText29, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlMacroName = new wxTextCtrl( m_panel6, wxID_ANY, _("SVN_REVISION"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_textCtrlMacroName, 1, wxALL|wxEXPAND, 5 );
	
	bSizer23->Add( bSizer24, 0, wxEXPAND|wxALL, 5 );
	
	m_panel6->SetSizer( bSizer23 );
	m_panel6->Layout();
	bSizer23->Fit( m_panel6 );
	m_notebook->AddPage( m_panel6, _("Integration"), false );
	
	bSizer12->Add( m_notebook, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button8 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button8->SetDefault(); 
	bSizer13->Add( m_button8, 0, wxALL, 5 );
	
	m_button9 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_button9, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer13, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer12 );
	this->Layout();
	bSizer12->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_buttonBrowse->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnPreferencesDialogBase::OnBrowseSvnExe ), NULL, this );
	m_staticText10->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnUseExternalDiffUI ), NULL, this );
	m_textCtrlDiffViewer->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnUseExternalDiffUI ), NULL, this );
	m_buttonBrowseExtDiff->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnPreferencesDialogBase::OnBrowseDiffViewer ), NULL, this );
	m_buttonBrowseExtDiff->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnUseExternalDiffUI ), NULL, this );
	m_button12->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnPreferencesDialogBase::OnBrowseSSHClient ), NULL, this );
	m_staticText29->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnAddRevisionMacroUI ), NULL, this );
	m_textCtrlMacroName->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnAddRevisionMacroUI ), NULL, this );
	m_button8->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnPreferencesDialogBase::OnButtonOK ), NULL, this );
}

SvnPreferencesDialogBase::~SvnPreferencesDialogBase()
{
	// Disconnect Events
	m_buttonBrowse->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnPreferencesDialogBase::OnBrowseSvnExe ), NULL, this );
	m_staticText10->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnUseExternalDiffUI ), NULL, this );
	m_textCtrlDiffViewer->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnUseExternalDiffUI ), NULL, this );
	m_buttonBrowseExtDiff->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnPreferencesDialogBase::OnBrowseDiffViewer ), NULL, this );
	m_buttonBrowseExtDiff->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnUseExternalDiffUI ), NULL, this );
	m_button12->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnPreferencesDialogBase::OnBrowseSSHClient ), NULL, this );
	m_staticText29->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnAddRevisionMacroUI ), NULL, this );
	m_textCtrlMacroName->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnPreferencesDialogBase::OnAddRevisionMacroUI ), NULL, this );
	m_button8->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnPreferencesDialogBase::OnButtonOK ), NULL, this );
}

SvnInfoDialog::SvnInfoDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer7->AddGrowableCol( 1 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText19 = new wxStaticText( this, wxID_ANY, _("Root URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	m_staticText19->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer7->Add( m_staticText19, 0, wxALL, 5 );
	
	m_textCtrlRootURL = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 500,-1 ), wxTE_READONLY|wxTE_RICH2 );
	m_textCtrlRootURL->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	fgSizer7->Add( m_textCtrlRootURL, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, _("URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	m_staticText21->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer7->Add( m_staticText21, 0, wxALL, 5 );
	
	m_textCtrlURL = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RICH2 );
	m_textCtrlURL->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	fgSizer7->Add( m_textCtrlURL, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, _("Revision:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	m_staticText23->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer7->Add( m_staticText23, 0, wxALL, 5 );
	
	m_textCtrlRevision = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RICH2 );
	m_textCtrlRevision->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	fgSizer7->Add( m_textCtrlRevision, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, _("Author:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	m_staticText25->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer7->Add( m_staticText25, 0, wxALL, 5 );
	
	m_textCtrlAuthor = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RICH2 );
	m_textCtrlAuthor->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	fgSizer7->Add( m_textCtrlAuthor, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, _("Date:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	m_staticText27->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	fgSizer7->Add( m_staticText27, 0, wxALL, 5 );
	
	m_textCtrlDate = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RICH2 );
	m_textCtrlDate->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	fgSizer7->Add( m_textCtrlDate, 0, wxALL|wxEXPAND, 5 );
	
	sbSizer3->Add( fgSizer7, 1, wxEXPAND|wxALL, 5 );
	
	bSizer17->Add( sbSizer3, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button13 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button13->SetDefault(); 
	bSizer18->Add( m_button13, 0, wxALL, 5 );
	
	bSizer17->Add( bSizer18, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer17 );
	this->Layout();
	bSizer17->Fit( this );
	
	this->Centre( wxBOTH );
}

SvnInfoDialog::~SvnInfoDialog()
{
}

SvnCheckoutDialogBase::SvnCheckoutDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer8->AddGrowableCol( 1 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText24 = new wxStaticText( this, wxID_ANY, _("URL of repository:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer8->Add( m_staticText24, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_comboBoxRepoURL = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer8->Add( m_comboBoxRepoURL, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer8->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, _("Checkout directory:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer8->Add( m_staticText25, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl20 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_textCtrl20, 0, wxALL|wxEXPAND, 5 );
	
	m_buttonBrowseDir = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer8->Add( m_buttonBrowseDir, 0, wxALL, 5 );
	
	sbSizer4->Add( fgSizer8, 1, wxEXPAND|wxALL, 5 );
	
	bSizer19->Add( sbSizer4, 0, wxEXPAND|wxALL, 5 );
	
	
	bSizer19->Add( 0, 0, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button14 = new wxButton( this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button14->SetDefault(); 
	bSizer20->Add( m_button14, 0, wxALL, 5 );
	
	m_button15 = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_button15, 0, wxALL, 5 );
	
	bSizer19->Add( bSizer20, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizer19 );
	this->Layout();
	bSizer19->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_comboBoxRepoURL->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SvnCheckoutDialogBase::OnCheckoutDirectoryText ), NULL, this );
	m_buttonBrowseDir->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnCheckoutDialogBase::OnBrowseDirectory ), NULL, this );
	m_button14->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnCheckoutDialogBase::OnOK ), NULL, this );
	m_button14->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnCheckoutDialogBase::OnOkUI ), NULL, this );
}

SvnCheckoutDialogBase::~SvnCheckoutDialogBase()
{
	// Disconnect Events
	m_comboBoxRepoURL->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SvnCheckoutDialogBase::OnCheckoutDirectoryText ), NULL, this );
	m_buttonBrowseDir->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnCheckoutDialogBase::OnBrowseDirectory ), NULL, this );
	m_button14->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SvnCheckoutDialogBase::OnOK ), NULL, this );
	m_button14->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SvnCheckoutDialogBase::OnOkUI ), NULL, this );
}

SvnLogDialog::SvnLogDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer9->AddGrowableCol( 1 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText28 = new wxStaticText( this, wxID_ANY, _("From revision:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText28->Wrap( -1 );
	fgSizer9->Add( m_staticText28, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_from = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), 0 );
	fgSizer9->Add( m_from, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText29 = new wxStaticText( this, wxID_ANY, _("To revision:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	fgSizer9->Add( m_staticText29, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_to = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer9->Add( m_to, 0, wxALL|wxEXPAND, 5 );
	
	bSizer21->Add( fgSizer9, 0, wxEXPAND|wxALL, 5 );
	
	
	bSizer21->Add( 0, 0, 1, wxEXPAND|wxALL, 5 );
	
	m_compact = new wxCheckBox( this, wxID_ANY, _("Create compact log"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( m_compact, 0, wxALL|wxEXPAND, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer21->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button17 = new wxButton( this, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button17->SetDefault(); 
	bSizer22->Add( m_button17, 0, wxALL, 5 );
	
	m_button18 = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer22->Add( m_button18, 0, wxALL, 5 );
	
	bSizer21->Add( bSizer22, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer21 );
	this->Layout();
	bSizer21->Fit( this );
	
	this->Centre( wxBOTH );
}

SvnLogDialog::~SvnLogDialog()
{
}
