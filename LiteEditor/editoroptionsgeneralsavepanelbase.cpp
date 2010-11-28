///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Nov 18 2010)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editoroptionsgeneralsavepanelbase.h"

///////////////////////////////////////////////////////////////////////////

EditorOptionsGeneralSavePanelBase::EditorOptionsGeneralSavePanelBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBoxTrimLine = new wxCheckBox( this, wxID_ANY, _("When saving files, trim empty lines"), wxDefaultPosition, wxDefaultSize, 0 );
	m_checkBoxTrimLine->SetValue(true); 
	bSizer3->Add( m_checkBoxTrimLine, 0, wxALL, 5 );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_checkBoxDontTrimCurrentLine = new wxCheckBox( this, wxID_ANY, _("Do not trim the caret line"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_checkBoxDontTrimCurrentLine, 0, wxRIGHT|wxLEFT, 20 );
	
	bSizer3->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	m_checkBoxAppendLF = new wxCheckBox( this, wxID_ANY, _("If missing, append EOL at end of file"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_checkBoxAppendLF, 0, wxALL, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	bSizer3->Fit( this );
	
	// Connect Events
	m_checkBoxDontTrimCurrentLine->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorOptionsGeneralSavePanelBase::OnTrimCaretLineUI ), NULL, this );
}

EditorOptionsGeneralSavePanelBase::~EditorOptionsGeneralSavePanelBase()
{
	// Disconnect Events
	m_checkBoxDontTrimCurrentLine->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorOptionsGeneralSavePanelBase::OnTrimCaretLineUI ), NULL, this );
	
}
