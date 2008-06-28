///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
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
	
	m_toolBar2 = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_HORZ_TEXT|wxTB_NOICONS ); 
	m_toolBar2->AddTool( ID_TOOLNEW, wxT("New..."), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar2->AddTool( ID_TOOLDELETE, wxT("Delete"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar2->AddTool( ID_TOOLDELETEALL, wxT("Delete All"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_toolBar2->Realize();
	
	bSizer1->Add( m_toolBar2, 0, wxEXPAND|wxTOP, 5 );
	
	m_listTable = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES );
	bSizer1->Add( m_listTable, 1, wxEXPAND|wxALL, 1 );
	
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
	this->Connect( ID_TOOLNEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( SimpleTableBase::OnNewWatch ) );
	this->Connect( ID_TOOLNEW, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnNewWatchUI ) );
	this->Connect( ID_TOOLDELETE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( SimpleTableBase::OnDeleteWatch ) );
	this->Connect( ID_TOOLDELETE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnDeleteWatchUI ) );
	this->Connect( ID_TOOLDELETEALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( SimpleTableBase::OnDeleteAll ) );
	this->Connect( ID_TOOLDELETEALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnDeleteAllUI ) );
	m_listTable->Connect( wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, wxListEventHandler( SimpleTableBase::OnListEditLabelBegin ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( SimpleTableBase::OnListEditLabelEnd ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( SimpleTableBase::OnItemActivated ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( SimpleTableBase::OnItemDeSelected ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( SimpleTableBase::OnItemRightClick ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SimpleTableBase::OnItemSelected ), NULL, this );
	m_listTable->Connect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( SimpleTableBase::OnListKeyDown ), NULL, this );
	m_choiceDisplayFormat->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SimpleTableBase::OnDisplayFormat ), NULL, this );
}

SimpleTableBase::~SimpleTableBase()
{
	// Disconnect Events
	this->Disconnect( ID_TOOLNEW, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( SimpleTableBase::OnNewWatch ) );
	this->Disconnect( ID_TOOLNEW, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnNewWatchUI ) );
	this->Disconnect( ID_TOOLDELETE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( SimpleTableBase::OnDeleteWatch ) );
	this->Disconnect( ID_TOOLDELETE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnDeleteWatchUI ) );
	this->Disconnect( ID_TOOLDELETEALL, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( SimpleTableBase::OnDeleteAll ) );
	this->Disconnect( ID_TOOLDELETEALL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SimpleTableBase::OnDeleteAllUI ) );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_BEGIN_LABEL_EDIT, wxListEventHandler( SimpleTableBase::OnListEditLabelBegin ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler( SimpleTableBase::OnListEditLabelEnd ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( SimpleTableBase::OnItemActivated ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( SimpleTableBase::OnItemDeSelected ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( SimpleTableBase::OnItemRightClick ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SimpleTableBase::OnItemSelected ), NULL, this );
	m_listTable->Disconnect( wxEVT_COMMAND_LIST_KEY_DOWN, wxListEventHandler( SimpleTableBase::OnListKeyDown ), NULL, this );
	m_choiceDisplayFormat->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SimpleTableBase::OnDisplayFormat ), NULL, this );
}
