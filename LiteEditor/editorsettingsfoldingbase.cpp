///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Aug 25 2009)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "editorsettingsfoldingbase.h"

///////////////////////////////////////////////////////////////////////////

EditorSettingsFoldingBase::EditorSettingsFoldingBase( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_displayMargin = new wxCheckBox( this, wxID_ANY, _("Display Folding Margin"), wxDefaultPosition, wxDefaultSize, 0 );
	m_displayMargin->SetValue(true); 
	mainSizer->Add( m_displayMargin, 0, wxALL, 5 );
	
	m_underlineFolded = new wxCheckBox( this, wxID_ANY, _("Underline Folded Line"), wxDefaultPosition, wxDefaultSize, 0 );
	m_underlineFolded->SetValue(true); 
	mainSizer->Add( m_underlineFolded, 0, wxALL, 5 );
	
	m_foldPreprocessors = new wxCheckBox( this, wxID_ANY, _("Fold Preprocessors"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_foldPreprocessors, 0, wxALL, 5 );
	
	m_foldCompact = new wxCheckBox( this, wxID_ANY, _("Fold Compact"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_foldCompact, 0, wxALL, 5 );
	
	m_foldElse = new wxCheckBox( this, wxID_ANY, _("Fold At Else"), wxDefaultPosition, wxDefaultSize, 0 );
	mainSizer->Add( m_foldElse, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Fold Style"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	mainSizer->Add( m_staticText2, 0, wxALL, 5 );
	
	wxString m_foldStyleChoices[] = { _("Simple"), _("Arrows"), _("Arrows with Background Colour"), _("Simple with Background Colour"), _("Flatten Tree Square Headers"), _("Flatten Tree Circular Headers") };
	int m_foldStyleNChoices = sizeof( m_foldStyleChoices ) / sizeof( wxString );
	m_foldStyle = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_foldStyleNChoices, m_foldStyleChoices, 0 );
	m_foldStyle->SetSelection( 1 );
	mainSizer->Add( m_foldStyle, 0, wxALL|wxEXPAND, 5 );
	
	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 2, 2, 0, 0 );
	
	m_staticText21 = new wxStaticText( this, wxID_ANY, _("Folding background base colour:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText21->Wrap( -1 );
	gSizer1->Add( m_staticText21, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_colourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	gSizer1->Add( m_colourPicker, 0, wxALL|wxEXPAND, 5 );
	
	mainSizer->Add( gSizer1, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_underlineFolded->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldPreprocessors->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldCompact->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldElse->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_staticText2->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldStyle->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_staticText21->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldColourUI ), NULL, this );
	m_colourPicker->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldColourUI ), NULL, this );
}

EditorSettingsFoldingBase::~EditorSettingsFoldingBase()
{
	// Disconnect Events
	m_underlineFolded->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldPreprocessors->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldCompact->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldElse->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_staticText2->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_foldStyle->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldingMarginUI ), NULL, this );
	m_staticText21->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldColourUI ), NULL, this );
	m_colourPicker->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( EditorSettingsFoldingBase::OnFoldColourUI ), NULL, this );
}
