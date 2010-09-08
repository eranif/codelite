///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "findinfilesdlgbase.h"

///////////////////////////////////////////////////////////////////////////

FindInFilesDialogBase::FindInFilesDialogBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableRow( 2 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Find What :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_findString = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_findString->SetToolTip( _("String to search") );
	
	fgSizer1->Add( m_findString, 0, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Look in :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_dirPicker = new DirPicker(this, wxID_ANY, wxT("..."), wxEmptyString, _("Select a folder:"), wxDefaultPosition, wxDefaultSize, wxDP_USE_COMBOBOX);
	bSizer10->Add( m_dirPicker, 1, wxTOP|wxBOTTOM|wxLEFT, 5 );
	
	m_btnAddPath = new wxButton( this, wxID_ANY, _("&Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer10->Add( m_btnAddPath, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT, 5 );
	
	fgSizer1->Add( bSizer10, 1, wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_listPaths = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer1->Add( m_listPaths, 1, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_btnRemovelPath = new wxButton( this, wxID_ANY, _("-"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_btnRemovelPath->SetToolTip( _("Remove the selected search path") );
	
	bSizer9->Add( m_btnRemovelPath, 0, wxEXPAND|wxALL, 5 );
	
	m_btnClearPaths = new wxButton( this, wxID_ANY, _("X"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_btnClearPaths->SetToolTip( _("Clear the search path list") );
	
	bSizer9->Add( m_btnClearPaths, 0, wxEXPAND|wxALL, 5 );
	
	fgSizer1->Add( bSizer9, 0, wxEXPAND, 5 );
	
	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("File Mask:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_fileTypes = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_fileTypes->Append( _("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc") );
	m_fileTypes->Append( _("*.*") );
	m_fileTypes->SetToolTip( _("Search these file types") );
	
	fgSizer1->Add( m_fileTypes, 1, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Files Encoding:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	fgSizer1->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceEncodingChoices;
	m_choiceEncoding = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceEncodingChoices, 0 );
	m_choiceEncoding->SetSelection( 0 );
	m_choiceEncoding->SetToolTip( _("Use this file encoding when scanning files for matches") );
	
	fgSizer1->Add( m_choiceEncoding, 1, wxALL|wxEXPAND, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText7 = new wxStaticText( this, wxID_ANY, _("Options:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel1 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_matchCase = new wxCheckBox( m_panel1, wxID_ANY, _("&Match case"), wxDefaultPosition, wxDefaultSize, 0 );
	m_matchCase->SetToolTip( _("Toggle case sensitive search") );
	
	fgSizer3->Add( m_matchCase, 0, wxALL|wxEXPAND, 5 );
	
	m_matchWholeWord = new wxCheckBox( m_panel1, wxID_ANY, _("Match &whole word"), wxDefaultPosition, wxDefaultSize, 0 );
	m_matchWholeWord->SetToolTip( _("Toggle whole word search") );
	
	fgSizer3->Add( m_matchWholeWord, 0, wxALL|wxEXPAND, 5 );
	
	m_regualrExpression = new wxCheckBox( m_panel1, wxID_ANY, _("Regular &expression"), wxDefaultPosition, wxDefaultSize, 0 );
	m_regualrExpression->SetToolTip( _("The 'Find What' field is a regular expression") );
	
	fgSizer3->Add( m_regualrExpression, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxSaveFilesBeforeSearching = new wxCheckBox( m_panel1, wxID_ANY, _("&Save modified files before searching"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxSaveFilesBeforeSearching->SetToolTip( _("Save any modified files before search starts") );
	
	fgSizer3->Add( m_checkBoxSaveFilesBeforeSearching, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( fgSizer3, 1, wxEXPAND|wxALL, 5 );
	
	m_panel1->SetSizer( bSizer6 );
	m_panel1->Layout();
	bSizer6->Fit( m_panel1 );
	m_notebook1->AddPage( m_panel1, _("General"), true );
	m_panel2 = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->SetFlexibleDirection( wxBOTH );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_printScope = new wxCheckBox( m_panel2, wxID_ANY, _("Display the C++ scope of the match"), wxDefaultPosition, wxDefaultSize, 0 );
	m_printScope->SetToolTip( _("Display the class + function name for the matches") );
	
	fgSizer4->Add( m_printScope, 0, wxALL, 5 );
	
	m_checkBoxSkipMatchesFoundInComments = new wxCheckBox( m_panel2, wxID_ANY, _("Hide matches found in comments"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_checkBoxSkipMatchesFoundInComments, 0, wxALL, 5 );
	
	m_checkBoxSkipMatchesFoundInStrings = new wxCheckBox( m_panel2, wxID_ANY, _("Hide matches found inside strings"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_checkBoxSkipMatchesFoundInStrings, 0, wxALL, 5 );
	
	m_checkBoxHighlighStringComments = new wxCheckBox( m_panel2, wxID_ANY, _("Use different colour for matches found in comments"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer4->Add( m_checkBoxHighlighStringComments, 0, wxALL, 5 );
	
	bSizer7->Add( fgSizer4, 1, wxEXPAND|wxALL, 5 );
	
	m_panel2->SetSizer( bSizer7 );
	m_panel2->Layout();
	bSizer7->Fit( m_panel2 );
	m_notebook1->AddPage( m_panel2, _("C++"), false );
	
	fgSizer1->Add( m_notebook1, 1, wxEXPAND|wxALL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Show Results In:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	fgSizer1->Add( m_staticText6, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 0, 2, 0, 0 );
	
	m_resInNewTab = new wxRadioButton( this, wxID_ANY, _("New tab"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_resInNewTab->SetValue( true ); 
	gSizer1->Add( m_resInNewTab, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_resInActiveTab = new wxRadioButton( this, wxID_ANY, _("Active tab (overwrite last search)"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer1->Add( m_resInActiveTab, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	fgSizer1->Add( gSizer1, 0, wxTOP|wxBOTTOM, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	bSizer91->Add( fgSizer1, 1, wxEXPAND|wxALL, 5 );
	
	bSizer1->Add( bSizer91, 1, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL|wxLI_VERTICAL );
	bSizer1->Add( m_staticline1, 0, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_find = new wxButton( this, wxID_FIND, _("&Find"), wxDefaultPosition, wxDefaultSize, 0 );
	m_find->SetDefault(); 
	m_find->SetToolTip( _("Begin search") );
	
	bSizer2->Add( m_find, 0, wxALL|wxEXPAND, 5 );
	
	m_replaceAll = new wxButton( this, wxID_REPLACE, _("Find &Replace Candidates"), wxDefaultPosition, wxDefaultSize, 0 );
	m_replaceAll->SetToolTip( _("Search for matches and place them in the 'Replace' window as candidates for possible replace operation") );
	
	bSizer2->Add( m_replaceAll, 0, wxALL|wxEXPAND, 5 );
	
	m_stop = new wxButton( this, wxID_STOP, _("Sto&p Search"), wxDefaultPosition, wxDefaultSize, 0 );
	m_stop->SetToolTip( _("Stop the search operation") );
	
	bSizer2->Add( m_stop, 0, wxALL|wxEXPAND, 5 );
	
	m_cancel = new wxButton( this, wxID_CLOSE, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	m_cancel->SetToolTip( _("Close this dialog") );
	
	bSizer2->Add( m_cancel, 0, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( FindInFilesDialogBase::OnClose ) );
	m_btnAddPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnAddPath ), NULL, this );
	m_btnRemovelPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnRemovePath ), NULL, this );
	m_btnRemovelPath->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnRemovePathUI ), NULL, this );
	m_btnClearPaths->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClearPaths ), NULL, this );
	m_btnClearPaths->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnClearPathsUI ), NULL, this );
	m_matchCase->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_matchWholeWord->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_regualrExpression->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxSaveFilesBeforeSearching->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_printScope->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxSkipMatchesFoundInComments->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxSkipMatchesFoundInStrings->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxHighlighStringComments->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxHighlighStringComments->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnUseDiffColourForCommentsUI ), NULL, this );
	m_find->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_find->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnFindWhatUI ), NULL, this );
	m_replaceAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_replaceAll->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnFindWhatUI ), NULL, this );
	m_stop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
}

FindInFilesDialogBase::~FindInFilesDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( FindInFilesDialogBase::OnClose ) );
	m_btnAddPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnAddPath ), NULL, this );
	m_btnRemovelPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnRemovePath ), NULL, this );
	m_btnRemovelPath->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnRemovePathUI ), NULL, this );
	m_btnClearPaths->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClearPaths ), NULL, this );
	m_btnClearPaths->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnClearPathsUI ), NULL, this );
	m_matchCase->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_matchWholeWord->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_regualrExpression->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxSaveFilesBeforeSearching->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_printScope->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxSkipMatchesFoundInComments->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxSkipMatchesFoundInStrings->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxHighlighStringComments->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_checkBoxHighlighStringComments->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnUseDiffColourForCommentsUI ), NULL, this );
	m_find->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_find->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnFindWhatUI ), NULL, this );
	m_replaceAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_replaceAll->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( FindInFilesDialogBase::OnFindWhatUI ), NULL, this );
	m_stop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
}
