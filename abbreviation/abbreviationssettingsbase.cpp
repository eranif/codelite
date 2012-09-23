///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "abbreviationssettingsbase.h"

///////////////////////////////////////////////////////////////////////////

AbbreviationsSettingsBase::AbbreviationsSettingsBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	m_listBoxAbbreviations = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	m_listBoxAbbreviations->SetMinSize( wxSize( 150,-1 ) );
	
	bSizer7->Add( m_listBoxAbbreviations, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonNew = new wxButton( this, wxID_ANY, _("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonNew, 0, wxALL, 5 );
	
	m_buttonDelete = new wxButton( this, wxID_ANY, _("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer8->Add( m_buttonDelete, 0, wxALL, 5 );
	
	bSizer7->Add( bSizer8, 0, 0, 5 );
	
	bSizer3->Add( bSizer7, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	bSizer3->Add( bSizer6, 0, 0, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_textCtrlName, 1, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( bSizer5, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxEmptyString ), wxVERTICAL );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("You can use | (pipe) character to indicate where the caret should be placed. \nYou can use $(VariableName) to indicate CodeLite to prompt you for suitable replacement"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	sbSizer1->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );
	
	bSizer4->Add( sbSizer1, 0, wxEXPAND, 5 );
	
	m_textCtrlExpansion = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB );
	m_textCtrlExpansion->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	m_textCtrlExpansion->SetMinSize( wxSize( 300,300 ) );
	
	bSizer4->Add( m_textCtrlExpansion, 1, wxALL|wxEXPAND, 5 );
	
	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );
	
	mainSizer->Add( bSizer3, 1, wxEXPAND|wxALL, 5 );
	
	m_checkBoxImmediateInsert = new wxCheckBox( this, wxID_ANY, _("Immediate Insert"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	mainSizer->Add( m_checkBoxImmediateInsert, 0, wxALL, 11 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonSave = new wxButton( this, wxID_OK, _("&Save"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonSave, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( bSizer2, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_listBoxAbbreviations->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( AbbreviationsSettingsBase::OnItemSelected ), NULL, this );
	m_buttonNew->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AbbreviationsSettingsBase::OnNew ), NULL, this );
	m_buttonDelete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AbbreviationsSettingsBase::OnDelete ), NULL, this );
	m_buttonDelete->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( AbbreviationsSettingsBase::OnDeleteUI ), NULL, this );
	m_textCtrlName->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AbbreviationsSettingsBase::OnMarkDirty ), NULL, this );
	m_textCtrlExpansion->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AbbreviationsSettingsBase::OnMarkDirty ), NULL, this );
	m_buttonSave->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AbbreviationsSettingsBase::OnSave ), NULL, this );
}

AbbreviationsSettingsBase::~AbbreviationsSettingsBase()
{
	// Disconnect Events
	m_listBoxAbbreviations->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( AbbreviationsSettingsBase::OnItemSelected ), NULL, this );
	m_buttonNew->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AbbreviationsSettingsBase::OnNew ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AbbreviationsSettingsBase::OnDelete ), NULL, this );
	m_buttonDelete->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( AbbreviationsSettingsBase::OnDeleteUI ), NULL, this );
	m_textCtrlName->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AbbreviationsSettingsBase::OnMarkDirty ), NULL, this );
	m_textCtrlExpansion->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( AbbreviationsSettingsBase::OnMarkDirty ), NULL, this );
	m_buttonSave->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AbbreviationsSettingsBase::OnSave ), NULL, this );
	
}
