///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "compiler_pages.h"

///////////////////////////////////////////////////////////////////////////

CompilerPatternsBase::CompilerPatternsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerError;
	bSizerError = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText161;
	m_staticText161 = new wxStaticText( this, wxID_ANY, _("Compiler Errors Patterns:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	m_staticText161->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizerError->Add( m_staticText161, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listErrPatterns = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer25->Add( m_listErrPatterns, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	m_btnAddErrPattern = new wxButton( this, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnAddErrPattern->SetToolTip( _("Add new error pattern") );
	
	bSizer26->Add( m_btnAddErrPattern, 0, wxALL|wxEXPAND, 5 );
	
	m_btnDelErrPattern = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnDelErrPattern->SetToolTip( _("Delete the selected error pattern") );
	
	bSizer26->Add( m_btnDelErrPattern, 0, wxALL|wxEXPAND, 5 );
	
	m_btnUpdateErrPattern = new wxButton( this, wxID_ANY, _("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnUpdateErrPattern->SetToolTip( _("Edit the selected error pattern") );
	
	bSizer26->Add( m_btnUpdateErrPattern, 0, wxALL|wxEXPAND, 5 );
	
	bSizer25->Add( bSizer26, 0, 0, 5 );
	
	bSizerError->Add( bSizer25, 1, wxEXPAND, 5 );
	
	wxStaticLine* m_staticline5;
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerError->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	bSizer14->Add( bSizerError, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerWarnings;
	bSizerWarnings = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText17;
	m_staticText17 = new wxStaticText( this, wxID_ANY, _("Compiler Warnings Patterns:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	m_staticText17->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizerWarnings->Add( m_staticText17, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer251;
	bSizer251 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listWarnPatterns = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer251->Add( m_listWarnPatterns, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer261;
	bSizer261 = new wxBoxSizer( wxVERTICAL );
	
	m_btnAddWarnPattern = new wxButton( this, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnAddWarnPattern->SetToolTip( _("Add new warning pattern") );
	
	bSizer261->Add( m_btnAddWarnPattern, 0, wxALL|wxEXPAND, 5 );
	
	m_btnDelWarnPattern = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnDelWarnPattern->SetToolTip( _("Delete the selected warning pattern") );
	
	bSizer261->Add( m_btnDelWarnPattern, 0, wxALL|wxEXPAND, 5 );
	
	m_btnUpdateWarnPattern = new wxButton( this, wxID_ANY, _("Edit..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_btnUpdateWarnPattern->SetToolTip( _("Edit the selected warning pattern") );
	
	bSizer261->Add( m_btnUpdateWarnPattern, 0, wxALL|wxEXPAND, 5 );
	
	bSizer251->Add( bSizer261, 0, 0, 5 );
	
	bSizerWarnings->Add( bSizer251, 1, wxEXPAND, 5 );
	
	bSizer14->Add( bSizerWarnings, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer14 );
	this->Layout();
	
	// Connect Events
	m_listErrPatterns->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerPatternsBase::OnErrItemActivated ), NULL, this );
	m_btnAddErrPattern->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnAddErrPattern ), NULL, this );
	m_btnDelErrPattern->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnDelErrPattern ), NULL, this );
	m_btnDelErrPattern->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CompilerPatternsBase::OnErrorPatternSelectedUI ), NULL, this );
	m_btnUpdateErrPattern->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnUpdateErrPattern ), NULL, this );
	m_btnUpdateErrPattern->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CompilerPatternsBase::OnErrorPatternSelectedUI ), NULL, this );
	m_listWarnPatterns->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerPatternsBase::OnWarnItemActivated ), NULL, this );
	m_btnAddWarnPattern->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnAddWarnPattern ), NULL, this );
	m_btnDelWarnPattern->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnDelWarnPattern ), NULL, this );
	m_btnDelWarnPattern->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CompilerPatternsBase::OnWarningPatternSelectedUI ), NULL, this );
	m_btnUpdateWarnPattern->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnUpdateWarnPattern ), NULL, this );
	m_btnUpdateWarnPattern->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CompilerPatternsBase::OnWarningPatternSelectedUI ), NULL, this );
}

CompilerPatternsBase::~CompilerPatternsBase()
{
	// Disconnect Events
	m_listErrPatterns->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerPatternsBase::OnErrItemActivated ), NULL, this );
	m_btnAddErrPattern->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnAddErrPattern ), NULL, this );
	m_btnDelErrPattern->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnDelErrPattern ), NULL, this );
	m_btnDelErrPattern->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CompilerPatternsBase::OnErrorPatternSelectedUI ), NULL, this );
	m_btnUpdateErrPattern->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnUpdateErrPattern ), NULL, this );
	m_btnUpdateErrPattern->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CompilerPatternsBase::OnErrorPatternSelectedUI ), NULL, this );
	m_listWarnPatterns->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerPatternsBase::OnWarnItemActivated ), NULL, this );
	m_btnAddWarnPattern->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnAddWarnPattern ), NULL, this );
	m_btnDelWarnPattern->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnDelWarnPattern ), NULL, this );
	m_btnDelWarnPattern->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CompilerPatternsBase::OnWarningPatternSelectedUI ), NULL, this );
	m_btnUpdateWarnPattern->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternsBase::OnBtnUpdateWarnPattern ), NULL, this );
	m_btnUpdateWarnPattern->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( CompilerPatternsBase::OnWarningPatternSelectedUI ), NULL, this );
	
}

CompilerToolsBase::CompilerToolsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 7, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText9 = new wxStaticText( this, wxID_ANY, _("C++ Compiler Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer4->Add( m_staticText9, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	m_textCompilerName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textCompilerName, 0, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, _("C Compiler Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer4->Add( m_staticText25, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlCCompilerName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textCtrlCCompilerName, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, _("Linker Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer4->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textLinkerName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, _("Shared Object Linker:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer4->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textSOLinker = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textSOLinker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText10 = new wxStaticText( this, wxID_ANY, _("Archive Tool:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer4->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textArchiveTool = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textArchiveTool, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, _("Resource Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer4->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textResourceCmp = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textResourceCmp, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText20 = new wxStaticText( this, wxID_ANY, _("PATH environment variable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	m_staticText20->SetToolTip( _("You can set here the PATH environment variable that will be used for this toolset only. The value sets here will override the value set in 'Environement Variables' table. To concatenate a value to the current PATH, use the following format: $(PATH);C:\newpath (Windows) OR $(PATH):/home/eran/bin (*nix)") );
	
	fgSizer4->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlPathVariable = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlPathVariable->SetToolTip( _("You can set here the PATH environment variable that will be used for this toolset only. The value sets here will override the value set in 'Environement Variables' table. To concatenate a value to the current PATH, use the following format: $(PATH);C:\newpath (Windows) OR $(PATH):/home/eran/bin (*nix)") );
	
	fgSizer4->Add( m_textCtrlPathVariable, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer4 );
	this->Layout();
	fgSizer4->Fit( this );
}

CompilerToolsBase::~CompilerToolsBase()
{
}

CompilerSwitchesBase::CompilerSwitchesBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText8 = new wxStaticText( this, wxID_ANY, _("Double click on an entry to modify it:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	mainSizer->Add( m_staticText8, 0, wxALL, 5 );
	
	m_listSwitches = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	mainSizer->Add( m_listSwitches, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_listSwitches->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerSwitchesBase::OnItemActivated ), NULL, this );
	m_listSwitches->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CompilerSwitchesBase::OnItemSelected ), NULL, this );
}

CompilerSwitchesBase::~CompilerSwitchesBase()
{
	// Disconnect Events
	m_listSwitches->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerSwitchesBase::OnItemActivated ), NULL, this );
	m_listSwitches->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CompilerSwitchesBase::OnItemSelected ), NULL, this );
	
}

CompilerFileTypesBase::CompilerFileTypesBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, _("Double click on an entry to modify it:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer10->Add( m_staticText23, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlFileTypes = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer12->Add( m_listCtrlFileTypes, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewFileType = new wxButton( this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonNewFileType->SetDefault(); 
	bSizer111->Add( m_buttonNewFileType, 0, wxALL, 5 );
	
	m_buttonDeleteFileType = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer111->Add( m_buttonDeleteFileType, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer111, 0, wxEXPAND, 5 );
	
	bSizer10->Add( bSizer12, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer10 );
	this->Layout();
	
	// Connect Events
	m_listCtrlFileTypes->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerFileTypesBase::OnFileTypeActivated ), NULL, this );
	m_listCtrlFileTypes->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CompilerFileTypesBase::OnFileTypeDeSelected ), NULL, this );
	m_listCtrlFileTypes->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CompilerFileTypesBase::OnFileTypeSelected ), NULL, this );
	m_buttonNewFileType->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerFileTypesBase::OnNewFileType ), NULL, this );
	m_buttonDeleteFileType->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerFileTypesBase::OnDeleteFileType ), NULL, this );
}

CompilerFileTypesBase::~CompilerFileTypesBase()
{
	// Disconnect Events
	m_listCtrlFileTypes->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerFileTypesBase::OnFileTypeActivated ), NULL, this );
	m_listCtrlFileTypes->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CompilerFileTypesBase::OnFileTypeDeSelected ), NULL, this );
	m_listCtrlFileTypes->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CompilerFileTypesBase::OnFileTypeSelected ), NULL, this );
	m_buttonNewFileType->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerFileTypesBase::OnNewFileType ), NULL, this );
	m_buttonDeleteFileType->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerFileTypesBase::OnDeleteFileType ), NULL, this );
	
}

CompilerAdvanceBase::CompilerAdvanceBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxGenerateDependenciesFiles = new wxCheckBox( this, wxID_ANY, _("Use compiler -MT switch to generate source dependencies files (*.o.d)"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_checkBoxGenerateDependenciesFiles, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText18 = new wxStaticText( this, wxID_ANY, _("Global Paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	m_staticText18->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer7->Add( m_staticText18, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText141 = new wxStaticText( this, wxID_ANY, _("Include Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	fgSizer3->Add( m_staticText141, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGlobalIncludePath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer3->Add( m_textCtrlGlobalIncludePath, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( this, wxID_ANY, _("Libraries Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer3->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGlobalLibPath = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer3->Add( m_textCtrlGlobalLibPath, 0, wxALL|wxEXPAND, 5 );
	
	bSizer7->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	bSizer11->Add( bSizer7, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline31 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer8->Add( m_staticline31, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText19 = new wxStaticText( this, wxID_ANY, _("Misc:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	m_staticText19->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer8->Add( m_staticText19, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Objects extesion:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textObjectExtension = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer2->Add( m_textObjectExtension, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText24 = new wxStaticText( this, wxID_ANY, _("Depends extension:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer2->Add( m_staticText24, 0, wxALL, 5 );
	
	m_textDependExtension = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer2->Add( m_textDependExtension, 0, wxALL, 5 );
	
	m_staticText25 = new wxStaticText( this, wxID_ANY, _("Preprocessed extension:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer2->Add( m_staticText25, 0, wxALL, 5 );
	
	m_textPreprocessExtension = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer2->Add( m_textPreprocessExtension, 0, wxALL, 5 );
	
	bSizer8->Add( fgSizer2, 0, wxALL|wxEXPAND, 0 );
	
	bSizer11->Add( bSizer8, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer11 );
	this->Layout();
	bSizer11->Fit( this );
}

CompilerAdvanceBase::~CompilerAdvanceBase()
{
}

CompilerCompilerOptionsBase::CompilerCompilerOptionsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, _("Double click on an entry to modify it:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer10->Add( m_staticText23, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCompilerOptions = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer12->Add( m_listCompilerOptions, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonCompilerOption = new wxButton( this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonCompilerOption->SetDefault(); 
	bSizer111->Add( m_buttonCompilerOption, 0, wxALL, 5 );
	
	m_buttonDeleteCompilerOption = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer111->Add( m_buttonDeleteCompilerOption, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer111, 0, wxEXPAND, 5 );
	
	bSizer10->Add( bSizer12, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer10 );
	this->Layout();
	
	// Connect Events
	m_listCompilerOptions->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerCompilerOptionsBase::OnCompilerOptionActivated ), NULL, this );
	m_listCompilerOptions->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CompilerCompilerOptionsBase::OnCompilerOptionDeSelected ), NULL, this );
	m_listCompilerOptions->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CompilerCompilerOptionsBase::OnCompilerOptionSelected ), NULL, this );
	m_buttonCompilerOption->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerCompilerOptionsBase::OnNewCompilerOption ), NULL, this );
	m_buttonDeleteCompilerOption->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerCompilerOptionsBase::OnDeleteCompilerOption ), NULL, this );
}

CompilerCompilerOptionsBase::~CompilerCompilerOptionsBase()
{
	// Disconnect Events
	m_listCompilerOptions->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerCompilerOptionsBase::OnCompilerOptionActivated ), NULL, this );
	m_listCompilerOptions->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CompilerCompilerOptionsBase::OnCompilerOptionDeSelected ), NULL, this );
	m_listCompilerOptions->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CompilerCompilerOptionsBase::OnCompilerOptionSelected ), NULL, this );
	m_buttonCompilerOption->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerCompilerOptionsBase::OnNewCompilerOption ), NULL, this );
	m_buttonDeleteCompilerOption->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerCompilerOptionsBase::OnDeleteCompilerOption ), NULL, this );
	
}

CompilerLinkerOptionsBase::CompilerLinkerOptionsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText23 = new wxStaticText( this, wxID_ANY, _("Double click on an entry to modify it:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer10->Add( m_staticText23, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listLinkerOptions = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer12->Add( m_listLinkerOptions, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonLinkerOption = new wxButton( this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonLinkerOption->SetDefault(); 
	bSizer111->Add( m_buttonLinkerOption, 0, wxALL, 5 );
	
	m_buttonDeleteLinkerOption = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer111->Add( m_buttonDeleteLinkerOption, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer111, 0, wxEXPAND, 5 );
	
	bSizer10->Add( bSizer12, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer10 );
	this->Layout();
	
	// Connect Events
	m_listLinkerOptions->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerLinkerOptionsBase::OnLinkerOptionActivated ), NULL, this );
	m_listLinkerOptions->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CompilerLinkerOptionsBase::OnLinkerOptionDeSelected ), NULL, this );
	m_listLinkerOptions->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CompilerLinkerOptionsBase::OnLinkerOptionSelected ), NULL, this );
	m_buttonLinkerOption->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerLinkerOptionsBase::OnNewLinkerOption ), NULL, this );
	m_buttonDeleteLinkerOption->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerLinkerOptionsBase::OnDeleteLinkerOption ), NULL, this );
}

CompilerLinkerOptionsBase::~CompilerLinkerOptionsBase()
{
	// Disconnect Events
	m_listLinkerOptions->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( CompilerLinkerOptionsBase::OnLinkerOptionActivated ), NULL, this );
	m_listLinkerOptions->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( CompilerLinkerOptionsBase::OnLinkerOptionDeSelected ), NULL, this );
	m_listLinkerOptions->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( CompilerLinkerOptionsBase::OnLinkerOptionSelected ), NULL, this );
	m_buttonLinkerOption->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerLinkerOptionsBase::OnNewLinkerOption ), NULL, this );
	m_buttonDeleteLinkerOption->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerLinkerOptionsBase::OnDeleteLinkerOption ), NULL, this );
	
}

CompilerOptionDlgBase::CompilerOptionDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer7->AddGrowableCol( 1 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText26 = new wxStaticText( this, wxID_ANY, _("Switch"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText26->Wrap( -1 );
	fgSizer7->Add( m_staticText26, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl18 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl18->SetValidator( wxTextValidator( wxFILTER_NONE, &m_sName ) );
	
	fgSizer7->Add( m_textCtrl18, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText27 = new wxStaticText( this, wxID_ANY, _("Help"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText27->Wrap( -1 );
	fgSizer7->Add( m_staticText27, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl19 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrl19->SetValidator( wxTextValidator( wxFILTER_NONE, &m_sHelp ) );
	
	fgSizer7->Add( m_textCtrl19, 0, wxALL|wxEXPAND, 5 );
	
	bSizer23->Add( fgSizer7, 0, wxEXPAND|wxALL, 5 );
	
	
	bSizer23->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer23->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer24->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizer23->Add( bSizer24, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizer23 );
	this->Layout();
	bSizer23->Fit( this );
}

CompilerOptionDlgBase::~CompilerOptionDlgBase()
{
}

CompilerPatternDlgBase::CompilerPatternDlgBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerError;
	bSizerError = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer41->AddGrowableCol( 1 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer41->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textPattern->SetValidator( wxTextValidator( wxFILTER_NONE, &m_pattern ) );
	
	fgSizer41->Add( m_textPattern, 1, wxEXPAND|wxALL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer41->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textFileIndex = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textFileIndex->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_fileIdx ) );
	
	fgSizer41->Add( m_textFileIndex, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer41->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textLineNumber = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textLineNumber->SetValidator( wxTextValidator( wxFILTER_NUMERIC, &m_lineIdx ) );
	
	fgSizer41->Add( m_textLineNumber, 0, wxEXPAND|wxALL, 5 );
	
	bSizerError->Add( fgSizer41, 0, wxEXPAND, 5 );
	
	
	bSizerError->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerError->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOK = new wxButton( this, wxID_ANY, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOK->SetDefault(); 
	bSizer24->Add( m_buttonOK, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer24->Add( m_buttonCancel, 0, wxALL, 5 );
	
	bSizerError->Add( bSizer24, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bSizerError );
	this->Layout();
	bSizerError->Fit( this );
	
	// Connect Events
	m_buttonOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternDlgBase::OnSubmit ), NULL, this );
}

CompilerPatternDlgBase::~CompilerPatternDlgBase()
{
	// Disconnect Events
	m_buttonOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompilerPatternDlgBase::OnSubmit ), NULL, this );
	
}
