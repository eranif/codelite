///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "qmaketabbase.h"

///////////////////////////////////////////////////////////////////////////

QMakeTabBase::QMakeTabBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxUseQmake = new wxCheckBox( this, wxID_ANY, _("This project uses qmake"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_checkBoxUseQmake, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("qmake settings:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	m_staticText3->SetToolTip( _("Select the QMake to be used for this build configuration as defined in 'Plugins -> QMake -> Settings'") );
	
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_choiceQmakeSettingsChoices;
	m_choiceQmakeSettings = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceQmakeSettingsChoices, 0 );
	m_choiceQmakeSettings->SetSelection( 0 );
	m_choiceQmakeSettings->SetToolTip( _("Select the QMake to be used for this build configuration as defined in 'Plugins -> QMake -> Settings'") );
	
	fgSizer1->Add( m_choiceQmakeSettings, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("qmake execution line:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrlQmakeExeLine = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_textCtrlQmakeExeLine, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("CodeLite will place the below text after the auto generated section (so you may override the generated variables)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	mainSizer->Add( m_staticText5, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrlFreeText = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_PROCESS_TAB|wxTE_RICH2 );
	m_textCtrlFreeText->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 76, 90, 90, false, wxEmptyString ) );
	
	mainSizer->Add( m_textCtrlFreeText, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_staticText3->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_choiceQmakeSettings->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_staticText4->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_textCtrlQmakeExeLine->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_staticText5->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_textCtrlFreeText->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
}

QMakeTabBase::~QMakeTabBase()
{
	// Disconnect Events
	m_staticText3->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_choiceQmakeSettings->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_staticText4->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_textCtrlQmakeExeLine->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_staticText5->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	m_textCtrlFreeText->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( QMakeTabBase::OnUseQmake ), NULL, this );
	
}
