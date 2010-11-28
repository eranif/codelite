///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "implementparentvirtualfunctionsbase.h"

///////////////////////////////////////////////////////////////////////////

ImplementParentVirtualFunctionsBase::ImplementParentVirtualFunctionsBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3DSASH|wxSP_LIVE_UPDATE|wxSP_NO_XP_THEME );
	m_splitter1->SetSashGravity( 0.5 );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( ImplementParentVirtualFunctionsBase::m_splitter1OnIdle ), NULL, this );
	
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, _("Prototype(s):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer5->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_textCtrlProtos = new wxTextCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	m_textCtrlProtos->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer5->Add( m_textCtrlProtos, 1, wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer5 );
	m_panel1->Layout();
	bSizer5->Fit( m_panel1 );
	m_panel2 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText2 = new wxStaticText( m_panel2, wxID_ANY, _("Implementation(s):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer4->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlImplFile = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_textCtrlImplFile, 1, wxALL|wxEXPAND, 5 );
	
	bSizer6->Add( bSizer4, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_textCtrlImpl = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH2 );
	m_textCtrlImpl->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	bSizer6->Add( m_textCtrlImpl, 1, wxALL|wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer6 );
	m_panel2->Layout();
	bSizer6->Fit( m_panel2 );
	m_splitter1->SplitHorizontally( m_panel1, m_panel2, 0 );
	mainSizer->Add( m_splitter1, 1, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Options:") ), wxVERTICAL );
	
	m_checkBoxFormat = new wxCheckBox( this, wxID_ANY, _("Format text after insertion"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxFormat, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxAddDoxy = new wxCheckBox( this, wxID_ANY, _("Add doxygen comments for the functions"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxAddDoxy, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxAddVirtualKeyword = new wxCheckBox( this, wxID_ANY, _("Prepend 'virtual' keyword to the functions"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_checkBoxAddVirtualKeyword, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* buttonSizer;
	buttonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_buttonOk = new wxButton( this, wxID_OK, _("&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonOk->SetDefault(); 
	buttonSizer->Add( m_buttonOk, 0, wxALL, 5 );
	
	m_buttonCancel = new wxButton( this, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonSizer->Add( m_buttonCancel, 0, wxALL, 5 );
	
	mainSizer->Add( buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_checkBoxAddDoxy->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ImplementParentVirtualFunctionsBase::OnAddDoxy ), NULL, this );
	m_checkBoxAddVirtualKeyword->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ImplementParentVirtualFunctionsBase::OnAddVirtual ), NULL, this );
}

ImplementParentVirtualFunctionsBase::~ImplementParentVirtualFunctionsBase()
{
	// Disconnect Events
	m_checkBoxAddDoxy->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ImplementParentVirtualFunctionsBase::OnAddDoxy ), NULL, this );
	m_checkBoxAddVirtualKeyword->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ImplementParentVirtualFunctionsBase::OnAddVirtual ), NULL, this );
	
}
