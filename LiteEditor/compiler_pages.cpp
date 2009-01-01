///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
	
	m_staticText161 = new wxStaticText( this, wxID_ANY, _("Compiler Errors Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText161->Wrap( -1 );
	m_staticText161->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizerError->Add( m_staticText161, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer41;
	fgSizer41 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer41->AddGrowableCol( 1 );
	fgSizer41->SetFlexibleDirection( wxBOTH );
	fgSizer41->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer41->Add( m_staticText5, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_textErrorPattern, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer41->Add( m_staticText6, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorFileIndex = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_textErrorFileIndex, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer41->Add( m_staticText7, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textErrorLineNumber = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer41->Add( m_textErrorLineNumber, 0, wxEXPAND|wxALL, 5 );
	
	bSizerError->Add( fgSizer41, 1, wxEXPAND, 5 );
	
	m_staticline5 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerError->Add( m_staticline5, 0, wxEXPAND | wxALL, 5 );
	
	bSizer14->Add( bSizerError, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerWarnings;
	bSizerWarnings = new wxBoxSizer( wxVERTICAL );
	
	m_staticText17 = new wxStaticText( this, wxID_ANY, _("Compiler Warnings Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText17->Wrap( -1 );
	m_staticText17->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizerWarnings->Add( m_staticText17, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText51 = new wxStaticText( this, wxID_ANY, _("Regex Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText51->Wrap( -1 );
	fgSizer5->Add( m_staticText51, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnPattern = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textWarnPattern, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText61 = new wxStaticText( this, wxID_ANY, _("File Index in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText61->Wrap( -1 );
	fgSizer5->Add( m_staticText61, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnFileIndex = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textWarnFileIndex, 0, wxEXPAND|wxALL, 5 );
	
	m_staticText71 = new wxStaticText( this, wxID_ANY, _("Line Number in Pattern:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText71->Wrap( -1 );
	fgSizer5->Add( m_staticText71, 0, wxALIGN_CENTER_VERTICAL|wxTOP|wxRIGHT|wxLEFT, 5 );
	
	m_textWarnLineNumber = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer5->Add( m_textWarnLineNumber, 0, wxEXPAND|wxALL, 5 );
	
	bSizerWarnings->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	bSizer14->Add( bSizerWarnings, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer14 );
	this->Layout();
}

CompilerPatternsBase::~CompilerPatternsBase()
{
}

CompilerToolsBase::CompilerToolsBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->AddGrowableCol( 3 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText9 = new wxStaticText( m_panel2, wxID_ANY, _("Compiler Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	fgSizer4->Add( m_staticText9, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCompilerName = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textCompilerName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText11 = new wxStaticText( m_panel2, wxID_ANY, _("Linker Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer4->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textLinkerName = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textLinkerName, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText12 = new wxStaticText( m_panel2, wxID_ANY, _("Shared Object Linker:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer4->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textSOLinker = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textSOLinker, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText10 = new wxStaticText( m_panel2, wxID_ANY, _("Archive Tool:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	fgSizer4->Add( m_staticText10, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textArchiveTool = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textArchiveTool, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText14 = new wxStaticText( m_panel2, wxID_ANY, _("Resource Compiler:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer4->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textResourceCmp = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_textResourceCmp, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText20 = new wxStaticText( m_panel2, wxID_ANY, _("PATH environment variable:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	m_staticText20->SetToolTip( _("You can set here the PATH environment variable that will be used for this toolset only. The value sets here will override the value set in 'Environement Variables' table. To concatenate a value to the current PATH, use the following format: $(PATH);C:\newpath (Windows) OR $(PATH):/home/eran/bin (*nix)") );
	
	fgSizer4->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlPathVariable = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_textCtrlPathVariable->SetToolTip( _("You can set here the PATH environment variable that will be used for this toolset only. The value sets here will override the value set in 'Environement Variables' table. To concatenate a value to the current PATH, use the following format: $(PATH);C:\newpath (Windows) OR $(PATH):/home/eran/bin (*nix)") );
	
	fgSizer4->Add( m_textCtrlPathVariable, 1, wxALL|wxEXPAND, 5 );
	
	bSizer9->Add( fgSizer4, 0, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer9 );
	m_panel2->Layout();
	bSizer9->Fit( m_panel2 );
	bSizer15->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer15 );
	this->Layout();
	bSizer15->Fit( this );
}

CompilerToolsBase::~CompilerToolsBase()
{
}

CompilerSwitchesBase::CompilerSwitchesBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_panel3 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText8 = new wxStaticText( m_panel3, wxID_ANY, _("Double click on an entry to modify it:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	bSizer3->Add( m_staticText8, 0, wxALL, 5 );
	
	m_listSwitches = new wxListCtrl( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer3->Add( m_listSwitches, 1, wxEXPAND|wxALL, 5 );
	
	m_panel3->SetSizer( bSizer3 );
	m_panel3->Layout();
	bSizer3->Fit( m_panel3 );
	mainSizer->Add( m_panel3, 1, wxEXPAND | wxALL, 5 );
	
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
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	m_panel4 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText23 = new wxStaticText( m_panel4, wxID_ANY, _("Double click on an entry to modify it:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText23->Wrap( -1 );
	bSizer10->Add( m_staticText23, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listCtrlFileTypes = new wxListCtrl( m_panel4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer12->Add( m_listCtrlFileTypes, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );
	
	m_buttonNewFileType = new wxButton( m_panel4, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonNewFileType->SetDefault(); 
	bSizer111->Add( m_buttonNewFileType, 0, wxALL, 5 );
	
	m_buttonDeleteFileType = new wxButton( m_panel4, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer111->Add( m_buttonDeleteFileType, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer111, 0, wxEXPAND, 5 );
	
	bSizer10->Add( bSizer12, 1, wxEXPAND, 5 );
	
	m_panel4->SetSizer( bSizer10 );
	m_panel4->Layout();
	bSizer10->Fit( m_panel4 );
	bSizer16->Add( m_panel4, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer16 );
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
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxGenerateDependenciesFiles = new wxCheckBox( m_panel1, wxID_ANY, _("Use compiler -MT switch to generate source dependencies files (*.o.d)"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bSizer11->Add( m_checkBoxGenerateDependenciesFiles, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline2 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer7->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText18 = new wxStaticText( m_panel1, wxID_ANY, _("Global Paths:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText18->Wrap( -1 );
	m_staticText18->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer7->Add( m_staticText18, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText141 = new wxStaticText( m_panel1, wxID_ANY, _("Include Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText141->Wrap( -1 );
	fgSizer3->Add( m_staticText141, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGlobalIncludePath = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlGlobalIncludePath, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText16 = new wxStaticText( m_panel1, wxID_ANY, _("Libraries Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText16->Wrap( -1 );
	fgSizer3->Add( m_staticText16, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlGlobalLibPath = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer3->Add( m_textCtrlGlobalLibPath, 0, wxALL|wxEXPAND, 5 );
	
	bSizer7->Add( fgSizer3, 0, wxEXPAND, 5 );
	
	bSizer11->Add( bSizer7, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	m_staticline31 = new wxStaticLine( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer8->Add( m_staticline31, 0, wxEXPAND | wxALL, 5 );
	
	m_staticText19 = new wxStaticText( m_panel1, wxID_ANY, _("Misc:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText19->Wrap( -1 );
	m_staticText19->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer8->Add( m_staticText19, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( m_panel1, wxID_ANY, _("Objects extesion:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textObjectExtension = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textObjectExtension, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText24 = new wxStaticText( m_panel1, wxID_ANY, _("Depends extension:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText24->Wrap( -1 );
	fgSizer2->Add( m_staticText24, 0, wxALL, 5 );
	
	m_textDependExtension = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textDependExtension, 0, wxALL, 5 );
	
	m_staticText25 = new wxStaticText( m_panel1, wxID_ANY, _("Preprocessed extension:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText25->Wrap( -1 );
	fgSizer2->Add( m_staticText25, 0, wxALL, 5 );
	
	m_textPreprocessExtension = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_textPreprocessExtension, 0, wxALL, 5 );
	
	bSizer8->Add( fgSizer2, 0, wxALL|wxEXPAND, 0 );
	
	bSizer11->Add( bSizer8, 0, wxEXPAND, 5 );
	
	bSizer4->Add( bSizer11, 0, wxALL|wxEXPAND, 0 );
	
	m_panel1->SetSizer( bSizer4 );
	m_panel1->Layout();
	bSizer4->Fit( m_panel1 );
	bSizer13->Add( m_panel1, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer13 );
	this->Layout();
	bSizer13->Fit( this );
}

CompilerAdvanceBase::~CompilerAdvanceBase()
{
}
