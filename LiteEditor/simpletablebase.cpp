///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "simpletablebase.h"

///////////////////////////////////////////////////////////////////////////

SimpleTableBase::SimpleTableBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_listTable = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer3->Add( m_listTable, 1, wxEXPAND|wxALL, 1 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_button1 = new wxButton( this, wxID_ANY, wxT("New..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button1, 0, wxALL|wxEXPAND, 5 );

	m_button2 = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button2, 0, wxALL|wxEXPAND, 5 );

	m_button3 = new wxButton( this, wxID_ANY, wxT("Delete All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button3, 0, wxALL|wxEXPAND, 5 );

	bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );

	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_staticTextDisplayText = new wxStaticText( this, wxID_ANY, wxT("Display Format:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextDisplayText->Wrap( -1 );
	bSizer2->Add( m_staticTextDisplayText, 0, wxALL, 5 );

	wxString m_choiceDisplayFormatChoices[] = { wxT("natural"), wxT("hexadecimal"), wxT("binary"), wxT("octal"), wxT("decimal") };
	int m_choiceDisplayFormatNChoices = sizeof( m_choiceDisplayFormatChoices ) / sizeof( wxString );
	m_choiceDisplayFormat = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceDisplayFormatNChoices, m_choiceDisplayFormatChoices, 0 );
	m_choiceDisplayFormat->SetSelection( 0 );
	bSizer2->Add( m_choiceDisplayFormat, 0, 0, 5 );

	bSizer1->Add( bSizer2, 0, 0, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	m_listTable->Connect( wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, wxListEventHandler( SimpleTableBase::OnListEditLabelBegin ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( SimpleTableBase::OnListEditLabelEnd ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( SimpleTableBase::OnItemActivated ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( SimpleTableBase::OnItemDeSelected ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( SimpleTableBase::OnItemRightClick ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SimpleTableBase::OnItemSelected ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( SimpleTableBase::OnListKeyDown ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleTableBase::OnNewWatch ), NULL, this );
	m_button1->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnNewWatchUI ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleTableBase::OnDeleteWatch ), NULL, this );
	m_button2->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnDeleteWatchUI ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleTableBase::OnDeleteAll ), NULL, this );
	m_button3->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnDeleteAllUI ), NULL, this );
	m_choiceDisplayFormat->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SimpleTableBase::OnDisplayFormat ), NULL, this );
}

SimpleTableBase::~SimpleTableBase()
{
	// Disconnect Events
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, wxListEventHandler( SimpleTableBase::OnListEditLabelBegin ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( SimpleTableBase::OnListEditLabelEnd ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( SimpleTableBase::OnItemActivated ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( SimpleTableBase::OnItemDeSelected ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( SimpleTableBase::OnItemRightClick ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SimpleTableBase::OnItemSelected ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( SimpleTableBase::OnListKeyDown ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleTableBase::OnNewWatch ), NULL, this );
	m_button1->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnNewWatchUI ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleTableBase::OnDeleteWatch ), NULL, this );
	m_button2->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnDeleteWatchUI ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SimpleTableBase::OnDeleteAll ), NULL, this );
	m_button3->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnDeleteAllUI ), NULL, this );
	m_choiceDisplayFormat->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SimpleTableBase::OnDisplayFormat ), NULL, this );
}

LocalsTableBase::LocalsTableBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_listTable = new wxListView( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer3->Add( m_listTable, 1, wxEXPAND|wxALL, 1 );

	bSizer1->Add( bSizer3, 1, wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( LocalsTableBase::OnItemActivated ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( LocalsTableBase::OnItemSelected ), NULL, this );
}

LocalsTableBase::~LocalsTableBase()
{
	// Disconnect Events
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( LocalsTableBase::OnItemActivated ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( LocalsTableBase::OnItemSelected ), NULL, this );
}
