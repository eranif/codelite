///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun 30 2011)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingscaretbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsCaretBase::EditorSettingsCaretBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Caret blink period (milliseconds):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlBlinkPeriod = new wxTextCtrl( this, wxID_ANY, _("500"), wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer1->Add( m_spinCtrlBlinkPeriod, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Caret width (pixels):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_spinCtrlCaretWidth = new wxTextCtrl( this, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, wxTE_RICH2 );
	fgSizer1->Add( m_spinCtrlCaretWidth, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	mainSizer->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	mainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 1, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxScrollBeyondLastLine = new wxCheckBox( this, wxID_ANY, _("Allow caret to scroll beyond end of file"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxScrollBeyondLastLine, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxAdjustScrollbarSize = new wxCheckBox( this, wxID_ANY, _("Auto-adjust horizontal scrollbar width"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxAdjustScrollbarSize, 0, wxALL|wxEXPAND, 5 );
	
	m_checkBoxCaretUseCamelCase = new wxCheckBox( this, wxID_ANY, _("Caret jumps between word segments marked by capitalisation (CamelCase) or underscores"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer2->Add( m_checkBoxCaretUseCamelCase, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( fgSizer2, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
}

EditorSettingsCaretBase::~EditorSettingsCaretBase()
{
}
