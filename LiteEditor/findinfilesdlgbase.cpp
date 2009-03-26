///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
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
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Find What :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_findString = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer1->Add( m_findString, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Look in :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );
	
	m_dirPicker = new DirPicker(this, wxID_ANY, wxT("..."), wxEmptyString, _("Select a folder:"), wxDefaultPosition, wxDefaultSize, wxDP_USE_COMBOBOX);
	bSizer10->Add( m_dirPicker, 1, wxALL, 5 );
	
	m_btnAddPath = new wxButton( this, wxID_ANY, _("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	bSizer10->Add( m_btnAddPath, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	fgSizer1->Add( bSizer10, 1, wxEXPAND, 5 );
	
	bSizer1->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	
	m_listPaths = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer6->Add( m_listPaths, 1, wxRIGHT|wxLEFT|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );
	
	m_btnRemovelPath = new wxButton( this, wxID_ANY, _("-"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_btnRemovelPath->SetToolTip( _("Remove the selected path from the search paths") );
	
	bSizer9->Add( m_btnRemovelPath, 0, wxALL|wxEXPAND, 5 );
	
	m_btnClearPaths = new wxButton( this, wxID_ANY, _("X"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_btnClearPaths->SetToolTip( _("Remove all search paths") );
	
	bSizer9->Add( m_btnClearPaths, 0, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( bSizer9, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer1->Add( bSizer6, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Options") ), wxVERTICAL );
	
	m_matchCase = new wxCheckBox( this, wxID_ANY, _("&Match case"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer1->Add( m_matchCase, 0, wxALL, 5 );
	
	m_matchWholeWord = new wxCheckBox( this, wxID_ANY, _("Match &whole word"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer1->Add( m_matchWholeWord, 0, wxALL, 5 );
	
	m_regualrExpression = new wxCheckBox( this, wxID_ANY, _("Regular &expression"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer1->Add( m_regualrExpression, 0, wxALL, 5 );
	
	m_fontEncoding = new wxCheckBox( this, wxID_ANY, _("Use the editor's font encoding (when left unchecked encoding is set to UTF8)"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer1->Add( m_fontEncoding, 0, wxALL, 5 );
	
	m_printScope = new wxCheckBox( this, wxID_ANY, _("Display C++ scope (class/function) in result match"), wxDefaultPosition, wxDefaultSize, 0 );
	
	sbSizer1->Add( m_printScope, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Look at these file &types :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer2->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_fileTypes = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_fileTypes->Append( _("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.hxx;*.hh;*.inl;*.inc") );
	m_fileTypes->Append( _("*.*") );
	fgSizer2->Add( m_fileTypes, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticText* m_staticText4;
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Display search results in tab :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_searchResultsTabChoices;
	m_searchResultsTab = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_searchResultsTabChoices, 0 );
	m_searchResultsTab->SetSelection( 0 );
	fgSizer2->Add( m_searchResultsTab, 1, wxALL|wxEXPAND, 5 );
	
	sbSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	bSizer1->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_find = new wxButton( this, wxID_ANY, _("&Find"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_find, 1, wxALL|wxEXPAND, 5 );
	
	m_replaceAll = new wxButton( this, wxID_ANY, _("Find &Replace Candidates"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_replaceAll, 1, wxALL|wxEXPAND, 5 );
	
	m_stop = new wxButton( this, wxID_ANY, _("&Stop Search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_stop, 1, wxALL|wxEXPAND, 5 );
	
	m_cancel = new wxButton( this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_cancel, 1, wxALL|wxEXPAND, 5 );
	
	bSizer1->Add( bSizer2, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( FindInFilesDialogBase::OnClose ) );
	m_btnAddPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnAddPath ), NULL, this );
	m_btnRemovelPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnRemovePath ), NULL, this );
	m_btnClearPaths->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClearPaths ), NULL, this );
	m_matchCase->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_matchWholeWord->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_regualrExpression->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_fontEncoding->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_printScope->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_find->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_replaceAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_stop->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
}

FindInFilesDialogBase::~FindInFilesDialogBase()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( FindInFilesDialogBase::OnClose ) );
	m_btnAddPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnAddPath ), NULL, this );
	m_btnRemovelPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnRemovePath ), NULL, this );
	m_btnClearPaths->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClearPaths ), NULL, this );
	m_matchCase->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_matchWholeWord->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_regualrExpression->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_fontEncoding->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_printScope->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_find->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_replaceAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_stop->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
	m_cancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindInFilesDialogBase::OnClick ), NULL, this );
}
